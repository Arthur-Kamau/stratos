#ifndef STRATOS_GARBAGE_COLLECTOR_H
#define STRATOS_GARBAGE_COLLECTOR_H

#include <memory>
#include <unordered_set>
#include <vector>

namespace stratos {

// Forward declarations
struct ClassInstance;
struct RuntimeValue;

// Garbage Collector using mark-and-sweep algorithm for cycle detection
// Works alongside reference counting to collect circular references
class GarbageCollector {
public:
    GarbageCollector();
    ~GarbageCollector();

    // Register a newly allocated object
    void registerObject(std::shared_ptr<ClassInstance> obj);

    // Mark phase: Mark all reachable objects from roots
    void mark(const std::vector<RuntimeValue>& roots);

    // Sweep phase: Collect unmarked objects
    size_t sweep();

    // Full collection cycle: mark from roots, then sweep
    size_t collect(const std::vector<RuntimeValue>& roots);

    // Get statistics
    size_t getTotalObjects() const;
    size_t getLiveObjects() const;

    // Configuration
    void setGCThreshold(size_t threshold);
    bool shouldCollect() const;

private:
    // Track all allocated objects using weak_ptr
    std::vector<std::weak_ptr<ClassInstance>> objects;

    // Mark set for current GC cycle
    std::unordered_set<ClassInstance*> marked;

    // Statistics
    size_t totalAllocations;
    size_t lastCollectionCount;
    size_t gcThreshold; // Trigger GC after this many allocations

    // Helper methods
    void markValue(const RuntimeValue& value);
    void markObject(std::shared_ptr<ClassInstance> obj);
    void cleanupExpired();
};

} // namespace stratos

#endif // STRATOS_GARBAGE_COLLECTOR_H
