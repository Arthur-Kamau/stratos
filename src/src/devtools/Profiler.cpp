#include "stratos/Profiler.h"
#include "stratos/DevToolsServer.h"
#include <algorithm>
#include <sstream>
#include <cmath>

namespace stratos {

Profiler& Profiler::instance() {
    static Profiler inst;
    return inst;
}

Profiler::Profiler() : enabled_(false), profiling_(false) {}

void Profiler::enable() { enabled_ = true; }
void Profiler::disable() { enabled_ = false; profiling_ = false; }
bool Profiler::isEnabled() const { return enabled_; }
bool Profiler::isProfiling() const { return profiling_; }

void Profiler::startProfiling() {
    if (!enabled_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    functionStats_.clear();
    flameEntries_.clear();
    callStack_.clear();
    profileStart_ = std::chrono::high_resolution_clock::now();
    profiling_ = true;

    if (devtools_) {
        devtools_->broadcastEvent("Profiler.started", "{}");
    }
}

void Profiler::stopProfiling() {
    if (!profiling_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    profileEnd_ = std::chrono::high_resolution_clock::now();
    profiling_ = false;

    if (devtools_) {
        devtools_->broadcastEvent("Profiler.stopped", "{}");
    }
}

void Profiler::enterFunction(const std::string& name, const std::string& file) {
    if (!profiling_) return;
    std::lock_guard<std::mutex> lock(mutex_);

    CallEntry entry;
    entry.name = name;
    entry.file = file;
    entry.startTime = std::chrono::high_resolution_clock::now();
    entry.childTimeMs = 0;
    callStack_.push_back(entry);

    // Initialize function stats if new
    if (functionStats_.find(name) == functionStats_.end()) {
        functionStats_[name].file = file;
    }
    functionStats_[name].callCount++;
}

void Profiler::exitFunction(const std::string& name) {
    if (!profiling_) return;
    std::lock_guard<std::mutex> lock(mutex_);

    if (callStack_.empty()) return;

    // Find the matching entry (should be the last one)
    if (callStack_.back().name != name) return;

    auto& entry = callStack_.back();
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalMs = std::chrono::duration<double, std::milli>(endTime - entry.startTime).count();
    double selfMs = totalMs - entry.childTimeMs;

    // Update function stats
    auto& stats = functionStats_[name];
    stats.totalTimeMs += totalMs;
    stats.selfTimeMs += selfMs;

    // Record flame entry
    double startMs = std::chrono::duration<double, std::milli>(entry.startTime - profileStart_).count();
    FlameEntry flame;
    flame.name = name;
    flame.startMs = startMs;
    flame.endMs = startMs + totalMs;
    flame.depth = static_cast<int>(callStack_.size()) - 1;
    flameEntries_.push_back(flame);

    callStack_.pop_back();

    // Add to parent's child time
    if (!callStack_.empty()) {
        callStack_.back().childTimeMs += totalMs;
    }
}

ProfileResult Profiler::getProfile() const {
    std::lock_guard<std::mutex> lock(mutex_);
    ProfileResult result;

    auto end = profiling_ ? std::chrono::high_resolution_clock::now() : profileEnd_;
    result.durationMs = std::chrono::duration<double, std::milli>(end - profileStart_).count();
    result.totalSamples = static_cast<int>(flameEntries_.size());

    // Build function list sorted by self time
    for (const auto& [name, stats] : functionStats_) {
        FunctionProfile fp;
        fp.name = name;
        fp.file = stats.file;
        fp.selfTimeMs = stats.selfTimeMs;
        fp.totalTimeMs = stats.totalTimeMs;
        fp.callCount = stats.callCount;
        fp.selfPercent = result.durationMs > 0 ? (stats.selfTimeMs / result.durationMs) * 100.0 : 0;
        result.functions.push_back(fp);
    }

    // Sort by self time descending
    std::sort(result.functions.begin(), result.functions.end(),
        [](const FunctionProfile& a, const FunctionProfile& b) {
            return a.selfTimeMs > b.selfTimeMs;
        });

    // Build flame graph
    buildFlameGraph(result);

    return result;
}

void Profiler::buildFlameGraph(ProfileResult& result) const {
    // Convert flame entries to a tree structure
    // Root node
    FlameNode root;
    root.name = "(root)";
    root.value = result.durationMs;
    root.depth = 0;
    result.flameNodes.push_back(root);

    // Group entries by depth and create nodes
    for (const auto& entry : flameEntries_) {
        FlameNode node;
        node.name = entry.name;
        node.value = entry.endMs - entry.startMs;
        node.depth = entry.depth + 1; // +1 because root is depth 0
        result.flameNodes.push_back(node);
    }
}

void Profiler::setDevToolsServer(DevToolsServer* server) {
    devtools_ = server;
}

} // namespace stratos
