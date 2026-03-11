#include "stratos/gui/Store.h"

namespace stratos {
namespace gui {

// ============================================================================
// Store
// ============================================================================

Store::Store() {}
Store::~Store() {}

std::any Store::get(const std::string& key) const {
    auto it = data_.find(key);
    if (it != data_.end()) {
        return it->second;
    }
    return std::any{}; // empty
}

void Store::set(const std::string& key, std::any value) {
    data_[key] = value;

    // Notify subscribers for this key
    for (auto& sub : subscriptions_) {
        if (sub.key == key && sub.callback) {
            sub.callback(value);
        }
    }
}

int Store::subscribe(const std::string& key, std::function<void(std::any)> callback) {
    int id = nextSubId_++;
    subscriptions_.push_back({id, key, std::move(callback)});
    return id;
}

bool Store::unsubscribe(int subscriptionId) {
    for (auto it = subscriptions_.begin(); it != subscriptions_.end(); ++it) {
        if (it->id == subscriptionId) {
            subscriptions_.erase(it);
            return true;
        }
    }
    return false;
}

// ============================================================================
// StoreRegistry (singleton)
// ============================================================================

StoreRegistry& StoreRegistry::instance() {
    static StoreRegistry inst;
    return inst;
}

int StoreRegistry::createStore() {
    int id = nextId_++;
    stores_[id] = std::make_unique<Store>();
    return id;
}

Store* StoreRegistry::getStore(int id) {
    auto it = stores_.find(id);
    return (it != stores_.end()) ? it->second.get() : nullptr;
}

void StoreRegistry::clear() {
    stores_.clear();
    nextId_ = 0;
}

// ============================================================================
// ContextRegistry (singleton)
// ============================================================================

ContextRegistry& ContextRegistry::instance() {
    static ContextRegistry inst;
    return inst;
}

int ContextRegistry::createContext(const std::string& name, std::any defaultValue) {
    int id = nextId_++;
    contexts_[id] = {name, std::move(defaultValue)};
    return id;
}

bool ContextRegistry::provide(int contextId, std::any value) {
    auto it = contexts_.find(contextId);
    if (it == contexts_.end()) return false;
    it->second.value = std::move(value);
    return true;
}

std::any ContextRegistry::consume(int contextId) {
    auto it = contexts_.find(contextId);
    if (it != contexts_.end()) {
        return it->second.value;
    }
    return std::any{};
}

void ContextRegistry::clear() {
    contexts_.clear();
    nextId_ = 0;
}

} // namespace gui
} // namespace stratos
