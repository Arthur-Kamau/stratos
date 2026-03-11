#include "stratos/gui/Signals.h"
#include <algorithm>

namespace stratos {
namespace gui {

// ============================================================
// TrackingContext
// ============================================================

void TrackingContext::endBatch() {
    if (batchDepth_ > 0) batchDepth_--;
    if (batchDepth_ == 0) {
        // Execute all pending effects
        auto pending = std::move(pendingEffects_);
        pendingEffects_.clear();
        // Deduplicate — an effect may have been scheduled multiple times
        std::unordered_set<Effect*> seen;
        for (auto* effect : pending) {
            if (seen.insert(effect).second && !effect->isDisposed()) {
                effect->run();
            }
        }
    }
}

void TrackingContext::scheduleEffect(Effect* effect) {
    pendingEffects_.push_back(effect);
}

// ============================================================
// Signal
// ============================================================

std::any Signal::get() {
    auto& ctx = TrackingContext::instance();
    if (ctx.isTracking()) {
        subscribe(ctx.getCurrentEffect());
    }
    return value_;
}

void Signal::set(std::any newValue) {
    value_ = std::move(newValue);
    notifySubscribers();
}

void Signal::subscribe(Effect* effect) {
    subscribers_.insert(effect);
}

void Signal::unsubscribe(Effect* effect) {
    subscribers_.erase(effect);
}

void Signal::notifySubscribers() {
    // Copy set — effects may modify subscribers during execution
    auto subs = subscribers_;
    for (auto* effect : subs) {
        if (!effect->isDisposed()) {
            effect->notify();
        }
    }
}

// ============================================================
// Effect
// ============================================================

void Effect::run() {
    if (disposed_ || running_) return;
    running_ = true;

    // Run cleanup from previous execution
    runCleanups();

    // Clear old subscriptions — we'll re-subscribe during execution
    clearSources();

    // Set this as the current tracking effect
    auto& ctx = TrackingContext::instance();
    Effect* prevEffect = ctx.getCurrentEffect();
    ctx.setCurrentEffect(this);

    // Execute the callback — any Signal::get() calls will subscribe us
    if (callback_) {
        callback_();
    }

    // Restore previous tracking context
    ctx.setCurrentEffect(prevEffect);
    running_ = false;
}

void Effect::dispose() {
    if (disposed_) return;
    disposed_ = true;
    runCleanups();
    clearSources();
    callback_ = nullptr;
}

void Effect::notify() {
    if (disposed_) return;
    auto& ctx = TrackingContext::instance();
    if (ctx.isBatching()) {
        ctx.scheduleEffect(this);
    } else {
        run();
    }
}

void Effect::removeSource(Signal* signal) {
    sources_.erase(signal);
}

void Effect::runCleanups() {
    for (auto& cleanup : cleanups_) {
        if (cleanup) cleanup();
    }
    cleanups_.clear();
}

void Effect::clearSources() {
    for (auto* signal : sources_) {
        signal->unsubscribe(this);
    }
    sources_.clear();
}

// ============================================================
// Computed
// ============================================================

Computed::Computed(ComputeFn fn)
    : computeFn_(std::move(fn))
{
    // Create an inner effect that marks us dirty when dependencies change
    innerEffect_ = std::make_unique<Effect>([this]() {
        // Re-compute the value
        if (computeFn_) {
            cachedValue_ = computeFn_();
        }
        dirty_ = false;
    });
    // Run once to compute initial value and set up subscriptions
    innerEffect_->run();
}

Computed::~Computed() {
    if (innerEffect_) {
        innerEffect_->dispose();
    }
}

std::any Computed::get() {
    // If we're being tracked, the inner effect's signals
    // will already trigger recomputation when they change.
    // Just return the cached value.
    if (dirty_ && computeFn_) {
        innerEffect_->run();
    }
    return cachedValue_;
}

// ============================================================
// SignalRegistry
// ============================================================

int SignalRegistry::createSignal(std::any initial) {
    int id = nextSignalId_++;
    signals_[id] = std::make_shared<Signal>(std::move(initial));
    return id;
}

std::any SignalRegistry::getSignal(int id) {
    auto it = signals_.find(id);
    if (it != signals_.end()) {
        return it->second->get();
    }
    return {};
}

void SignalRegistry::setSignal(int id, std::any value) {
    auto it = signals_.find(id);
    if (it != signals_.end()) {
        it->second->set(std::move(value));
    }
}

std::any SignalRegistry::peekSignal(int id) {
    auto it = signals_.find(id);
    if (it != signals_.end()) {
        return it->second->peek();
    }
    return {};
}

Signal* SignalRegistry::getSignalPtr(int id) {
    auto it = signals_.find(id);
    return (it != signals_.end()) ? it->second.get() : nullptr;
}

int SignalRegistry::createEffect(std::function<void()> callback) {
    int id = nextEffectId_++;
    auto effect = std::make_shared<Effect>(std::move(callback));
    effects_[id] = effect;
    // Run the effect immediately to set up subscriptions
    effect->run();
    return id;
}

void SignalRegistry::disposeEffect(int id) {
    auto it = effects_.find(id);
    if (it != effects_.end()) {
        it->second->dispose();
        effects_.erase(it);
    }
}

int SignalRegistry::createComputed(std::function<std::any()> fn) {
    int id = nextComputedId_++;
    computeds_[id] = std::make_shared<Computed>(std::move(fn));
    return id;
}

std::any SignalRegistry::getComputed(int id) {
    auto it = computeds_.find(id);
    if (it != computeds_.end()) {
        return it->second->get();
    }
    return {};
}

void SignalRegistry::clear() {
    // Dispose all effects first
    for (auto& [id, effect] : effects_) {
        effect->dispose();
    }
    effects_.clear();
    computeds_.clear();
    signals_.clear();
    nextSignalId_ = 1;
    nextEffectId_ = 1;
    nextComputedId_ = 1;
}

} // namespace gui
} // namespace stratos
