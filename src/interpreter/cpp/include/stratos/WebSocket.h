#ifndef STRATOS_WEBSOCKET_H
#define STRATOS_WEBSOCKET_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <cstdint>

namespace stratos {

struct WebSocketConnection {
    int socket;
    std::string host;
    int port;
    std::string path;
    bool connected;
    bool isServerSocket;  // True if this is a server listening socket

    WebSocketConnection() : socket(-1), port(0), connected(false), isServerSocket(false) {}
};

struct WebSocketClient {
    int socket;
    std::string address;
    bool connected;

    WebSocketClient() : socket(-1), connected(false) {}
};

class WebSocketManager {
public:
    static WebSocketManager& instance();

    // CLIENT FUNCTIONS
    // Connect to WebSocket server
    int connect(const std::string& url);

    // Send text message
    void send(int wsId, const std::string& message);

    // Receive message (blocking with timeout)
    std::string receive(int wsId, int timeoutMs = 5000);

    // Close connection
    void close(int wsId);

    // Check if connected
    bool isConnected(int wsId);

    // SERVER FUNCTIONS
    // Create WebSocket server listening on port
    int createServer(int port);

    // Accept incoming client connection (non-blocking)
    int acceptClient(int serverId, int timeoutMs = 1000);

    // Send message to specific client
    void sendToClient(int clientId, const std::string& message);

    // Receive message from specific client
    std::string receiveFromClient(int clientId, int timeoutMs = 5000);

    // Close client connection
    void closeClient(int clientId);

    // Close server
    void closeServer(int serverId);

    // Check if client is connected
    bool isClientConnected(int clientId);

private:
    WebSocketManager() = default;
    ~WebSocketManager();

    // Helper methods - Client
    bool parseUrl(const std::string& url, std::string& host, int& port, std::string& path);
    bool performHandshake(int socket, const std::string& host, const std::string& path);
    void sendFrame(int socket, const std::string& payload, uint8_t opcode, bool maskPayload = true);
    std::string receiveFrame(int socket, bool expectMasked = false);
    std::string base64Encode(const std::vector<uint8_t>& input);
    std::vector<uint8_t> sha1(const std::string& input);

    // Helper methods - Server
    bool performServerHandshake(int clientSocket);
    std::string extractWebSocketKey(const std::string& request);

    // Connection storage
    std::unordered_map<int, std::shared_ptr<WebSocketConnection>> connections;  // Client connections & server sockets
    std::unordered_map<int, std::shared_ptr<WebSocketClient>> clients;  // Accepted client connections from server
    int nextConnectionId = 1;
    int nextClientId = 1000;  // Start from 1000 to avoid confusion with connection IDs
};

} // namespace stratos

#endif // STRATOS_WEBSOCKET_H
