#pragma once

#include <any>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

namespace stratos {
namespace gui {

// Forward declarations
class Signal;
class Effect;
class Computed;

using SignalPtr = std::shared_ptr<Signal>;
using EffectPtr = std::shared_ptr<Effect>;
using ComputedPtr = std::shared_ptr<Computed>;

// ============================================================
// Tracking Context
// ============================================================

/// Global tracking context — tracks which Effect is currently executing
/// so that Signal::get() can auto-register dependencies.
class TrackingContext {
public:
    static TrackingContext& instance() {
        static TrackingContext ctx;
        return ctx;
    }

    /// Set the currently-executing effect (nullptr to clear).
    void setCurrentEffect(Effect* effect) { currentEffect_ = effect; }
    Effect* getCurrentEffect() const { return currentEffect_; }

    /// Temporarily disable tracking (for untrack()).
    void pushUntrack() { untrackDepth_++; }
    void popUntrack() { if (untrackDepth_ > 0) untrackDepth_--; }
    bool isTracking() const { return untrackDepth_ == 0 && currentEffect_ != nullptr; }

    /// Batch mode — defer effect execution until batch completes.
    void beginBatch() { batchDepth_++; }
    void endBatch();
    bool isBatching() const { return batchDepth_ > 0; }
    void scheduleEffect(Effect* effect);

private:
    TrackingContext() = default;
    Effect* currentEffect_ = nullptr;
    int untrackDepth_ = 0;
    int batchDepth_ = 0;
    std::vector<Effect*> pendingEffects_;
};

// ============================================================
// Signal<T> — Reactive primitive
// ============================================================

class Signal {
public:
    explicit Signal(std::any initialValue)
        : value_(std::move(initialValue)) {}

    /// Get value — registers current Effect as subscriber if tracking.
    std::any get();

    /// Get value without tracking (like Solid's peek / untrack for single read).
    std::any peek() const { return value_; }

    /// Set value — notifies all subscribed Effects.
    void set(std::any newValue);

    /// Subscribe an effect to this signal.
    void subscribe(Effect* effect);

    /// Unsubscribe an effect from this signal.
    void unsubscribe(Effect* effect);

private:
    std::any value_;
    std::unordered_set<Effect*> subscribers_;

    void notifySubscribers();
};

// ============================================================
// Effect — Auto-tracking reactive side-effect
// ============================================================

class Effect {
public:
    using Callback = std::function<void()>;
    using CleanupFn = std::function<void()>;

    explicit Effect(Callback callback)
        : callback_(std::move(callback)) {}

    ~Effect() { dispose(); }

    /// Run the effect — clears old subscriptions, executes callback,
    /// auto-subscribes to any signals read during execution.
    void run();

    /// Stop the effect permanently.
    void dispose();

    /// Register a cleanup function that runs before each re-execution.
    void addCleanup(CleanupFn fn) { cleanups_.push_back(std::move(fn)); }

    bool isDisposed() const { return disposed_; }

    /// Called by Signal when a dependency changes.
    void notify();

    /// Remove a signal from our tracked dependencies (called by Signal destructor).
    void removeSource(Signal* signal);

private:
    Callback callback_;
    std::vector<CleanupFn> cleanups_;
    std::unordered_set<Signal*> sources_;   // Signals this effect depends on
    bool disposed_ = false;
    bool running_ = false;  // Prevent infinite re-entry

    void runCleanups();
    void clearSources();
};

// ============================================================
// Computed — Memoized derived value
// ============================================================

class Computed {
public:
    using ComputeFn = std::function<std::any()>;

    explicit Computed(ComputeFn fn);
    ~Computed();

    /// Get the cached value. Recomputes if dirty.
    std::any get();

private:
    ComputeFn computeFn_;
    std::any cachedValue_;
    bool dirty_ = true;
    std::unique_ptr<Effect> innerEffect_;
};

// ============================================================
// SignalRegistry — Manages all signals/effects for ID-based access
// ============================================================

class SignalRegistry {
public:
    static SignalRegistry& instance() {
        static SignalRegistry reg;
        return reg;
    }

    // Signal management
    int createSignal(std::any initial);
    std::any getSignal(int id);
    void setSignal(int id, std::any value);
    std::any peekSignal(int id);
    Signal* getSignalPtr(int id);

    // Effect management
    int createEffect(std::function<void()> callback);
    void disposeEffect(int id);

    // Computed management
    int createComputed(std::function<std::any()> fn);
    std::any getComputed(int id);

    // Cleanup
    void clear();

private:
    SignalRegistry() = default;

    int nextSignalId_ = 1;
    int nextEffectId_ = 1;
    int nextComputedId_ = 1;

    std::unordered_map<int, SignalPtr> signals_;
    std::unordered_map<int, EffectPtr> effects_;
    std::unordered_map<int, ComputedPtr> computeds_;
};

} // namespace gui
} // namespace stratos
