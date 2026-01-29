#include "stratos/MemoryProfiler.h"
#include "stratos/GarbageCollector.h"
#include "stratos/DevToolsServer.h"
#include <sstream>
#include <algorithm>

namespace stratos {

MemoryProfiler::MemoryProfiler()
    : enabled_(false)
    , gc_(nullptr)
    , devtools_(nullptr)
    , maxHistorySize_(100)
    , totalCollections_(0)
    , totalCyclesBroken_(0)
    , totalPauseTimeMs_(0.0) {
}

MemoryProfiler::~MemoryProfiler() {
}

MemoryProfiler& MemoryProfiler::instance() {
    static MemoryProfiler instance;
    return instance;
}

void MemoryProfiler::enable() {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = true;
}

void MemoryProfiler::disable() {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = false;
}

bool MemoryProfiler::isEnabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return enabled_;
}

void MemoryProfiler::setGarbageCollector(GarbageCollector* gc) {
    std::lock_guard<std::mutex> lock(mutex_);
    gc_ = gc;
}

void MemoryProfiler::setDevToolsServer(DevToolsServer* server) {
    std::lock_guard<std::mutex> lock(mutex_);
    devtools_ = server;
}

void MemoryProfiler::recordCollection(double pauseTimeMs, size_t freedObjects, size_t cyclesBroken) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!enabled_) return;

    // Create GC event
    GCEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.pauseTimeMs = pauseTimeMs;
    event.freedBytes = freedObjects * 64; // Estimate: ~64 bytes per object
    event.freedObjects = freedObjects;
    event.cyclesBroken = cyclesBroken;

    // Add to history
    gcHistory_.push_back(event);

    // Limit history size
    if (gcHistory_.size() > maxHistorySize_) {
        gcHistory_.erase(gcHistory_.begin());
    }

    // Update cumulative stats
    totalCollections_++;
    totalCyclesBroken_ += cyclesBroken;
    totalPauseTimeMs_ += pauseTimeMs;

    // Broadcast event to DevTools if connected
    if (devtools_) {
        std::ostringstream oss;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            event.timestamp.time_since_epoch()).count();

        oss << "{";
        oss << "\"timestamp\":" << ms << ",";
        oss << "\"pauseTime\":" << pauseTimeMs << ",";
        oss << "\"freedBytes\":" << event.freedBytes << ",";
        oss << "\"freedObjects\":" << freedObjects << ",";
        oss << "\"cyclesBroken\":" << cyclesBroken;
        oss << "}";

        devtools_->broadcastEvent("Memory.gcPerformed", oss.str());
    }
}

MemoryStats MemoryProfiler::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);

    MemoryStats stats;

    if (gc_) {
        stats.objectCount = gc_->getTotalObjects();
        stats.totalAllocated = estimateCurrentUsage();
        stats.currentUsage = estimateCurrentUsage();
    } else {
        stats.objectCount = 0;
        stats.totalAllocated = 0;
        stats.currentUsage = 0;
    }

    // GC stats
    stats.collectionsTotal = totalCollections_;
    stats.cyclesBrokenTotal = totalCyclesBroken_;
    stats.avgPauseTimeMs = totalCollections_ > 0
        ? (totalPauseTimeMs_ / totalCollections_)
        : 0.0;

    if (!gcHistory_.empty()) {
        stats.lastCollectionTime = gcHistory_.back().timestamp;
    } else {
        stats.lastCollectionTime = std::chrono::system_clock::now();
    }

    // Object counts by type (placeholder - would need type tracking)
    // For now, just report total
    stats.objectsByType["Object"] = stats.objectCount;

    return stats;
}

std::vector<GCEvent> MemoryProfiler::getGCHistory(size_t maxEvents) const {
    std::lock_guard<std::mutex> lock(mutex_);

    if (maxEvents >= gcHistory_.size()) {
        return gcHistory_;
    }

    // Return last N events
    return std::vector<GCEvent>(
        gcHistory_.end() - maxEvents,
        gcHistory_.end()
    );
}

void MemoryProfiler::broadcastStatsUpdate() {
    if (!enabled_ || !devtools_) return;

    auto stats = getStats();

    std::ostringstream oss;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    oss << "{";
    oss << "\"timestamp\":" << ms << ",";
    oss << "\"currentUsage\":" << stats.currentUsage << ",";
    oss << "\"objectCount\":" << stats.objectCount;
    oss << "}";

    devtools_->broadcastEvent("Memory.statsUpdated", oss.str());
}

size_t MemoryProfiler::estimateCurrentUsage() const {
    // Estimate: ~64 bytes per object on average
    // This is a rough estimate; a more accurate implementation would
    // track actual allocation sizes
    if (gc_) {
        return gc_->getTotalObjects() * 64;
    }
    return 0;
}

} // namespace stratos
