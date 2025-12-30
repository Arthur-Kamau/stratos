#include "stratos/GarbageCollector.h"
#include "stratos/Interpreter.h"
#include <iostream>

namespace stratos {

GarbageCollector::GarbageCollector()
    : totalAllocations(0), lastCollectionCount(0), gcThreshold(100) {
}

GarbageCollector::~GarbageCollector() {
    // Cleanup on shutdown
    objects.clear();
    marked.clear();
}

void GarbageCollector::registerObject(std::shared_ptr<ClassInstance> obj) {
    if (!obj) return;

    // Store weak reference to avoid affecting refcount
    objects.push_back(std::weak_ptr<ClassInstance>(obj));
    totalAllocations++;

    // Periodically clean up expired weak pointers
    if (totalAllocations % 50 == 0) {
        cleanupExpired();
    }
}

void GarbageCollector::mark(const std::vector<RuntimeValue>& roots) {
    // Clear previous marks
    marked.clear();

    // Mark from all roots
    for (const auto& root : roots) {
        markValue(root);
    }
}

void GarbageCollector::markValue(const RuntimeValue& value) {
    // Only mark objects
    if (value.type != "object") {
        return;
    }

    // Get the object
    try {
        auto obj = value.asObject();
        if (obj) {
            markObject(obj);
        }
    } catch (...) {
        // Invalid object, skip
    }
}

void GarbageCollector::markObject(std::shared_ptr<ClassInstance> obj) {
    if (!obj) return;

    // Get raw pointer for marking
    ClassInstance* rawPtr = obj.get();

    // If already marked, skip (prevents infinite recursion)
    if (marked.count(rawPtr) > 0) {
        return;
    }

    // Mark this object
    marked.insert(rawPtr);

    // Recursively mark all fields
    for (const auto& field : obj->fields) {
        markValue(field.second);
    }
}

size_t GarbageCollector::sweep() {
    size_t collected = 0;

    // Iterate through all tracked objects
    for (auto& weakObj : objects) {
        // Check if object still exists
        if (auto obj = weakObj.lock()) {
            ClassInstance* rawPtr = obj.get();

            // If not marked and refcount > 1 (has references), it's in a cycle
            // Note: refcount is at least 1 from our shared_ptr here
            if (marked.count(rawPtr) == 0 && obj.use_count() > 1) {
                // Break cycles by clearing all object fields
                for (auto& field : obj->fields) {
                    if (field.second.type == "object") {
                        // Clear the object reference to break cycle
                        field.second = RuntimeValue(); // void value
                    }
                }
                collected++;
            }
        }
    }

    return collected;
}

size_t GarbageCollector::collect(const std::vector<RuntimeValue>& roots) {
    // Mark phase
    mark(roots);

    // Sweep phase
    size_t collected = sweep();

    // Clean up expired weak pointers
    cleanupExpired();

    lastCollectionCount = collected;
    return collected;
}

void GarbageCollector::cleanupExpired() {
    // Remove expired weak pointers
    objects.erase(
        std::remove_if(objects.begin(), objects.end(),
            [](const std::weak_ptr<ClassInstance>& wp) {
                return wp.expired();
            }),
        objects.end()
    );
}

size_t GarbageCollector::getTotalObjects() const {
    // Count non-expired objects
    size_t count = 0;
    for (const auto& weakObj : objects) {
        if (!weakObj.expired()) {
            count++;
        }
    }
    return count;
}

size_t GarbageCollector::getLiveObjects() const {
    return marked.size();
}

void GarbageCollector::setGCThreshold(size_t threshold) {
    gcThreshold = threshold;
}

bool GarbageCollector::shouldCollect() const {
    return (totalAllocations - lastCollectionCount) >= gcThreshold;
}

} // namespace stratos
