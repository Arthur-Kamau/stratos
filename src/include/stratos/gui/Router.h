#pragma once

#include "stratos/gui/Widget.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <regex>
#include <any>

namespace stratos {
namespace gui {

// Route parameter: extracted from path patterns like /user/:id
struct RouteParams {
    std::unordered_map<std::string, std::string> params;
    std::unordered_map<std::string, std::string> query;
};

// A single route entry
struct RouteEntry {
    std::string pattern;                          // e.g., "/user/:id"
    std::vector<std::string> paramNames;          // e.g., ["id"]
    std::regex compiledPattern;                   // compiled regex for matching
    std::any builder;                             // closure that builds the widget tree
    std::any guard;                               // optional: beforeEnter(from, to) -> bool
    bool isWildcard = false;
};

// Router: manages client-side navigation with route matching, history, and guards
class Router {
public:
    Router();
    ~Router();

    // Route registration
    void addRoute(const std::string& pattern, std::any builder);
    void addGuard(const std::string& pattern, std::any guardFn);
    void setNotFound(std::any builder);
    void setRedirect(const std::string& from, const std::string& to);

    // Navigation
    bool navigate(const std::string& path);
    bool back();
    bool forward();
    bool replace(const std::string& path);

    // Route info
    std::string getParam(const std::string& name) const;
    std::string getQuery(const std::string& name) const;
    std::string currentPath() const;

    // Widget access — returns the current route's widget ID
    int getCurrentWidgetId() const { return currentWidgetId_; }
    void setCurrentWidgetId(int id) { currentWidgetId_ = id; }

    // Get the builder for the current matched route
    std::any& getCurrentBuilder();
    std::any& getNotFoundBuilder();
    bool hasNotFoundBuilder() const { return notFoundBuilder_.has_value(); }

    // Route matching (public for native function access)
    bool matchRoute(const std::string& path, RouteEntry& matched, RouteParams& params);

private:
    std::vector<RouteEntry> routes_;
    std::unordered_map<std::string, std::string> redirects_;
    std::any notFoundBuilder_;

    // Navigation state
    std::string currentPath_;
    RouteParams currentParams_;
    int currentWidgetId_ = -1;

    // History
    std::vector<std::string> history_;
    int historyIndex_ = -1;

    // Compile a route pattern into a regex
    RouteEntry compilePattern(const std::string& pattern);

    // Parse query string from path
    static void parseQueryString(const std::string& queryStr,
                                  std::unordered_map<std::string, std::string>& query);
    static std::string extractPath(const std::string& fullPath);
    static std::string extractQuery(const std::string& fullPath);
};

// Global router registry (like signal/widget registries)
class RouterRegistry {
public:
    static RouterRegistry& instance();

    int createRouter();
    Router* getRouter(int id);
    void clear();

private:
    RouterRegistry() = default;
    std::unordered_map<int, std::unique_ptr<Router>> routers_;
    int nextId_ = 0;
};

} // namespace gui
} // namespace stratos
