#include "stratos/HttpServer.h"
#include "stratos/Interpreter.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cstring>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace stratos {

HttpServerManager::~HttpServerManager() {
    // Stop all servers
    for (auto& [id, server] : servers_) {
        if (server && server->running) {
            stopServer(id);
        }
    }
}

// ============================================================================
// Router Management
// ============================================================================

int HttpServerManager::createRouter() {
    std::lock_guard<std::mutex> lock(mutex_);
    int id = nextRouterId_++;
    auto router = std::make_unique<RouterData>();
    router->id = id;
    router->prefix = "";
    routers_[id] = std::move(router);
    return id;
}

RouterData* HttpServerManager::getRouter(int routerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = routers_.find(routerId);
    if (it != routers_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void HttpServerManager::addRoute(int routerId, HttpMethod method, const std::string& pattern, int handlerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = routers_.find(routerId);
    if (it != routers_.end()) {
        HttpRoute route;
        route.method = method;
        route.pattern = it->second->prefix + pattern;
        route.handlerId = handlerId;
        route.isMiddleware = false;
        it->second->routes.push_back(route);
    }
}

void HttpServerManager::addMiddleware(int routerId, int handlerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = routers_.find(routerId);
    if (it != routers_.end()) {
        it->second->middlewareIds.push_back(handlerId);
    }
}

int HttpServerManager::createRouterGroup(int routerId, const std::string& prefix) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = routers_.find(routerId);
    if (it == routers_.end()) {
        return -1;
    }

    int id = nextRouterId_++;
    auto router = std::make_unique<RouterData>();
    router->id = id;
    router->prefix = it->second->prefix + prefix;
    // Copy middleware from parent
    router->middlewareIds = it->second->middlewareIds;
    routers_[id] = std::move(router);
    return id;
}

// ============================================================================
// Server Management
// ============================================================================

int HttpServerManager::createServer(int routerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    int id = nextServerId_++;
    auto server = std::make_unique<ServerData>();
    server->id = id;
    server->routerId = routerId;
    servers_[id] = std::move(server);
    return id;
}

ServerData* HttpServerManager::getServer(int serverId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = servers_.find(serverId);
    if (it != servers_.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool HttpServerManager::startServer(int serverId, int port, Interpreter* interpreter) {
    ServerData* server = getServer(serverId);
    if (!server) {
        std::cerr << "Server not found: " << serverId << std::endl;
        return false;
    }

    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    // Set socket options
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind to port " << port << std::endl;
        close(serverSocket);
        return false;
    }

    // Listen
    if (listen(serverSocket, 128) < 0) {
        std::cerr << "Failed to listen" << std::endl;
        close(serverSocket);
        return false;
    }

    server->serverSocket = serverSocket;
    server->port = port;
    server->running = true;

    // Run server loop (blocking in main thread for simplicity)
    runServerLoop(server, interpreter);

    return true;
}

void HttpServerManager::stopServer(int serverId) {
    ServerData* server = getServer(serverId);
    if (server) {
        server->running = false;
        if (server->serverSocket >= 0) {
            close(server->serverSocket);
            server->serverSocket = -1;
        }
    }
}

// ============================================================================
// Handler Storage
// ============================================================================

int HttpServerManager::storeHandler(const RuntimeValue& handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    int id = nextHandlerId_++;
    handlers_[id] = std::make_unique<RuntimeValue>(handler);
    return id;
}

RuntimeValue* HttpServerManager::getHandler(int handlerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = handlers_.find(handlerId);
    if (it != handlers_.end()) {
        return it->second.get();
    }
    return nullptr;
}

// ============================================================================
// HTTP Parsing
// ============================================================================

HttpRequestInternal HttpServerManager::parseRequest(const std::string& rawRequest) {
    HttpRequestInternal req;
    std::istringstream stream(rawRequest);
    std::string line;

    // Parse request line
    if (std::getline(stream, line)) {
        // Remove \r if present
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        std::istringstream requestLine(line);
        std::string methodStr;
        requestLine >> methodStr >> req.url >> req.httpVersion;
        req.method = stringToMethod(methodStr);

        // Parse path and query string
        size_t queryPos = req.url.find('?');
        if (queryPos != std::string::npos) {
            req.path = req.url.substr(0, queryPos);
            req.query = req.url.substr(queryPos + 1);

            // Parse query parameters
            std::istringstream queryStream(req.query);
            std::string param;
            while (std::getline(queryStream, param, '&')) {
                size_t eqPos = param.find('=');
                if (eqPos != std::string::npos) {
                    req.queryParams[param.substr(0, eqPos)] = param.substr(eqPos + 1);
                }
            }
        } else {
            req.path = req.url;
        }
    }

    // Parse headers
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {
            break;  // End of headers
        }

        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            // Trim leading whitespace from value
            size_t start = value.find_first_not_of(" \t");
            if (start != std::string::npos) {
                value = value.substr(start);
            }
            req.headers[key] = value;
        }
    }

    // Parse body (remaining content)
    std::stringstream bodyStream;
    bodyStream << stream.rdbuf();
    req.body = bodyStream.str();

    return req;
}

bool HttpServerManager::matchRoute(const std::string& pattern, const std::string& path,
                                   std::unordered_map<std::string, std::string>& params) {
    // Handle wildcard
    if (pattern == "*") {
        return true;
    }

    // Split pattern and path into segments
    std::vector<std::string> patternParts, pathParts;

    std::istringstream patternStream(pattern);
    std::istringstream pathStream(path);
    std::string part;

    while (std::getline(patternStream, part, '/')) {
        if (!part.empty()) {
            patternParts.push_back(part);
        }
    }

    while (std::getline(pathStream, part, '/')) {
        if (!part.empty()) {
            pathParts.push_back(part);
        }
    }

    // Check if lengths match (unless pattern ends with wildcard)
    if (patternParts.size() != pathParts.size()) {
        // Check if last pattern part is wildcard
        if (!patternParts.empty() && patternParts.back() == "*") {
            // Allow path to be longer
            if (pathParts.size() < patternParts.size() - 1) {
                return false;
            }
        } else {
            return false;
        }
    }

    // Match each segment
    for (size_t i = 0; i < patternParts.size(); ++i) {
        const std::string& pp = patternParts[i];

        if (pp == "*") {
            // Wildcard matches rest
            return true;
        }

        if (i >= pathParts.size()) {
            return false;
        }

        if (pp.size() > 0 && pp[0] == ':') {
            // Parameter - extract value
            std::string paramName = pp.substr(1);
            params[paramName] = pathParts[i];
        } else {
            // Literal match
            if (pp != pathParts[i]) {
                return false;
            }
        }
    }

    return true;
}

// ============================================================================
// Server Loop
// ============================================================================

void HttpServerManager::runServerLoop(ServerData* server, Interpreter* interpreter) {
    while (server->running) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        // Use poll to check for connections with timeout
        struct pollfd pfd;
        pfd.fd = server->serverSocket;
        pfd.events = POLLIN;

        int pollResult = poll(&pfd, 1, 1000);  // 1 second timeout

        if (pollResult < 0) {
            if (errno == EINTR) continue;
            break;
        }

        if (pollResult == 0) {
            // Timeout - check if still running
            continue;
        }

        int clientSocket = accept(server->serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            if (errno == EINTR) continue;
            break;
        }

        // Get client address
        char clientAddrStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientAddrStr, INET_ADDRSTRLEN);

        // Handle client in same thread (simple implementation)
        handleClient(clientSocket, server, interpreter);

        close(clientSocket);
    }
}

void HttpServerManager::handleClient(int clientSocket, ServerData* server, Interpreter* interpreter) {
    // Read request
    char buffer[8192];
    std::string rawRequest;

    // Simple read - in production would need more robust handling
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        return;
    }
    buffer[bytesRead] = '\0';
    rawRequest = buffer;

    // Parse request
    HttpRequestInternal req = parseRequest(rawRequest);

    // Create response
    HttpResponseInternal res;

    // Get router
    RouterData* router = getRouter(server->routerId);
    if (!router) {
        res.statusCode = 500;
        res.body = "Server configuration error";
        std::string response = res.build();
        send(clientSocket, response.c_str(), response.size(), 0);
        return;
    }

    // Find matching route
    bool routeFound = false;
    HttpRoute* matchedRoute = nullptr;

    for (auto& route : router->routes) {
        if (route.method == req.method || route.pattern == "*") {
            std::unordered_map<std::string, std::string> params;
            if (matchRoute(route.pattern, req.path, params)) {
                req.pathParams = params;
                matchedRoute = &route;
                routeFound = true;
                break;
            }
        }
    }

    if (!routeFound) {
        res.statusCode = 404;
        res.headers["Content-Type"] = "application/json";
        res.body = "{\"error\": \"Not Found\"}";
        std::string response = res.build();
        send(clientSocket, response.c_str(), response.size(), 0);
        return;
    }

    // Execute handler through interpreter
    if (interpreter && matchedRoute) {
        RuntimeValue* handler = getHandler(matchedRoute->handlerId);
        if (handler) {
            // Create Request and Response RuntimeValues and call the handler
            // This is done through the interpreter's executeHttpHandler method
            interpreter->executeHttpHandler(*handler, req, res);
        }
    }

    // Send response
    std::string response = res.build();
    send(clientSocket, response.c_str(), response.size(), 0);
}

} // namespace stratos
