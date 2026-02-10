#include "stratos/NetworkMonitor.h"
#include "stratos/DevToolsServer.h"
#include <sstream>

namespace stratos {

NetworkMonitor& NetworkMonitor::instance() {
    static NetworkMonitor inst;
    return inst;
}

NetworkMonitor::NetworkMonitor() : enabled_(false), nextId_(1) {}

void NetworkMonitor::enable() { enabled_ = true; }
void NetworkMonitor::disable() { enabled_ = false; }
bool NetworkMonitor::isEnabled() const { return enabled_; }

int NetworkMonitor::recordRequestStart(const std::string& url, const std::string& method,
                                       const std::unordered_map<std::string, std::string>& headers,
                                       const std::string& body) {
    if (!enabled_) return -1;

    std::lock_guard<std::mutex> lock(mutex_);
    NetworkRequest req;
    req.id = nextId_++;
    req.url = url;
    req.method = method;
    req.requestHeaders = headers;
    req.requestBody = body;
    req.startTimeMs = std::chrono::duration<double, std::milli>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    req.completed = false;

    requests_.push_back(req);

    // Keep bounded
    if (requests_.size() > maxRequests_) {
        requests_.erase(requests_.begin(), requests_.begin() + 100);
    }

    // Broadcast event
    if (devtools_) {
        std::ostringstream oss;
        oss << "{\"requestId\":" << req.id;
        oss << ",\"url\":\"" << url << "\"";
        oss << ",\"method\":\"" << method << "\"";
        oss << ",\"startTime\":" << req.startTimeMs << "}";
        devtools_->broadcastEvent("Network.requestStarted", oss.str());
    }

    return req.id;
}

void NetworkMonitor::recordRequestEnd(int requestId, int status, const std::string& statusText,
                                      const std::unordered_map<std::string, std::string>& responseHeaders,
                                      const std::string& responseBody, size_t responseSize) {
    if (!enabled_) return;

    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& req : requests_) {
        if (req.id == requestId) {
            req.responseStatus = status;
            req.responseStatusText = statusText;
            req.responseHeaders = responseHeaders;
            req.responseBody = responseBody;
            req.responseSize = responseSize;
            req.completed = true;

            double endTime = std::chrono::duration<double, std::milli>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            req.durationMs = endTime - req.startTimeMs;

            // Try to get content type from response headers
            for (const auto& [k, v] : responseHeaders) {
                std::string lower = k;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                if (lower == "content-type") {
                    req.contentType = v;
                    break;
                }
            }

            // Broadcast event
            if (devtools_) {
                std::ostringstream oss;
                oss << "{\"requestId\":" << req.id;
                oss << ",\"status\":" << status;
                oss << ",\"size\":" << responseSize;
                oss << ",\"duration\":" << req.durationMs << "}";
                devtools_->broadcastEvent("Network.requestCompleted", oss.str());
            }
            break;
        }
    }
}

std::vector<NetworkRequest> NetworkMonitor::getRequests() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return requests_;
}

std::optional<NetworkRequest> NetworkMonitor::getRequestDetail(int requestId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& req : requests_) {
        if (req.id == requestId) return req;
    }
    return std::nullopt;
}

void NetworkMonitor::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    requests_.clear();
}

void NetworkMonitor::setDevToolsServer(DevToolsServer* server) {
    devtools_ = server;
}

} // namespace stratos
