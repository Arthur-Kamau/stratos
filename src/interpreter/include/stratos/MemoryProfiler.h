#ifndef STRATOS_MEMORY_PROFILER_H
#define STRATOS_MEMORY_PROFILER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>

namespace stratos {

// Forward declarations
class GarbageCollector;
class DevToolsServer;

// GC collection event
struct GCEvent {
    std::chrono::system_clock::time_point timestamp;
    double pauseTimeMs;
    size_t freedBytes;
    size_t freedObjects;
    size_t cyclesBroken;
};

// Memory statistics snapshot
struct MemoryStats {
    size_t totalAllocated;
    size_t currentUsage;
    size_t objectCount;
    std::unordered_map<std::string, size_t> objectsByType;

    // GC statistics
    size_t collectionsTotal;
    size_t cyclesBrokenTotal;
    double avgPauseTimeMs;
    std::chrono::system_clock::time_point lastCollectionTime;
};

// Memory profiler that tracks GC stats and heap information
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();

    // Singleton access
    static MemoryProfiler& instance();

    // Enable/disable profiling
    void enable();
    void disable();
    bool isEnabled() const;

    // Set the GC instance to monitor
    void setGarbageCollector(GarbageCollector* gc);

    // Record a GC collection event
    void recordCollection(double pauseTimeMs, size_t freedObjects, size_t cyclesBroken);

    // Get current memory statistics
    MemoryStats getStats() const;

    // Get GC event history
    std::vector<GCEvent> getGCHistory(size_t maxEvents = 100) const;

    // DevTools integration
    void setDevToolsServer(DevToolsServer* server);

    // Send stats update event to DevTools
    void broadcastStatsUpdate();

private:
    bool enabled_;
    GarbageCollector* gc_;
    DevToolsServer* devtools_;

    mutable std::mutex mutex_;

    // GC event history
    std::vector<GCEvent> gcHistory_;
    size_t maxHistorySize_;

    // Cumulative statistics
    size_t totalCollections_;
    size_t totalCyclesBroken_;
    double totalPauseTimeMs_;

    // Helper to estimate memory usage
    size_t estimateCurrentUsage() const;
};

} // namespace stratos

#endif // STRATOS_MEMORY_PROFILER_H
