#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

namespace stratos {
namespace gui {

// ============================================================
// Easing Functions
// ============================================================

enum class EasingType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut
};

inline float applyEasing(float t, EasingType easing) {
    switch (easing) {
        case EasingType::Linear:
            return t;
        case EasingType::EaseIn:
            return t * t;
        case EasingType::EaseOut:
            return t * (2.0f - t);
        case EasingType::EaseInOut:
            return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
    }
    return t;
}

// ============================================================
// Tween
// ============================================================

class Tween {
public:
    Tween(float startValue, float endValue, float durationMs, EasingType easing = EasingType::Linear)
        : start_(startValue), end_(endValue), duration_(durationMs), easing_(easing) {}

    /// Update the tween by deltaMs milliseconds. Returns current value.
    float update(float deltaMs) {
        if (!running_) return getCurrentValue();

        elapsed_ += deltaMs;
        if (elapsed_ >= duration_) {
            elapsed_ = duration_;
            running_ = false;
            if (onComplete_) onComplete_();
        }

        float t = elapsed_ / duration_;
        t = applyEasing(t, easing_);
        currentValue_ = start_ + (end_ - start_) * t;

        if (onUpdate_) onUpdate_(currentValue_);
        return currentValue_;
    }

    void start() { running_ = true; elapsed_ = 0; currentValue_ = start_; }
    void stop() { running_ = false; }
    void reset() { elapsed_ = 0; currentValue_ = start_; running_ = false; }

    bool isRunning() const { return running_; }
    float getCurrentValue() const { return currentValue_; }
    float getProgress() const { return duration_ > 0 ? elapsed_ / duration_ : 1.0f; }

    void setOnUpdate(std::function<void(float)> fn) { onUpdate_ = std::move(fn); }
    void setOnComplete(std::function<void()> fn) { onComplete_ = std::move(fn); }

    // Configuration
    void setStartValue(float v) { start_ = v; }
    void setEndValue(float v) { end_ = v; }
    void setDuration(float ms) { duration_ = ms; }
    void setEasing(EasingType e) { easing_ = e; }

private:
    float start_ = 0;
    float end_ = 1;
    float duration_ = 300; // milliseconds
    EasingType easing_ = EasingType::Linear;
    float elapsed_ = 0;
    float currentValue_ = 0;
    bool running_ = false;
    std::function<void(float)> onUpdate_;
    std::function<void()> onComplete_;
};

// ============================================================
// Animation Manager
// ============================================================

class AnimationManager {
public:
    using TweenPtr = std::shared_ptr<Tween>;

    /// Create and register a new tween.
    TweenPtr createTween(float start, float end, float durationMs,
                          EasingType easing = EasingType::Linear) {
        auto tween = std::make_shared<Tween>(start, end, durationMs, easing);
        tweens_.push_back(tween);
        return tween;
    }

    /// Update all running animations. Call this every frame with delta time.
    void update(float deltaMs) {
        for (auto& tween : tweens_) {
            if (tween->isRunning()) {
                tween->update(deltaMs);
            }
        }
        // Remove completed, non-running tweens
        tweens_.erase(
            std::remove_if(tweens_.begin(), tweens_.end(),
                [](const TweenPtr& t) { return !t->isRunning() && t->getProgress() >= 1.0f; }),
            tweens_.end()
        );
    }

    /// Check if any animations are currently running.
    bool hasRunningAnimations() const {
        for (const auto& t : tweens_) {
            if (t->isRunning()) return true;
        }
        return false;
    }

    /// Remove all animations.
    void clear() { tweens_.clear(); }

    /// Get the number of active tweens.
    size_t count() const { return tweens_.size(); }

    /// Singleton instance
    static AnimationManager& instance() {
        static AnimationManager mgr;
        return mgr;
    }

private:
    std::vector<TweenPtr> tweens_;
};

} // namespace gui
} // namespace stratos
