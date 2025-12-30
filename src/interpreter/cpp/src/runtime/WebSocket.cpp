#include "stratos/WebSocket.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <iomanip>

// Platform-specific includes
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define close closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/select.h>
#endif

// Simple SHA-1 implementation for WebSocket handshake
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

namespace stratos {

WebSocketManager& WebSocketManager::instance() {
    static WebSocketManager instance;
    return instance;
}

WebSocketManager::~WebSocketManager() {
    for (auto& pair : connections) {
        if (pair.second->connected) {
            close(pair.first);
        }
    }
}

bool WebSocketManager::parseUrl(const std::string& url, std::string& host, int& port, std::string& path) {
    // Parse WebSocket URL: ws://host:port/path or wss://host:port/path
    std::string protocol;
    size_t protocolEnd = url.find("://");
    if (protocolEnd == std::string::npos) {
        return false;
    }

    protocol = url.substr(0, protocolEnd);
    if (protocol != "ws" && protocol != "wss") {
        return false;
    }

    // Default ports
    port = (protocol == "wss") ? 443 : 80;

    size_t hostStart = protocolEnd + 3;
    size_t pathStart = url.find('/', hostStart);

    std::string hostPort;
    if (pathStart == std::string::npos) {
        hostPort = url.substr(hostStart);
        path = "/";
    } else {
        hostPort = url.substr(hostStart, pathStart - hostStart);
        path = url.substr(pathStart);
    }

    // Check for port in host
    size_t portPos = hostPort.find(':');
    if (portPos != std::string::npos) {
        host = hostPort.substr(0, portPos);
        port = std::stoi(hostPort.substr(portPos + 1));
    } else {
        host = hostPort;
    }

    return true;
}

std::vector<uint8_t> WebSocketManager::sha1(const std::string& input) {
    std::vector<uint8_t> hash(SHA_DIGEST_LENGTH);
    SHA1(reinterpret_cast<const uint8_t*>(input.c_str()), input.length(), hash.data());
    return hash;
}

std::string WebSocketManager::base64Encode(const std::vector<uint8_t>& input) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.data(), input.size());
    BIO_flush(bio);

    BUF_MEM* bufferPtr;
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return result;
}

bool WebSocketManager::performHandshake(int socket, const std::string& host, const std::string& path) {
    // Generate random WebSocket key
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::vector<uint8_t> keyBytes(16);
    for (int i = 0; i < 16; i++) {
        keyBytes[i] = dis(gen);
    }
    std::string key = base64Encode(keyBytes);

    // Build handshake request
    std::ostringstream request;
    request << "GET " << path << " HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "Upgrade: websocket\r\n";
    request << "Connection: Upgrade\r\n";
    request << "Sec-WebSocket-Key: " << key << "\r\n";
    request << "Sec-WebSocket-Version: 13\r\n";
    request << "\r\n";

    std::string requestStr = request.str();
    if (::send(socket, requestStr.c_str(), requestStr.length(), 0) < 0) {
        return false;
    }

    // Read response
    char buffer[4096];
    int bytesRead = recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        return false;
    }
    buffer[bytesRead] = '\0';

    std::string response(buffer);

    // Check for 101 Switching Protocols
    if (response.find("101") == std::string::npos) {
        return false;
    }

    // Verify Sec-WebSocket-Accept header
    std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string acceptKey = key + magic;
    std::vector<uint8_t> acceptHash = sha1(acceptKey);
    std::string expectedAccept = base64Encode(acceptHash);

    if (response.find("Sec-WebSocket-Accept: " + expectedAccept) == std::string::npos) {
        std::cerr << "WebSocket handshake failed: Invalid Sec-WebSocket-Accept" << std::endl;
        return false;
    }

    return true;
}

int WebSocketManager::connect(const std::string& url) {
    std::string host;
    int port;
    std::string path;

    if (!parseUrl(url, host, port, path)) {
        throw std::runtime_error("Invalid WebSocket URL: " + url);
    }

    // Create socket
    int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // Disable Nagle's algorithm for low-latency messaging
    int flag = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

    // Resolve host
    struct hostent* server = gethostbyname(host.c_str());
    if (server == nullptr) {
        ::close(sock);
        throw std::runtime_error("Failed to resolve host: " + host);
    }

    // Connect
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    std::memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (::connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        ::close(sock);
        throw std::runtime_error("Failed to connect to " + host + ":" + std::to_string(port));
    }

    // Perform WebSocket handshake
    if (!performHandshake(sock, host, path)) {
        ::close(sock);
        throw std::runtime_error("WebSocket handshake failed");
    }

    // Store connection
    auto conn = std::make_shared<WebSocketConnection>();
    conn->socket = sock;
    conn->host = host;
    conn->port = port;
    conn->path = path;
    conn->connected = true;

    int id = nextConnectionId++;
    connections[id] = conn;

    return id;
}

void WebSocketManager::sendFrame(int socket, const std::string& payload, uint8_t opcode, bool maskPayload) {
    std::vector<uint8_t> frame;

    // FIN bit set, opcode
    frame.push_back(0x80 | opcode);

    // Payload length and mask bit
    size_t payloadLen = payload.length();
    uint8_t maskBit = maskPayload ? 0x80 : 0x00;

    if (payloadLen < 126) {
        frame.push_back(maskBit | payloadLen);
    } else if (payloadLen <= 65535) {
        frame.push_back(maskBit | 126);
        frame.push_back((payloadLen >> 8) & 0xFF);
        frame.push_back(payloadLen & 0xFF);
    } else {
        frame.push_back(maskBit | 127);
        for (int i = 7; i >= 0; i--) {
            frame.push_back((payloadLen >> (i * 8)) & 0xFF);
        }
    }

    // Add masking key if masking is enabled
    uint8_t maskingKey[4] = {0};
    if (maskPayload) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        for (int i = 0; i < 4; i++) {
            maskingKey[i] = dis(gen);
            frame.push_back(maskingKey[i]);
        }
    }

    // Add payload (masked or unmasked)
    for (size_t i = 0; i < payloadLen; i++) {
        if (maskPayload) {
            frame.push_back(payload[i] ^ maskingKey[i % 4]);
        } else {
            frame.push_back(payload[i]);
        }
    }

    // Send frame
    ::send(socket, reinterpret_cast<const char*>(frame.data()), frame.size(), 0);
}

std::string WebSocketManager::receiveFrame(int socket, bool expectMasked) {
    // Read frame header (at least 2 bytes)
    uint8_t header[2];
    int bytesRead = recv(socket, reinterpret_cast<char*>(header), 2, 0);
    if (bytesRead < 2) {
        throw std::runtime_error("Failed to read WebSocket frame header");
    }

    bool fin = (header[0] & 0x80) != 0;
    uint8_t opcode = header[0] & 0x0F;
    bool masked = (header[1] & 0x80) != 0;
    uint64_t payloadLen = header[1] & 0x7F;

    // Read extended payload length if needed
    if (payloadLen == 126) {
        uint8_t extLen[2];
        recv(socket, reinterpret_cast<char*>(extLen), 2, 0);
        payloadLen = (extLen[0] << 8) | extLen[1];
    } else if (payloadLen == 127) {
        uint8_t extLen[8];
        recv(socket, reinterpret_cast<char*>(extLen), 8, 0);
        payloadLen = 0;
        for (int i = 0; i < 8; i++) {
            payloadLen = (payloadLen << 8) | extLen[i];
        }
    }

    // Read masking key if present (server shouldn't mask, but handle it)
    uint8_t maskingKey[4] = {0};
    if (masked) {
        recv(socket, reinterpret_cast<char*>(maskingKey), 4, 0);
    }

    // Read payload
    std::vector<uint8_t> payload(payloadLen);
    size_t totalRead = 0;
    while (totalRead < payloadLen) {
        int n = recv(socket, reinterpret_cast<char*>(payload.data() + totalRead),
                     payloadLen - totalRead, 0);
        if (n <= 0) {
            throw std::runtime_error("Connection closed while reading payload");
        }
        totalRead += n;
    }

    // Unmask if needed
    if (masked) {
        for (size_t i = 0; i < payloadLen; i++) {
            payload[i] ^= maskingKey[i % 4];
        }
    }

    // Handle different opcodes
    if (opcode == 0x1) {  // Text frame
        return std::string(payload.begin(), payload.end());
    } else if (opcode == 0x8) {  // Close frame
        throw std::runtime_error("WebSocket connection closed by server");
    } else if (opcode == 0x9) {  // Ping frame
        sendFrame(socket, std::string(payload.begin(), payload.end()), 0xA);  // Send pong
        return receiveFrame(socket);  // Read next frame
    }

    return "";
}

void WebSocketManager::send(int wsId, const std::string& message) {
    auto it = connections.find(wsId);
    if (it == connections.end() || !it->second->connected) {
        throw std::runtime_error("Invalid WebSocket connection ID: " + std::to_string(wsId));
    }

    sendFrame(it->second->socket, message, 0x1);  // Text frame
}

std::string WebSocketManager::receive(int wsId, int timeoutMs) {
    auto it = connections.find(wsId);
    if (it == connections.end() || !it->second->connected) {
        throw std::runtime_error("Invalid WebSocket connection ID: " + std::to_string(wsId));
    }

    // Set socket timeout
    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;
    setsockopt(it->second->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    try {
        return receiveFrame(it->second->socket);
    } catch (const std::exception& e) {
        // Timeout or error
        return "";
    }
}

void WebSocketManager::close(int wsId) {
    auto it = connections.find(wsId);
    if (it == connections.end()) {
        return;
    }

    if (it->second->connected) {
        // Send close frame
        sendFrame(it->second->socket, "", 0x8);
        ::close(it->second->socket);
        it->second->connected = false;
    }

    connections.erase(it);
}

bool WebSocketManager::isConnected(int wsId) {
    auto it = connections.find(wsId);
    if (it == connections.end()) {
        return false;
    }
    return it->second->connected;
}

// ============================================================================
// SERVER IMPLEMENTATION
// ============================================================================

std::string WebSocketManager::extractWebSocketKey(const std::string& request) {
    // Find Sec-WebSocket-Key header
    std::string keyHeader = "Sec-WebSocket-Key: ";
    size_t keyPos = request.find(keyHeader);
    if (keyPos == std::string::npos) {
        return "";
    }

    size_t keyStart = keyPos + keyHeader.length();
    size_t keyEnd = request.find("\r\n", keyStart);
    if (keyEnd == std::string::npos) {
        return "";
    }

    return request.substr(keyStart, keyEnd - keyStart);
}

bool WebSocketManager::performServerHandshake(int clientSocket) {
    // Read HTTP upgrade request
    char buffer[4096];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        return false;
    }
    buffer[bytesRead] = '\0';

    std::string request(buffer);

    // Check for HTTP upgrade request
    if (request.find("GET ") != 0 || request.find("Upgrade: websocket") == std::string::npos) {
        return false;
    }

    // Extract Sec-WebSocket-Key
    std::string key = extractWebSocketKey(request);
    if (key.empty()) {
        return false;
    }

    // Calculate Sec-WebSocket-Accept
    std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string acceptKey = key + magic;
    std::vector<uint8_t> acceptHash = sha1(acceptKey);
    std::string acceptValue = base64Encode(acceptHash);

    // Build response
    std::ostringstream response;
    response << "HTTP/1.1 101 Switching Protocols\r\n";
    response << "Upgrade: websocket\r\n";
    response << "Connection: Upgrade\r\n";
    response << "Sec-WebSocket-Accept: " << acceptValue << "\r\n";
    response << "\r\n";

    std::string responseStr = response.str();
    int sent = ::send(clientSocket, responseStr.c_str(), responseStr.length(), 0);

    return sent > 0;
}

int WebSocketManager::createServer(int port) {
    // Create server socket
    int serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error("Failed to create server socket");
    }

    // Set socket options to reuse address
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind to port
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (::bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        ::close(serverSocket);
        throw std::runtime_error("Failed to bind to port " + std::to_string(port));
    }

    // Listen for connections
    if (::listen(serverSocket, 5) < 0) {
        ::close(serverSocket);
        throw std::runtime_error("Failed to listen on port " + std::to_string(port));
    }

    // Store server socket
    auto conn = std::make_shared<WebSocketConnection>();
    conn->socket = serverSocket;
    conn->port = port;
    conn->connected = true;
    conn->isServerSocket = true;

    int id = nextConnectionId++;
    connections[id] = conn;

    return id;
}

int WebSocketManager::acceptClient(int serverId, int timeoutMs) {
    auto it = connections.find(serverId);
    if (it == connections.end() || !it->second->isServerSocket) {
        throw std::runtime_error("Invalid server ID: " + std::to_string(serverId));
    }

    int serverSocket = it->second->socket;

    // Set socket to non-blocking for timeout
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(serverSocket, &readfds);

    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    int result = select(serverSocket + 1, &readfds, NULL, NULL, &tv);

    if (result <= 0) {
        return -1;  // Timeout or error
    }

    // Accept client connection
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = ::accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

    if (clientSocket < 0) {
        return -1;
    }

    // Disable Nagle's algorithm for low-latency messaging
    int flag = 1;
    setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

    // Perform WebSocket handshake
    if (!performServerHandshake(clientSocket)) {
        ::close(clientSocket);
        return -1;
    }

    // Store client connection
    auto client = std::make_shared<WebSocketClient>();
    client->socket = clientSocket;
    client->address = inet_ntoa(clientAddr.sin_addr);
    client->connected = true;

    int clientId = nextClientId++;
    clients[clientId] = client;

    return clientId;
}

void WebSocketManager::sendToClient(int clientId, const std::string& message) {
    auto it = clients.find(clientId);
    if (it == clients.end() || !it->second->connected) {
        throw std::runtime_error("Invalid client ID: " + std::to_string(clientId));
    }

    // Server sends unmasked frames
    sendFrame(it->second->socket, message, 0x1, false);
}

std::string WebSocketManager::receiveFromClient(int clientId, int timeoutMs) {
    auto it = clients.find(clientId);
    if (it == clients.end() || !it->second->connected) {
        throw std::runtime_error("Invalid client ID: " + std::to_string(clientId));
    }

    // Set socket timeout
    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;
    setsockopt(it->second->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    try {
        // Client sends masked frames to server
        return receiveFrame(it->second->socket, true);
    } catch (const std::exception& e) {
        return "";
    }
}

void WebSocketManager::closeClient(int clientId) {
    auto it = clients.find(clientId);
    if (it == clients.end()) {
        return;
    }

    if (it->second->connected) {
        // Send close frame (unmasked)
        sendFrame(it->second->socket, "", 0x8, false);
        ::close(it->second->socket);
        it->second->connected = false;
    }

    clients.erase(it);
}

void WebSocketManager::closeServer(int serverId) {
    auto it = connections.find(serverId);
    if (it == connections.end() || !it->second->isServerSocket) {
        return;
    }

    if (it->second->connected) {
        ::close(it->second->socket);
        it->second->connected = false;
    }

    connections.erase(it);
}

bool WebSocketManager::isClientConnected(int clientId) {
    auto it = clients.find(clientId);
    if (it == clients.end()) {
        return false;
    }
    return it->second->connected;
}

} // namespace stratos
