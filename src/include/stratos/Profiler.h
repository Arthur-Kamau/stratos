#ifndef STRATOS_PROFILER_H
#define STRATOS_PROFILER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <atomic>

namespace stratos {

class DevToolsServer;

// Function profiling entry
struct FunctionProfile {
    std::string name;
    std::string file;
    double selfTimeMs = 0;
    double totalTimeMs = 0;
    int callCount = 0;
    double selfPercent = 0;
};

// Flame graph node
struct FlameNode {
    std::string name;
    double value = 0; // time in ms
    int depth = 0;
    std::vector<int> children; // indices into flameNodes array
};

// Complete profile result
struct ProfileResult {
    double durationMs = 0;
    int totalSamples = 0;
    std::vector<FunctionProfile> functions;
    std::vector<FlameNode> flameNodes;
};

// CPU Profiler - tracks function timing and call counts
class Profiler {
public:
    static Profiler& instance();

    void enable();
    void disable();
    bool isEnabled() const;

    // Start/stop profiling sessions
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const;

    // Called by Interpreter at function entry/exit
    void enterFunction(const std::string& name, const std::string& file);
    void exitFunction(const std::string& name);

    // Get aggregated profile data
    ProfileResult getProfile() const;

    // DevTools integration
    void setDevToolsServer(DevToolsServer* server);

private:
    Profiler();
    ~Profiler() = default;

    struct CallEntry {
        std::string name;
        std::string file;
        std::chrono::high_resolution_clock::time_point startTime;
        double childTimeMs = 0; // time spent in children
    };

    std::atomic<bool> enabled_;
    std::atomic<bool> profiling_;
    mutable std::mutex mutex_;

    std::chrono::high_resolution_clock::time_point profileStart_;
    std::chrono::high_resolution_clock::time_point profileEnd_;

    // Active call stack for timing
    std::vector<CallEntry> callStack_;

    // Accumulated function stats
    struct FunctionStats {
        std::string file;
        double selfTimeMs = 0;
        double totalTimeMs = 0;
        int callCount = 0;
    };
    std::unordered_map<std::string, FunctionStats> functionStats_;

    // Flame graph data
    struct FlameEntry {
        std::string name;
        double startMs;
        double endMs;
        int depth;
    };
    std::vector<FlameEntry> flameEntries_;

    DevToolsServer* devtools_ = nullptr;

    void buildFlameGraph(ProfileResult& result) const;
};

} // namespace stratos

#endif // STRATOS_PROFILER_H
