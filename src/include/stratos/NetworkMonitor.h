#ifndef STRATOS_NETWORK_MONITOR_H
#define STRATOS_NETWORK_MONITOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <optional>
#include <atomic>

namespace stratos {

class DevToolsServer;

// Recorded network request with response info
struct NetworkRequest {
    int id;
    std::string url;
    std::string method; // GET, POST, PUT, DELETE
    std::unordered_map<std::string, std::string> requestHeaders;
    std::string requestBody;

    int responseStatus = 0;
    std::string responseStatusText;
    std::unordered_map<std::string, std::string> responseHeaders;
    std::string responseBody;
    size_t responseSize = 0;
    std::string contentType;

    double startTimeMs = 0;
    double durationMs = 0;
    bool completed = false;
};

// Network Monitor - intercepts and records HTTP requests
class NetworkMonitor {
public:
    static NetworkMonitor& instance();

    void enable();
    void disable();
    bool isEnabled() const;

    // Record a request start (returns request ID)
    int recordRequestStart(const std::string& url, const std::string& method,
                           const std::unordered_map<std::string, std::string>& headers = {},
                           const std::string& body = "");

    // Record a request completion
    void recordRequestEnd(int requestId, int status, const std::string& statusText,
                          const std::unordered_map<std::string, std::string>& responseHeaders,
                          const std::string& responseBody, size_t responseSize);

    // Get all recorded requests
    std::vector<NetworkRequest> getRequests() const;

    // Get detailed info for a single request
    std::optional<NetworkRequest> getRequestDetail(int requestId) const;

    // Clear recorded requests
    void clear();

    // DevTools integration
    void setDevToolsServer(DevToolsServer* server);

private:
    NetworkMonitor();
    ~NetworkMonitor() = default;

    std::atomic<bool> enabled_;
    mutable std::mutex mutex_;
    int nextId_;
    std::vector<NetworkRequest> requests_;
    size_t maxRequests_ = 1000;
    DevToolsServer* devtools_ = nullptr;
};

} // namespace stratos

#endif // STRATOS_NETWORK_MONITOR_H
