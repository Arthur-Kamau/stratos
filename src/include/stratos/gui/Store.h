#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <any>

namespace stratos {
namespace gui {

// Store: reactive key-value container with per-key subscriptions
class Store {
public:
    Store();
    ~Store();

    // Read a field
    std::any get(const std::string& key) const;

    // Write a field, notify subscribers of that key
    void set(const std::string& key, std::any value);

    // Subscribe to changes on a specific key; returns subscription ID
    int subscribe(const std::string& key, std::function<void(std::any)> callback);

    // Remove a subscription by ID
    bool unsubscribe(int subscriptionId);

private:
    struct Subscription {
        int id;
        std::string key;
        std::function<void(std::any)> callback;
    };

    std::unordered_map<std::string, std::any> data_;
    std::vector<Subscription> subscriptions_;
    int nextSubId_ = 0;
};

// Context: named value for dependency injection
struct ContextEntry {
    std::string name;
    std::any value;
};

// StoreRegistry singleton
class StoreRegistry {
public:
    static StoreRegistry& instance();

    int createStore();
    Store* getStore(int id);
    void clear();

private:
    StoreRegistry() = default;
    std::unordered_map<int, std::unique_ptr<Store>> stores_;
    int nextId_ = 0;
};

// ContextRegistry singleton
class ContextRegistry {
public:
    static ContextRegistry& instance();

    int createContext(const std::string& name, std::any defaultValue);
    bool provide(int contextId, std::any value);
    std::any consume(int contextId);
    void clear();

private:
    ContextRegistry() = default;
    std::unordered_map<int, ContextEntry> contexts_;
    int nextId_ = 0;
};

} // namespace gui
} // namespace stratos
