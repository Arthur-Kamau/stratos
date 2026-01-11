#include "stratos/RuntimeLinkConfig.h"
#include <filesystem>
#include <sstream>
#include <iostream>
#include <vector>

// Platform-specific headers for getting executable path
#ifdef __linux__
#include <unistd.h>
#include <linux/limits.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#elif _WIN32
#include <windows.h>
#endif

namespace stratos {

RuntimeLinkConfig::RuntimeLinkConfig() {
    detectPlatform();
    findRuntimeLibrary();
}

void RuntimeLinkConfig::detectPlatform() {
#ifdef _WIN32
    platform = Platform::Windows;
#elif __APPLE__
    platform = Platform::macOS;
#elif __linux__
    platform = Platform::Linux;
#else
    platform = Platform::Unknown;
#endif
}

void RuntimeLinkConfig::findRuntimeLibrary() {
    namespace fs = std::filesystem;

    // Get the directory where the stratos executable is located
    std::string exeDir;

#ifdef __linux__
    // On Linux, read the symlink /proc/self/exe
    char exePath[4096];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
        exeDir = fs::path(exePath).parent_path().string();
    }
#elif __APPLE__
    // On macOS, use _NSGetExecutablePath
    char exePath[1024];
    uint32_t size = sizeof(exePath);
    if (_NSGetExecutablePath(exePath, &size) == 0) {
        exeDir = fs::path(exePath).parent_path().string();
    }
#elif _WIN32
    // On Windows, use GetModuleFileName
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    exeDir = fs::path(exePath).parent_path().string();
#endif

    // Try multiple locations for runtime library
    std::vector<std::string> searchPaths;

    // First, try same directory as executable
    if (!exeDir.empty()) {
        searchPaths.push_back(exeDir + "/libstratos_runtime.a");
    }

    // Try source tree locations (for development)
    searchPaths.push_back(fs::absolute(fs::path(__FILE__).parent_path() / "../../../build/libstratos_runtime.a").string());

    // Try current working directory and common locations
    searchPaths.push_back(fs::absolute("src/interpreter/cpp/build/libstratos_runtime.a").string());
    searchPaths.push_back(fs::absolute("build/libstratos_runtime.a").string());

    // System install locations
    searchPaths.push_back("/usr/local/lib/stratos/libstratos_runtime.a");
    searchPaths.push_back("/usr/lib/stratos/libstratos_runtime.a");

    for (const auto& path : searchPaths) {
        if (fs::exists(path)) {
            runtimeLibPath = path;
            return;
        }
    }

    // If not found, use default based on executable location
    if (!exeDir.empty()) {
        runtimeLibPath = exeDir + "/libstratos_runtime.a";
    } else {
        runtimeLibPath = "build/libstratos_runtime.a";
    }

    std::cerr << "Warning: Runtime library not found in standard locations. "
              << "Using: " << runtimeLibPath << std::endl;
}

std::string RuntimeLinkConfig::getStaticLinkFlags(OptimizationLevel optLevel) const {
    std::stringstream flags;

    // Optimization flags
    switch (optLevel) {
        case OptimizationLevel::O0:
            flags << "-O0 ";
            break;
        case OptimizationLevel::O1:
            flags << "-O1 ";
            break;
        case OptimizationLevel::O2:
            flags << "-O2 ";
            break;
        case OptimizationLevel::O3:
            flags << "-O3 -march=native ";
            break;
        case OptimizationLevel::Os:
            flags << "-Os ";
            break;
        case OptimizationLevel::Oz:
            flags << "-Oz ";
            break;
    }

    // Platform-specific static linking
    switch (platform) {
        case Platform::Linux: {
            // FULL STATIC LINKING on Linux
            flags << "-static ";                          // Static everything
            flags << "-static-libgcc -static-libstdc++ "; // Static C/C++ runtime
            flags << "\"" << runtimeLibPath << "\" ";     // Our runtime

            // Static system libraries
            // Use -l:lib.a syntax to force static version
            flags << "-l:libssl.a -l:libcrypto.a ";      // OpenSSL static
            flags << "-l:libpthread.a -l:libdl.a ";      // pthread and dl static

            // Ensure pthread is fully linked (circular dependencies)
            flags << "-Wl,--whole-archive -lpthread -Wl,--no-whole-archive ";

            // libc static (glibc)
            flags << "-l:libc.a ";

            break;
        }

        case Platform::macOS: {
            // PARTIAL STATIC LINKING on macOS
            // macOS doesn't allow full static linking of system libs since 10.5
            flags << "-static-libgcc -static-libstdc++ "; // Static C/C++ runtime
            flags << "\"" << runtimeLibPath << "\" ";     // Our runtime

            // Try to use static OpenSSL if available, else dynamic
            flags << "-lssl -lcrypto ";                   // Usually dynamic on macOS
            flags << "-lpthread ";                        // System pthread (dynamic)

            break;
        }

        case Platform::Windows: {
            // WINDOWS STATIC LINKING
            flags << "/MT ";                              // Static MSVC runtime
            flags << "\"" << runtimeLibPath << "\" ";     // Our runtime

            // Windows static libs
            flags << "libssl.lib libcrypto.lib ";         // OpenSSL (need static builds)
            flags << "ws2_32.lib advapi32.lib ";          // Winsock, crypto APIs
            flags << "crypt32.lib user32.lib ";           // Additional system libs

            break;
        }

        case Platform::Unknown: {
            // Fallback: try basic static linking
            std::cerr << "Warning: Unknown platform, using basic static linking" << std::endl;
            flags << "-static-libgcc -static-libstdc++ ";
            flags << "\"" << runtimeLibPath << "\" ";
            flags << "-lssl -lcrypto -lpthread -ldl ";
            break;
        }
    }

    return flags.str();
}

std::string RuntimeLinkConfig::getRuntimeLibraryPath() const {
    return runtimeLibPath;
}

bool RuntimeLinkConfig::isFullStaticSupported() const {
    // Only Linux truly supports full static linking
    return platform == Platform::Linux;
}

// Helper function implementations
std::string platformToString(Platform p) {
    switch (p) {
        case Platform::Linux: return "Linux";
        case Platform::Windows: return "Windows";
        case Platform::macOS: return "macOS";
        default: return "Unknown";
    }
}

std::string optLevelToString(OptimizationLevel level) {
    switch (level) {
        case OptimizationLevel::O0: return "-O0 (no optimization)";
        case OptimizationLevel::O1: return "-O1 (basic)";
        case OptimizationLevel::O2: return "-O2 (recommended)";
        case OptimizationLevel::O3: return "-O3 (aggressive)";
        case OptimizationLevel::Os: return "-Os (size)";
        case OptimizationLevel::Oz: return "-Oz (min size)";
        default: return "unknown";
    }
}

} // namespace stratos
