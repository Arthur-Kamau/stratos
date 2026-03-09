#pragma once
#include <string>

namespace stratos {

enum class Platform {
    Linux,
    Windows,
    macOS,
    Unknown
};

enum class OptimizationLevel {
    O0,  // No optimization - fast compile
    O1,  // Basic optimization
    O2,  // Recommended (default)
    O3,  // Aggressive optimization + native arch
    Os,  // Optimize for size
    Oz   // Aggressively optimize for size
};

class RuntimeLinkConfig {
public:
    RuntimeLinkConfig();

    // Get complete linking flags for static compilation
    std::string getStaticLinkFlags(OptimizationLevel optLevel) const;

    // Get path to runtime library
    std::string getRuntimeLibraryPath() const;

    // Check if platform supports full static linking
    bool isFullStaticSupported() const;

    // Get current platform
    Platform getPlatform() const { return platform; }

private:
    Platform platform;
    std::string runtimeLibPath;

    void detectPlatform();
    void findRuntimeLibrary();
};

// Helper functions
std::string platformToString(Platform p);
std::string optLevelToString(OptimizationLevel level);

} // namespace stratos
