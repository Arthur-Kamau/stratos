#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>

namespace stratos {

// Forward declarations
struct RuntimeValue;
class Interpreter;

// HTTP Methods enum (mirrors HttpMethod in Stratos)
enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE_,  // Avoid conflict with delete keyword
    PATCH,
    HEAD,
    OPTIONS
};

// Convert string to HttpMethod
inline HttpMethod stringToMethod(const std::string& method) {
    if (method == "GET") return HttpMethod::GET;
    if (method == "POST") return HttpMethod::POST;
    if (method == "PUT") return HttpMethod::PUT;
    if (method == "DELETE") return HttpMethod::DELETE_;
    if (method == "PATCH") return HttpMethod::PATCH;
    if (method == "HEAD") return HttpMethod::HEAD;
    if (method == "OPTIONS") return HttpMethod::OPTIONS;
    return HttpMethod::GET;  // Default
}

inline std::string methodToString(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET: return "GET";
        case HttpMethod::POST: return "POST";
        case HttpMethod::PUT: return "PUT";
        case HttpMethod::DELETE_: return "DELETE";
        case HttpMethod::PATCH: return "PATCH";
        case HttpMethod::HEAD: return "HEAD";
        case HttpMethod::OPTIONS: return "OPTIONS";
        default: return "GET";
    }
}

// Internal HTTP Request structure for parsing
struct HttpRequestInternal {
    HttpMethod method = HttpMethod::GET;
    std::string url;
    std::string path;
    std::string query;
    std::string httpVersion;
    std::unordered_map<std::string, std::string> headers;
    std::unordered_map<std::string, std::string> queryParams;
    std::unordered_map<std::string, std::string> pathParams;
    std::string body;
    std::string remoteAddr;
};

// Internal HTTP Response structure
struct HttpResponseInternal {
    int statusCode = 200;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    bool written = false;

    HttpResponseInternal() {
        headers["Content-Type"] = "text/plain";
    }

    std::string build() const {
        std::string response = "HTTP/1.1 " + std::to_string(statusCode) + " " + getStatusText() + "\r\n";

        // Add Content-Length if body exists
        std::string finalBody = body;
        response += "Content-Length: " + std::to_string(finalBody.size()) + "\r\n";

        // Add headers
        for (const auto& [key, value] : headers) {
            response += key + ": " + value + "\r\n";
        }

        response += "\r\n";
        response += finalBody;

        return response;
    }

    std::string getStatusText() const {
        switch (statusCode) {
            case 200: return "OK";
            case 201: return "Created";
            case 204: return "No Content";
            case 301: return "Moved Permanently";
            case 302: return "Found";
            case 304: return "Not Modified";
            case 400: return "Bad Request";
            case 401: return "Unauthorized";
            case 403: return "Forbidden";
            case 404: return "Not Found";
            case 405: return "Method Not Allowed";
            case 500: return "Internal Server Error";
            case 502: return "Bad Gateway";
            case 503: return "Service Unavailable";
            default: return "OK";
        }
    }
};

// Route storage structure
struct HttpRoute {
    HttpMethod method;
    std::string pattern;  // e.g., "/api/users/:id"
    int handlerId;        // ID to look up the handler RuntimeValue
    bool isMiddleware = false;
};

// Router storage structure
struct RouterData {
    int id;
    std::string prefix;
    std::vector<HttpRoute> routes;
    std::vector<int> middlewareIds;  // Handler IDs for middleware
};

// Server storage structure
struct ServerData {
    int id;
    int routerId;
    int port = 0;
    int serverSocket = -1;
    std::atomic<bool> running{false};
    std::thread serverThread;
};

// Handler callback storage
using HandlerCallback = std::function<void(HttpRequestInternal&, HttpResponseInternal&)>;

// HTTP Server Manager - Singleton to manage all HTTP resources
class HttpServerManager {
public:
    static HttpServerManager& getInstance() {
        static HttpServerManager instance;
        return instance;
    }

    // Router management
    int createRouter();
    RouterData* getRouter(int routerId);
    void addRoute(int routerId, HttpMethod method, const std::string& pattern, int handlerId);
    void addMiddleware(int routerId, int handlerId);
    int createRouterGroup(int routerId, const std::string& prefix);

    // Server management
    int createServer(int routerId);
    ServerData* getServer(int serverId);
    bool startServer(int serverId, int port, Interpreter* interpreter);
    void stopServer(int serverId);

    // Handler storage
    int storeHandler(const RuntimeValue& handler);
    RuntimeValue* getHandler(int handlerId);

    // Get all routes from all routers for matching
    std::vector<std::pair<int, HttpRoute*>> getAllRoutes();

    // HTTP parsing utilities
    static HttpRequestInternal parseRequest(const std::string& rawRequest);
    static bool matchRoute(const std::string& pattern, const std::string& path,
                          std::unordered_map<std::string, std::string>& params);

private:
    HttpServerManager() = default;
    ~HttpServerManager();

    HttpServerManager(const HttpServerManager&) = delete;
    HttpServerManager& operator=(const HttpServerManager&) = delete;

    void handleClient(int clientSocket, ServerData* server, Interpreter* interpreter);
    void runServerLoop(ServerData* server, Interpreter* interpreter);

    std::mutex mutex_;
    int nextRouterId_ = 1;
    int nextServerId_ = 1;
    int nextHandlerId_ = 1;

    std::unordered_map<int, std::unique_ptr<RouterData>> routers_;
    std::unordered_map<int, std::unique_ptr<ServerData>> servers_;
    std::unordered_map<int, std::unique_ptr<RuntimeValue>> handlers_;
};

} // namespace stratos
