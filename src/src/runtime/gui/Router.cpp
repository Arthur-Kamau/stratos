#include "stratos/gui/Router.h"
#include <sstream>
#include <algorithm>

namespace stratos {
namespace gui {

// ============================================================================
// Router
// ============================================================================

Router::Router() {}
Router::~Router() {}

RouteEntry Router::compilePattern(const std::string& pattern) {
    RouteEntry entry;
    entry.pattern = pattern;

    // Check for wildcard
    if (pattern == "*" || pattern.back() == '*') {
        entry.isWildcard = true;
        std::string base = pattern;
        if (base.back() == '*') base.pop_back();
        if (!base.empty() && base.back() == '/') base.pop_back();
        entry.compiledPattern = std::regex("^" + base + "(/.*)?$");
        return entry;
    }

    // Build regex from pattern, extracting parameter names
    std::string regexStr = "^";
    std::string remaining = pattern;
    size_t pos = 0;

    while (pos < remaining.size()) {
        if (remaining[pos] == ':') {
            // Parameter: :name
            size_t end = remaining.find('/', pos + 1);
            if (end == std::string::npos) end = remaining.size();
            std::string paramName = remaining.substr(pos + 1, end - pos - 1);
            entry.paramNames.push_back(paramName);
            regexStr += "([^/]+)";
            pos = end;
        } else {
            // Literal character (escape regex special chars)
            char c = remaining[pos];
            if (c == '.' || c == '+' || c == '?' || c == '(' || c == ')' ||
                c == '[' || c == ']' || c == '{' || c == '}' || c == '\\' ||
                c == '^' || c == '$' || c == '|') {
                regexStr += '\\';
            }
            regexStr += c;
            pos++;
        }
    }

    regexStr += "$";
    entry.compiledPattern = std::regex(regexStr);
    return entry;
}

void Router::addRoute(const std::string& pattern, std::any builder) {
    auto entry = compilePattern(pattern);
    entry.builder = std::move(builder);
    routes_.push_back(std::move(entry));
}

void Router::addGuard(const std::string& pattern, std::any guardFn) {
    for (auto& route : routes_) {
        if (route.pattern == pattern) {
            route.guard = std::move(guardFn);
            return;
        }
    }
}

void Router::setNotFound(std::any builder) {
    notFoundBuilder_ = std::move(builder);
}

void Router::setRedirect(const std::string& from, const std::string& to) {
    redirects_[from] = to;
}

bool Router::matchRoute(const std::string& path, RouteEntry& matched, RouteParams& params) {
    std::string cleanPath = extractPath(path);
    std::string queryStr = extractQuery(path);

    // Parse query parameters
    if (!queryStr.empty()) {
        parseQueryString(queryStr, params.query);
    }

    // Try each route
    for (auto& route : routes_) {
        std::smatch match;
        if (std::regex_match(cleanPath, match, route.compiledPattern)) {
            matched = route; // Copy — builder/guard are std::any (shared)

            // Extract path parameters
            for (size_t i = 0; i < route.paramNames.size() && i + 1 < match.size(); i++) {
                params.params[route.paramNames[i]] = match[i + 1].str();
            }

            return true;
        }
    }

    return false;
}

bool Router::navigate(const std::string& path) {
    // Check for redirects
    std::string targetPath = path;
    auto redirIt = redirects_.find(extractPath(path));
    if (redirIt != redirects_.end()) {
        targetPath = redirIt->second;
    }

    // Match route
    RouteEntry matched;
    RouteParams params;
    if (!matchRoute(targetPath, matched, params)) {
        // No match — use 404 handler if available
        if (notFoundBuilder_.has_value()) {
            currentPath_ = targetPath;
            currentParams_ = {};

            // Add to history
            if (historyIndex_ < (int)history_.size() - 1) {
                history_.erase(history_.begin() + historyIndex_ + 1, history_.end());
            }
            history_.push_back(targetPath);
            historyIndex_ = (int)history_.size() - 1;
            return true;
        }
        return false;
    }

    // Update state
    currentPath_ = targetPath;
    currentParams_ = params;

    // Add to history
    if (historyIndex_ < (int)history_.size() - 1) {
        history_.erase(history_.begin() + historyIndex_ + 1, history_.end());
    }
    history_.push_back(targetPath);
    historyIndex_ = (int)history_.size() - 1;

    return true;
}

bool Router::back() {
    if (historyIndex_ <= 0) return false;
    historyIndex_--;
    std::string path = history_[historyIndex_];

    // Re-match without adding to history
    RouteEntry matched;
    RouteParams params;
    if (matchRoute(path, matched, params)) {
        currentPath_ = path;
        currentParams_ = params;
        return true;
    }
    return false;
}

bool Router::forward() {
    if (historyIndex_ >= (int)history_.size() - 1) return false;
    historyIndex_++;
    std::string path = history_[historyIndex_];

    RouteEntry matched;
    RouteParams params;
    if (matchRoute(path, matched, params)) {
        currentPath_ = path;
        currentParams_ = params;
        return true;
    }
    return false;
}

bool Router::replace(const std::string& path) {
    RouteEntry matched;
    RouteParams params;
    if (!matchRoute(path, matched, params)) return false;

    currentPath_ = path;
    currentParams_ = params;

    // Replace current history entry
    if (!history_.empty() && historyIndex_ >= 0) {
        history_[historyIndex_] = path;
    }
    return true;
}

std::string Router::getParam(const std::string& name) const {
    auto it = currentParams_.params.find(name);
    return (it != currentParams_.params.end()) ? it->second : "";
}

std::string Router::getQuery(const std::string& name) const {
    auto it = currentParams_.query.find(name);
    return (it != currentParams_.query.end()) ? it->second : "";
}

std::string Router::currentPath() const {
    return currentPath_;
}

std::any& Router::getCurrentBuilder() {
    // Find the matching route's builder
    RouteEntry matched;
    RouteParams params;
    if (matchRoute(currentPath_, matched, params)) {
        // Find the actual route entry (matchRoute copies)
        for (auto& route : routes_) {
            if (route.pattern == matched.pattern) {
                return route.builder;
            }
        }
    }
    static std::any empty;
    return empty;
}

std::any& Router::getNotFoundBuilder() {
    return notFoundBuilder_;
}

// Static helpers

void Router::parseQueryString(const std::string& queryStr,
                               std::unordered_map<std::string, std::string>& query) {
    std::istringstream stream(queryStr);
    std::string pair;
    while (std::getline(stream, pair, '&')) {
        auto eqPos = pair.find('=');
        if (eqPos != std::string::npos) {
            query[pair.substr(0, eqPos)] = pair.substr(eqPos + 1);
        } else {
            query[pair] = "";
        }
    }
}

std::string Router::extractPath(const std::string& fullPath) {
    auto qPos = fullPath.find('?');
    return (qPos != std::string::npos) ? fullPath.substr(0, qPos) : fullPath;
}

std::string Router::extractQuery(const std::string& fullPath) {
    auto qPos = fullPath.find('?');
    return (qPos != std::string::npos) ? fullPath.substr(qPos + 1) : "";
}

// ============================================================================
// RouterRegistry (singleton)
// ============================================================================

RouterRegistry& RouterRegistry::instance() {
    static RouterRegistry inst;
    return inst;
}

int RouterRegistry::createRouter() {
    int id = nextId_++;
    routers_[id] = std::make_unique<Router>();
    return id;
}

Router* RouterRegistry::getRouter(int id) {
    auto it = routers_.find(id);
    return (it != routers_.end()) ? it->second.get() : nullptr;
}

void RouterRegistry::clear() {
    routers_.clear();
    nextId_ = 0;
}

} // namespace gui
} // namespace stratos
