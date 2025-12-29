#ifndef STRATOS_DEVTOOLS_SERVER_H
#define STRATOS_DEVTOOLS_SERVER_H

#include "stratos/Logger.h"
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace stratos {

// Forward declarations
class DevToolsServer;

// JSON-RPC message types
struct JsonRpcRequest {
    std::string jsonrpc = "2.0";
    int id;
    std::string method;
    std::string params; // JSON string
};

struct JsonRpcResponse {
    std::string jsonrpc = "2.0";
    int id;
    std::string result; // JSON string
    std::string error;  // JSON string (if error)
};

struct JsonRpcEvent {
    std::string jsonrpc = "2.0";
    std::string method;
    std::string params; // JSON string
};

// DevTools sink that sends logs to DevTools clients
class DevToolsSink : public LogSink {
public:
    DevToolsSink(DevToolsServer* server);
    void write(const LogEntry& entry) override;

private:
    DevToolsServer* server_;
    std::string entryToJson(const LogEntry& entry);
};

// Simple DevTools server using HTTP polling (upgradeable to WebSocket)
class DevToolsServer {
public:
    DevToolsServer(int port = 9222);
    ~DevToolsServer();

    // Server control
    void start();
    void stop();
    bool isRunning() const;

    // Event broadcasting
    void broadcastEvent(const std::string& method, const std::string& params);

    // Client management
    void addClient(int clientId);
    void removeClient(int clientId);
    std::vector<int> getClients() const;

    // Message handling
    using RequestHandler = std::function<std::string(const std::string& params)>;
    void registerHandler(const std::string& method, RequestHandler handler);

private:
    void serverLoop();
    void handleClient(int clientSocket);
    std::string handleRequest(const std::string& request);
    std::string parseHttpRequest(const std::string& data);
    std::string createHttpResponse(const std::string& content, const std::string& contentType = "application/json");

    int port_;
    int serverSocket_;
    std::atomic<bool> running_;
    std::thread serverThread_;

    mutable std::mutex clientsMutex_;
    std::vector<int> clients_;

    mutable std::mutex eventsMutex_;
    std::vector<JsonRpcEvent> pendingEvents_;

    std::unordered_map<std::string, RequestHandler> handlers_;
    mutable std::mutex handlersMutex_;

    void initializeHandlers();
};

} // namespace stratos

#endif // STRATOS_DEVTOOLS_SERVER_H
