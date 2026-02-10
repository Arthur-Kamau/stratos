#include "stratos/DevToolsServer.h"
#include "stratos/MemoryProfiler.h"
#include "stratos/DebugEngine.h"
#include "stratos/NetworkMonitor.h"
#include "stratos/Profiler.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <algorithm>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;

    // Platform-specific socket functions
    inline int socket_close(int sock) { return closesocket(sock); }
    inline int socket_read(int sock, char* buf, int len) { return recv(sock, buf, len, 0); }
    inline int socket_write(int sock, const char* buf, int len) { return send(sock, buf, len, 0); }
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>

    // Platform-specific socket functions
    inline int socket_close(int sock) { return close(sock); }
    inline int socket_read(int sock, char* buf, int len) { return read(sock, buf, len); }
    inline int socket_write(int sock, const char* buf, int len) { return write(sock, buf, len); }
#endif

namespace stratos {

// Simple JSON value extractor helpers
static std::string jsonGetString(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";
    pos += search.size();
    while (pos < json.size() && json[pos] == ' ') pos++;
    if (pos >= json.size() || json[pos] != '"') return "";
    pos++; // skip opening quote
    std::string result;
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\' && pos + 1 < json.size()) { pos++; }
        result += json[pos++];
    }
    return result;
}

static int jsonGetInt(const std::string& json, const std::string& key, int defaultVal = 0) {
    std::string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return defaultVal;
    pos += search.size();
    while (pos < json.size() && json[pos] == ' ') pos++;
    size_t end = json.find_first_of(",}", pos);
    if (end == std::string::npos) return defaultVal;
    try { return std::stoi(json.substr(pos, end - pos)); } catch (...) { return defaultVal; }
}

// Escape a string for JSON output
static std::string jsonEscape(const std::string& str) {
    std::string result;
    result.reserve(str.size() + 10);
    for (char c : str) {
        switch (c) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:   result += c; break;
        }
    }
    return result;
}

// ============================================================================
// DevToolsSink Implementation
// ============================================================================

DevToolsSink::DevToolsSink(DevToolsServer* server) : server_(server) {}

void DevToolsSink::write(const LogEntry& entry) {
    if (!server_) {
        std::cerr << "[DevToolsSink] ERROR: server_ is null!\n";
        return;
    }

    std::string json = entryToJson(entry);
    std::cerr << "[DevToolsSink] Broadcasting event: Log.entryAdded\n";
    server_->broadcastEvent("Log.entryAdded", "{\"entry\":" + json + "}");
}

std::string DevToolsSink::entryToJson(const LogEntry& entry) {
    std::ostringstream oss;

    // Convert timestamp to milliseconds since epoch
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()).count();

    oss << "{";
    oss << "\"timestamp\":" << ms << ",";
    oss << "\"level\":\"" << logLevelToString(entry.level) << "\",";

    // Escape message
    oss << "\"message\":\"";
    for (char c : entry.message) {
        if (c == '"') oss << "\\\"";
        else if (c == '\\') oss << "\\\\";
        else if (c == '\n') oss << "\\n";
        else if (c == '\r') oss << "\\r";
        else if (c == '\t') oss << "\\t";
        else oss << c;
    }
    oss << "\"";

    // Source location
    if (!entry.source.file.empty()) {
        oss << ",\"source\":{";
        oss << "\"file\":\"" << entry.source.file << "\",";
        oss << "\"line\":" << entry.source.line;
        if (!entry.source.function.empty()) {
            oss << ",\"function\":\"" << entry.source.function << "\"";
        }
        oss << "}";
    }

    // Additional data
    if (!entry.data.empty()) {
        oss << ",\"data\":{";
        bool first = true;
        for (const auto& [key, value] : entry.data) {
            if (!first) oss << ",";
            oss << "\"" << key << "\":\"" << value << "\"";
            first = false;
        }
        oss << "}";
    }

    oss << "}";
    return oss.str();
}

// ============================================================================
// DevToolsServer Implementation
// ============================================================================

DevToolsServer::DevToolsServer(int port)
    : port_(port), serverSocket_(-1), running_(false), nextEventId_(1) {
    initializeHandlers();
}

DevToolsServer::~DevToolsServer() {
    stop();
}

void DevToolsServer::start() {
    if (running_) {
        std::cerr << "[DevTools] Server already running" << std::endl;
        return;
    }

    // Create socket
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ < 0) {
        std::cerr << "[DevTools] Failed to create socket" << std::endl;
        return;
    }

    // Set socket options
    int opt = 1;
    setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    // Bind to port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(serverSocket_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "[DevTools] Failed to bind to port " << port_ << std::endl;
        socket_close(serverSocket_);
        serverSocket_ = -1;
        return;
    }

    // Listen
    if (listen(serverSocket_, 5) < 0) {
        std::cerr << "[DevTools] Failed to listen" << std::endl;
        socket_close(serverSocket_);
        serverSocket_ = -1;
        return;
    }

    running_ = true;
    serverThread_ = std::thread(&DevToolsServer::serverLoop, this);

    std::cout << "[DevTools] Server started on port " << port_ << std::endl;
    std::cout << "[DevTools] Open http://localhost:8080 to connect" << std::endl;
}

void DevToolsServer::stop() {
    if (!running_) return;

    running_ = false;

    // Close server socket
    if (serverSocket_ >= 0) {
        socket_close(serverSocket_);
        serverSocket_ = -1;
    }

    // Wait for thread
    if (serverThread_.joinable()) {
        serverThread_.join();
    }

    std::cout << "[DevTools] Server stopped" << std::endl;
}

bool DevToolsServer::isRunning() const {
    return running_;
}

void DevToolsServer::serverLoop() {
    while (running_) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        // Accept with timeout
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(serverSocket_, &readfds);

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int activity = select(serverSocket_ + 1, &readfds, NULL, NULL, &tv);

        if (activity < 0) {
            if (running_) {
                std::cerr << "[DevTools] Select error" << std::endl;
            }
            break;
        }

        if (activity == 0) {
            // Timeout, check running flag
            continue;
        }

        int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            if (running_) {
                std::cerr << "[DevTools] Failed to accept connection" << std::endl;
            }
            continue;
        }

        // Handle client in a new thread (simple approach)
        std::thread([this, clientSocket]() {
            try {
                handleClient(clientSocket);
            } catch (const std::exception& e) {
                std::cerr << "[DevTools] Error handling client: " << e.what() << std::endl;
                socket_close(clientSocket);
            } catch (...) {
                std::cerr << "[DevTools] Unknown error handling client" << std::endl;
                socket_close(clientSocket);
            }
        }).detach();
    }
}

void DevToolsServer::handleClient(int clientSocket) {
    char buffer[8192];
    int bytesRead = socket_read(clientSocket, buffer, sizeof(buffer) - 1);

    if (bytesRead <= 0) {
        socket_close(clientSocket);
        return;
    }

    buffer[bytesRead] = '\0';
    std::string request(buffer);

    // Parse HTTP request
    std::string body = parseHttpRequest(request);
    std::string response;

    // Check if it's an event polling request
    if (request.find("GET /events") != std::string::npos) {
        // Return pending events
        std::lock_guard<std::mutex> lock(eventsMutex_);

        // Remove events older than 5 seconds (allows multiple tabs to poll)
        auto now = std::chrono::system_clock::now();
        auto fiveSecondsAgo = now - std::chrono::seconds(5);

        pendingEvents_.erase(
            std::remove_if(pendingEvents_.begin(), pendingEvents_.end(),
                [fiveSecondsAgo](const JsonRpcEvent& e) {
                    return e.timestamp < fiveSecondsAgo;
                }),
            pendingEvents_.end()
        );

        std::cerr << "[DevToolsServer] /events polled. Sending " << pendingEvents_.size() << " events\n";

        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < pendingEvents_.size(); ++i) {
            if (i > 0) oss << ",";
            oss << "{\"jsonrpc\":\"" << pendingEvents_[i].jsonrpc << "\",";
            oss << "\"id\":" << pendingEvents_[i].id << ",";
            oss << "\"method\":\"" << pendingEvents_[i].method << "\",";
            oss << "\"params\":" << pendingEvents_[i].params << "}";
        }
        oss << "]";

        std::string jsonResponse = oss.str();
        std::cerr << "[DevToolsServer] JSON Response: " << jsonResponse.substr(0, 200) << "...\n";

        // Don't clear immediately - let time-based expiry handle it
        // This allows multiple browser tabs to receive the same events

        response = createHttpResponse(jsonResponse);
    }
    // Handle JSON-RPC requests
    else if (!body.empty() && body.find("jsonrpc") != std::string::npos) {
        std::string result = handleRequest(body);
        response = createHttpResponse(result);
    }
    // Handle CORS preflight
    else if (request.find("OPTIONS ") != std::string::npos) {
        response = createHttpResponse("", "text/plain");
    }
    // Serve UI files for GET requests
    else if (request.find("GET /") != std::string::npos) {
        // Extract the path from GET /path HTTP/1.1
        size_t pathStart = request.find("GET ") + 4;
        size_t pathEnd = request.find(" HTTP/", pathStart);
        std::string urlPath = request.substr(pathStart, pathEnd - pathStart);

        // Default to index.html for root
        if (urlPath == "/" || urlPath.empty()) urlPath = "/index.html";

        // Try to serve from devtools/ui/ directory
        // Look for the UI directory relative to common locations
        std::vector<std::string> searchDirs = {
            "devtools/ui",
            "../devtools/ui",
            "../../devtools/ui",
            "../../../devtools/ui",
        };

        bool fileServed = false;
        for (const auto& dir : searchDirs) {
            std::string filePath = dir + urlPath;
            if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)) {
                std::ifstream file(filePath, std::ios::binary);
                if (file.is_open()) {
                    std::string content((std::istreambuf_iterator<char>(file)),
                                         std::istreambuf_iterator<char>());

                    // Determine content type
                    std::string contentType = "text/plain";
                    if (urlPath.ends_with(".html")) contentType = "text/html";
                    else if (urlPath.ends_with(".css")) contentType = "text/css";
                    else if (urlPath.ends_with(".js")) contentType = "application/javascript";
                    else if (urlPath.ends_with(".json")) contentType = "application/json";
                    else if (urlPath.ends_with(".svg")) contentType = "image/svg+xml";
                    else if (urlPath.ends_with(".png")) contentType = "image/png";

                    response = createHttpResponse(content, contentType);
                    fileServed = true;
                    break;
                }
            }
        }

        if (!fileServed) {
            // Fallback: API info
            if (urlPath == "/index.html" || urlPath == "/") {
                std::string info = "{\"name\":\"Stratos DevTools\",\"version\":\"1.0.0\",\"protocol\":\"1.0.0\",\"message\":\"DevTools UI not found. Place UI files in devtools/ui/\"}";
                response = createHttpResponse(info);
            } else {
                response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            }
        }
    }
    else {
        std::string errorMsg = "{\"error\":\"Invalid request\"}";
        response = createHttpResponse(errorMsg);
    }

    // Send response
    socket_write(clientSocket, response.c_str(), response.length());
    socket_close(clientSocket);
}

std::string DevToolsServer::parseHttpRequest(const std::string& data) {
    // Find the body (after \r\n\r\n)
    size_t bodyStart = data.find("\r\n\r\n");
    if (bodyStart == std::string::npos) {
        return "";
    }

    return data.substr(bodyStart + 4);
}

std::string DevToolsServer::createHttpResponse(const std::string& content, const std::string& contentType) {
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Content-Length: " << content.length() << "\r\n";
    oss << "Access-Control-Allow-Origin: *\r\n";
    oss << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    oss << "Access-Control-Allow-Headers: Content-Type\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    oss << content;
    return oss.str();
}

std::string DevToolsServer::handleRequest(const std::string& request) {
    // Simple JSON-RPC parsing
    // Extract method, params, and id
    std::string method;
    std::string params = "{}";
    int id = 1;

    // Very basic parsing - in production, use a JSON library
    size_t methodPos = request.find("\"method\":");
    if (methodPos != std::string::npos) {
        size_t methodStart = request.find("\"", methodPos + 9) + 1;
        size_t methodEnd = request.find("\"", methodStart);
        method = request.substr(methodStart, methodEnd - methodStart);
    }

    size_t idPos = request.find("\"id\":");
    if (idPos != std::string::npos) {
        size_t idStart = idPos + 5;
        size_t idEnd = request.find_first_of(",}", idStart);
        std::string idStr = request.substr(idStart, idEnd - idStart);
        try { id = std::stoi(idStr); } catch (...) {}
    }

    // Extract params object
    size_t paramsPos = request.find("\"params\":");
    if (paramsPos != std::string::npos) {
        size_t paramsStart = paramsPos + 9;
        // Skip whitespace
        while (paramsStart < request.size() && request[paramsStart] == ' ') paramsStart++;
        if (paramsStart < request.size() && request[paramsStart] == '{') {
            // Find matching closing brace
            int depth = 0;
            size_t paramsEnd = paramsStart;
            for (size_t i = paramsStart; i < request.size(); ++i) {
                if (request[i] == '{') depth++;
                else if (request[i] == '}') { depth--; if (depth == 0) { paramsEnd = i + 1; break; } }
            }
            params = request.substr(paramsStart, paramsEnd - paramsStart);
        }
    }

    // Handle request
    std::lock_guard<std::mutex> lock(handlersMutex_);
    auto it = handlers_.find(method);

    std::ostringstream response;
    response << "{\"jsonrpc\":\"2.0\",\"id\":" << id << ",";

    if (it != handlers_.end()) {
        try {
            std::string result = it->second(params);
            response << "\"result\":" << result << "}";
        } catch (const std::exception& e) {
            response << "\"error\":{\"code\":-32603,\"message\":\"" << e.what() << "\"}}";
        }
    } else {
        response << "\"error\":{\"code\":-32601,\"message\":\"Method not found\"}}";
    }

    return response.str();
}

void DevToolsServer::broadcastEvent(const std::string& method, const std::string& params) {
    std::lock_guard<std::mutex> lock(eventsMutex_);

    JsonRpcEvent event;
    event.id = nextEventId_++;
    event.method = method;
    event.params = params;
    event.timestamp = std::chrono::system_clock::now();

    pendingEvents_.push_back(event);
    std::cerr << "[DevToolsServer] Event added. Queue size: " << pendingEvents_.size() << "\n";

    // Keep only last 1000 events
    if (pendingEvents_.size() > 1000) {
        pendingEvents_.erase(pendingEvents_.begin(), pendingEvents_.begin() + 500);
    }
}

void DevToolsServer::registerHandler(const std::string& method, RequestHandler handler) {
    std::lock_guard<std::mutex> lock(handlersMutex_);
    handlers_[method] = handler;
}

void DevToolsServer::initializeHandlers() {
    // Runtime.getVersion
    registerHandler("Runtime.getVersion", [](const std::string&) -> std::string {
        return "{\"version\":\"0.1.0\",\"protocolVersion\":\"1.0.0\",\"buildDate\":\"2025-12-30\"}";
    });

    // Log.enable
    registerHandler("Log.enable", [](const std::string&) -> std::string {
        return "{}";
    });

    // Log.disable
    registerHandler("Log.disable", [](const std::string&) -> std::string {
        return "{}";
    });

    // Log.clear
    registerHandler("Log.clear", [](const std::string&) -> std::string {
        // Clear buffer sink
        auto bufferSink = Logger::instance().getBufferSink();
        if (bufferSink) {
            bufferSink->clear();
        }
        return "{}";
    });

    // Memory.enable
    registerHandler("Memory.enable", [](const std::string&) -> std::string {
        MemoryProfiler::instance().enable();
        return "{}";
    });

    // Memory.disable
    registerHandler("Memory.disable", [](const std::string&) -> std::string {
        MemoryProfiler::instance().disable();
        return "{}";
    });

    // Memory.getStats
    registerHandler("Memory.getStats", [](const std::string&) -> std::string {
        auto stats = MemoryProfiler::instance().getStats();

        std::ostringstream oss;
        oss << "{";
        oss << "\"totalAllocated\":" << stats.totalAllocated << ",";
        oss << "\"currentUsage\":" << stats.currentUsage << ",";
        oss << "\"objectCount\":" << stats.objectCount << ",";

        // objectsByType
        oss << "\"objectsByType\":{";
        bool first = true;
        for (const auto& [type, count] : stats.objectsByType) {
            if (!first) oss << ",";
            oss << "\"" << type << "\":" << count;
            first = false;
        }
        oss << "},";

        // gcStats
        auto lastMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            stats.lastCollectionTime.time_since_epoch()).count();
        oss << "\"gcStats\":{";
        oss << "\"collectionsTotal\":" << stats.collectionsTotal << ",";
        oss << "\"cyclesBroken\":" << stats.cyclesBrokenTotal << ",";
        oss << "\"lastCollectionTime\":" << lastMs << ",";
        oss << "\"avgPauseTime\":" << stats.avgPauseTimeMs;
        oss << "}";

        oss << "}";
        return oss.str();
    });

    // Memory.getGCHistory
    registerHandler("Memory.getGCHistory", [](const std::string&) -> std::string {
        auto history = MemoryProfiler::instance().getGCHistory(50);

        std::ostringstream oss;
        oss << "{\"events\":[";
        for (size_t i = 0; i < history.size(); ++i) {
            if (i > 0) oss << ",";

            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                history[i].timestamp.time_since_epoch()).count();

            oss << "{";
            oss << "\"timestamp\":" << ms << ",";
            oss << "\"pauseTime\":" << history[i].pauseTimeMs << ",";
            oss << "\"freedBytes\":" << history[i].freedBytes << ",";
            oss << "\"freedObjects\":" << history[i].freedObjects << ",";
            oss << "\"cyclesBroken\":" << history[i].cyclesBroken;
            oss << "}";
        }
        oss << "]}";
        return oss.str();
    });

    // ========================================================================
    // Debugger Domain
    // ========================================================================

    // Debugger.enable
    registerHandler("Debugger.enable", [](const std::string&) -> std::string {
        DebugEngine::instance().enable();
        return "{}";
    });

    // Debugger.disable
    registerHandler("Debugger.disable", [](const std::string&) -> std::string {
        DebugEngine::instance().disable();
        return "{}";
    });

    // Debugger.setBreakpoint - params: {"file":"...", "line":N}
    registerHandler("Debugger.setBreakpoint", [](const std::string& params) -> std::string {
        std::string file = jsonGetString(params, "file");
        int line = jsonGetInt(params, "line");
        std::string condition = jsonGetString(params, "condition");
        int bpId = DebugEngine::instance().setBreakpoint(file, line, condition);
        return "{\"breakpointId\":" + std::to_string(bpId) + "}";
    });

    // Debugger.removeBreakpoint - params: {"breakpointId":N}
    registerHandler("Debugger.removeBreakpoint", [](const std::string& params) -> std::string {
        int bpId = jsonGetInt(params, "breakpointId");
        bool ok = DebugEngine::instance().removeBreakpoint(bpId);
        return ok ? "{\"removed\":true}" : "{\"removed\":false}";
    });

    // Debugger.getBreakpoints
    registerHandler("Debugger.getBreakpoints", [](const std::string&) -> std::string {
        auto bps = DebugEngine::instance().getBreakpoints();
        std::ostringstream oss;
        oss << "{\"breakpoints\":[";
        for (size_t i = 0; i < bps.size(); ++i) {
            if (i > 0) oss << ",";
            oss << "{\"id\":" << bps[i].id;
            oss << ",\"file\":\"" << jsonEscape(bps[i].file) << "\"";
            oss << ",\"line\":" << bps[i].line;
            oss << ",\"enabled\":" << (bps[i].enabled ? "true" : "false");
            oss << ",\"hitCount\":" << bps[i].hitCount << "}";
        }
        oss << "]}";
        return oss.str();
    });

    // Debugger.pause
    registerHandler("Debugger.pause", [](const std::string&) -> std::string {
        DebugEngine::instance().pause();
        return "{}";
    });

    // Debugger.resume
    registerHandler("Debugger.resume", [](const std::string&) -> std::string {
        DebugEngine::instance().resume();
        return "{}";
    });

    // Debugger.stepOver
    registerHandler("Debugger.stepOver", [](const std::string&) -> std::string {
        DebugEngine::instance().stepOver();
        return "{}";
    });

    // Debugger.stepInto
    registerHandler("Debugger.stepInto", [](const std::string&) -> std::string {
        DebugEngine::instance().stepInto();
        return "{}";
    });

    // Debugger.stepOut
    registerHandler("Debugger.stepOut", [](const std::string&) -> std::string {
        DebugEngine::instance().stepOut();
        return "{}";
    });

    // Debugger.getCallStack
    registerHandler("Debugger.getCallStack", [](const std::string&) -> std::string {
        auto stack = DebugEngine::instance().getCallStack();
        std::ostringstream oss;
        oss << "{\"callFrames\":[";
        for (size_t i = 0; i < stack.size(); ++i) {
            if (i > 0) oss << ",";
            oss << "{\"frameId\":" << stack[i].frameId;
            oss << ",\"functionName\":\"" << jsonEscape(stack[i].functionName) << "\"";
            oss << ",\"file\":\"" << jsonEscape(stack[i].file) << "\"";
            oss << ",\"line\":" << stack[i].line << "}";
        }
        oss << "]}";
        return oss.str();
    });

    // Debugger.getVariables - params: {"frameId":N}
    registerHandler("Debugger.getVariables", [](const std::string& params) -> std::string {
        int frameId = jsonGetInt(params, "frameId");
        auto vars = DebugEngine::instance().getVariables(frameId);
        std::ostringstream oss;
        oss << "{\"variables\":[";
        for (size_t i = 0; i < vars.size(); ++i) {
            if (i > 0) oss << ",";
            oss << "{\"name\":\"" << jsonEscape(vars[i].name) << "\"";
            oss << ",\"value\":\"" << jsonEscape(vars[i].value) << "\"";
            oss << ",\"type\":\"" << jsonEscape(vars[i].type) << "\"}";
        }
        oss << "]}";
        return oss.str();
    });

    // Debugger.evaluateOnFrame - params: {"expression":"...", "frameId":N}
    registerHandler("Debugger.evaluateOnFrame", [](const std::string& params) -> std::string {
        std::string expr = jsonGetString(params, "expression");
        int frameId = jsonGetInt(params, "frameId");
        std::string result = DebugEngine::instance().evaluateExpression(expr, frameId);
        return "{\"result\":" + result + "}";
    });

    // Debugger.getSource - params: {"file":"..."}
    registerHandler("Debugger.getSource", [](const std::string& params) -> std::string {
        std::string file = jsonGetString(params, "file");
        std::string content = DebugEngine::instance().getSourceContent(file);
        return "{\"source\":\"" + jsonEscape(content) + "\"}";
    });

    // Debugger.getSourceFiles
    registerHandler("Debugger.getSourceFiles", [](const std::string&) -> std::string {
        auto files = DebugEngine::instance().getSourceFiles();
        std::ostringstream oss;
        oss << "{\"files\":[";
        for (size_t i = 0; i < files.size(); ++i) {
            if (i > 0) oss << ",";
            oss << "\"" << jsonEscape(files[i]) << "\"";
        }
        oss << "]}";
        return oss.str();
    });

    // Debugger.getState
    registerHandler("Debugger.getState", [](const std::string&) -> std::string {
        auto& engine = DebugEngine::instance();
        std::string stateStr;
        switch (engine.getState()) {
            case DebugState::Running: stateStr = "running"; break;
            case DebugState::Paused: stateStr = "paused"; break;
            case DebugState::Stepping: stateStr = "stepping"; break;
        }
        std::ostringstream oss;
        oss << "{\"state\":\"" << stateStr << "\"";
        oss << ",\"file\":\"" << jsonEscape(engine.getCurrentFile()) << "\"";
        oss << ",\"line\":" << engine.getCurrentLine();
        oss << ",\"enabled\":" << (engine.isEnabled() ? "true" : "false") << "}";
        return oss.str();
    });

    // ========================================================================
    // Network Domain
    // ========================================================================

    registerHandler("Network.enable", [](const std::string&) -> std::string {
        NetworkMonitor::instance().enable();
        return "{}";
    });

    registerHandler("Network.disable", [](const std::string&) -> std::string {
        NetworkMonitor::instance().disable();
        return "{}";
    });

    registerHandler("Network.getRequests", [](const std::string&) -> std::string {
        auto requests = NetworkMonitor::instance().getRequests();
        std::ostringstream oss;
        oss << "{\"requests\":[";
        for (size_t i = 0; i < requests.size(); ++i) {
            if (i > 0) oss << ",";
            const auto& req = requests[i];
            oss << "{\"id\":" << req.id;
            oss << ",\"url\":\"" << jsonEscape(req.url) << "\"";
            oss << ",\"method\":\"" << req.method << "\"";
            oss << ",\"status\":" << req.responseStatus;
            oss << ",\"statusText\":\"" << jsonEscape(req.responseStatusText) << "\"";
            oss << ",\"size\":" << req.responseSize;
            oss << ",\"startTime\":" << req.startTimeMs;
            oss << ",\"duration\":" << req.durationMs;
            oss << ",\"type\":\"" << req.contentType << "\"";
            oss << ",\"completed\":" << (req.completed ? "true" : "false") << "}";
        }
        oss << "]}";
        return oss.str();
    });

    registerHandler("Network.getRequestDetail", [](const std::string& params) -> std::string {
        int reqId = jsonGetInt(params, "id");
        auto detail = NetworkMonitor::instance().getRequestDetail(reqId);
        if (!detail) return "{\"error\":\"Request not found\"}";

        std::ostringstream oss;
        oss << "{\"id\":" << detail->id;
        oss << ",\"url\":\"" << jsonEscape(detail->url) << "\"";
        oss << ",\"method\":\"" << detail->method << "\"";
        oss << ",\"status\":" << detail->responseStatus;

        // Request headers
        oss << ",\"requestHeaders\":{";
        bool first = true;
        for (const auto& [k, v] : detail->requestHeaders) {
            if (!first) oss << ",";
            oss << "\"" << jsonEscape(k) << "\":\"" << jsonEscape(v) << "\"";
            first = false;
        }
        oss << "}";

        // Response headers
        oss << ",\"responseHeaders\":{";
        first = true;
        for (const auto& [k, v] : detail->responseHeaders) {
            if (!first) oss << ",";
            oss << "\"" << jsonEscape(k) << "\":\"" << jsonEscape(v) << "\"";
            first = false;
        }
        oss << "}";

        oss << ",\"responseBody\":\"" << jsonEscape(detail->responseBody) << "\"";
        oss << ",\"duration\":" << detail->durationMs;
        oss << ",\"size\":" << detail->responseSize << "}";
        return oss.str();
    });

    registerHandler("Network.clear", [](const std::string&) -> std::string {
        NetworkMonitor::instance().clear();
        return "{}";
    });

    // ========================================================================
    // Profiler Domain
    // ========================================================================

    registerHandler("Profiler.enable", [](const std::string&) -> std::string {
        Profiler::instance().enable();
        return "{}";
    });

    registerHandler("Profiler.disable", [](const std::string&) -> std::string {
        Profiler::instance().disable();
        return "{}";
    });

    registerHandler("Profiler.start", [](const std::string&) -> std::string {
        Profiler::instance().startProfiling();
        return "{}";
    });

    registerHandler("Profiler.stop", [](const std::string&) -> std::string {
        Profiler::instance().stopProfiling();
        return "{}";
    });

    registerHandler("Profiler.getProfile", [](const std::string&) -> std::string {
        auto profile = Profiler::instance().getProfile();
        std::ostringstream oss;
        oss << "{\"duration\":" << profile.durationMs;
        oss << ",\"totalSamples\":" << profile.totalSamples;

        // Functions table
        oss << ",\"functions\":[";
        for (size_t i = 0; i < profile.functions.size(); ++i) {
            if (i > 0) oss << ",";
            const auto& fn = profile.functions[i];
            oss << "{\"name\":\"" << jsonEscape(fn.name) << "\"";
            oss << ",\"file\":\"" << jsonEscape(fn.file) << "\"";
            oss << ",\"selfTime\":" << fn.selfTimeMs;
            oss << ",\"totalTime\":" << fn.totalTimeMs;
            oss << ",\"calls\":" << fn.callCount;
            oss << ",\"selfPercent\":" << fn.selfPercent << "}";
        }
        oss << "]";

        // Flame graph nodes
        oss << ",\"flameGraph\":[";
        for (size_t i = 0; i < profile.flameNodes.size(); ++i) {
            if (i > 0) oss << ",";
            const auto& node = profile.flameNodes[i];
            oss << "{\"name\":\"" << jsonEscape(node.name) << "\"";
            oss << ",\"value\":" << node.value;
            oss << ",\"depth\":" << node.depth;
            oss << ",\"children\":[";
            for (size_t j = 0; j < node.children.size(); ++j) {
                if (j > 0) oss << ",";
                oss << node.children[j];
            }
            oss << "]}";
        }
        oss << "]}";
        return oss.str();
    });
}

void DevToolsServer::addClient(int clientId) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    clients_.push_back(clientId);
}

void DevToolsServer::removeClient(int clientId) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    clients_.erase(std::remove(clients_.begin(), clients_.end(), clientId), clients_.end());
}

std::vector<int> DevToolsServer::getClients() const {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    return clients_;
}

} // namespace stratos
