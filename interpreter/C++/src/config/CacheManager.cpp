#include "stratos/CacheManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>

namespace fs = std::filesystem;

namespace stratos {

// ============================================================================
// Constructor and Initialization
// ============================================================================

CacheManager::CacheManager() {
    // Check for STRATOS_CACHE environment variable
    const char* envCache = std::getenv("STRATOS_CACHE");
    if (envCache && std::strlen(envCache) > 0) {
        cacheDir_ = envCache;
    } else {
        // Default to ~/.stratos/cache
        const char* home = std::getenv("HOME");
        if (!home) {
            home = std::getenv("USERPROFILE"); // Windows fallback
        }

        if (home) {
            cacheDir_ = std::string(home) + "/.stratos/cache";
        } else {
            cacheDir_ = "/tmp/stratos-cache"; // Fallback
        }
    }

    gitCacheDir_ = cacheDir_ + "/git";
    metadataDir_ = cacheDir_ + "/metadata";

    initializeCacheDirectories();
}

void CacheManager::initializeCacheDirectories() {
    try {
        // Create cache directories if they don't exist
        if (!fs::exists(cacheDir_)) {
            fs::create_directories(cacheDir_);
            std::cout << "Created cache directory: " << cacheDir_ << std::endl;
        }

        if (!fs::exists(gitCacheDir_)) {
            fs::create_directories(gitCacheDir_);
        }

        if (!fs::exists(metadataDir_)) {
            fs::create_directories(metadataDir_);
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Warning: Failed to create cache directories: " << e.what() << std::endl;
    }
}

// ============================================================================
// Public API
// ============================================================================

std::string CacheManager::getCacheDir() const {
    return cacheDir_;
}

std::string CacheManager::getCacheKey(const std::string& url, const std::string& version) const {
    std::string key = url;

    // Remove protocol prefixes
    if (key.starts_with("https://")) {
        key = key.substr(8);
    } else if (key.starts_with("http://")) {
        key = key.substr(7);
    } else if (key.starts_with("git@")) {
        key = key.substr(4);
    }

    // Replace : with /  (for git@github.com:user/repo)
    std::replace(key.begin(), key.end(), ':', '/');

    // Remove .git suffix
    if (key.ends_with(".git")) {
        key = key.substr(0, key.length() - 4);
    }

    // Sanitize for filesystem
    key = sanitizeForFilesystem(key);

    // Append version if provided
    if (!version.empty() && version != "main" && version != "master") {
        std::string sanitizedVersion = sanitizeForFilesystem(version);
        key += "_" + sanitizedVersion;
    }

    return key;
}

std::string CacheManager::sanitizeForFilesystem(const std::string& input) const {
    std::string result = input;

    // Replace unsafe characters with underscores
    const std::string unsafe = "/:@?&=+$,#<>%\"'`|*";
    for (char& ch : result) {
        if (unsafe.find(ch) != std::string::npos) {
            ch = '_';
        }
    }

    return result;
}

std::optional<std::string> CacheManager::getCachedDependency(const std::string& url,
                                                               const std::string& version) const {
    std::string key = getCacheKey(url, version);
    std::string cachedPath = gitCacheDir_ + "/" + key;

    if (fs::exists(cachedPath) && fs::is_directory(cachedPath)) {
        return cachedPath;
    }

    return std::nullopt;
}

bool CacheManager::isCached(const std::string& url, const std::string& version) const {
    return getCachedDependency(url, version).has_value();
}

std::string CacheManager::cacheDependency(const std::string& url,
                                          const std::string& version,
                                          const std::string& sourcePath) {
    if (!fs::exists(sourcePath) || !fs::is_directory(sourcePath)) {
        std::cerr << "Source path does not exist: " << sourcePath << std::endl;
        return "";
    }

    std::string key = getCacheKey(url, version);
    std::string cachedPath = gitCacheDir_ + "/" + key;

    // If already cached, return path
    if (fs::exists(cachedPath)) {
        std::cout << "Dependency already cached: " << key << std::endl;
        return cachedPath;
    }

    try {
        // Copy directory to cache
        if (!copyDirectory(sourcePath, cachedPath)) {
            std::cerr << "Failed to copy dependency to cache" << std::endl;
            return "";
        }

        // Create metadata file
        std::string metadataPath = metadataDir_ + "/" + key + ".json";
        std::ofstream metaFile(metadataPath);
        if (metaFile.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            std::stringstream timestamp;
            timestamp << std::put_time(std::gmtime(&time_t_now), "%Y-%m-%dT%H:%M:%SZ");

            metaFile << "{\n";
            metaFile << "  \"url\": \"" << url << "\",\n";
            metaFile << "  \"version\": \"" << version << "\",\n";
            metaFile << "  \"cached_at\": \"" << timestamp.str() << "\",\n";
            metaFile << "  \"cache_key\": \"" << key << "\"\n";
            metaFile << "}\n";
        }

        std::cout << "✓ Cached dependency: " << key << std::endl;
        return cachedPath;

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Failed to cache dependency: " << e.what() << std::endl;
        return "";
    }
}

// ============================================================================
// Cache Management
// ============================================================================

bool CacheManager::clearCache() {
    try {
        if (fs::exists(gitCacheDir_)) {
            size_t removed = fs::remove_all(gitCacheDir_);
            std::cout << "Removed " << removed << " cache entries" << std::endl;
        }

        if (fs::exists(metadataDir_)) {
            fs::remove_all(metadataDir_);
        }

        // Recreate directories
        initializeCacheDirectories();

        std::cout << "✓ Cache cleared successfully" << std::endl;
        return true;

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Failed to clear cache: " << e.what() << std::endl;
        return false;
    }
}

int CacheManager::cleanOldEntries(int days) {
    int cleaned = 0;

    try {
        auto now = fs::file_time_type::clock::now();
        auto cutoff = now - std::chrono::hours(24 * days);

        if (!fs::exists(gitCacheDir_)) {
            return 0;
        }

        for (const auto& entry : fs::directory_iterator(gitCacheDir_)) {
            if (!entry.is_directory()) continue;

            auto lastWrite = entry.last_write_time();

            if (lastWrite < cutoff) {
                std::cout << "Removing old cache entry: " << entry.path().filename() << std::endl;
                fs::remove_all(entry.path());
                cleaned++;

                // Also remove metadata
                std::string metadataPath = metadataDir_ + "/" +
                                          entry.path().filename().string() + ".json";
                if (fs::exists(metadataPath)) {
                    fs::remove(metadataPath);
                }
            }
        }

        if (cleaned > 0) {
            std::cout << "✓ Cleaned " << cleaned << " old cache entries" << std::endl;
        } else {
            std::cout << "No old entries to clean" << std::endl;
        }

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error cleaning cache: " << e.what() << std::endl;
    }

    return cleaned;
}

CacheManager::CacheStats CacheManager::getStats() const {
    CacheStats stats = {0, 0, "0 B"};

    try {
        if (!fs::exists(gitCacheDir_)) {
            return stats;
        }

        for (const auto& entry : fs::directory_iterator(gitCacheDir_)) {
            if (!entry.is_directory()) continue;

            stats.totalEntries++;

            // Calculate size
            for (const auto& file : fs::recursive_directory_iterator(entry.path())) {
                if (file.is_regular_file()) {
                    stats.totalSizeBytes += file.file_size();
                }
            }
        }

        // Format human-readable size
        double size = static_cast<double>(stats.totalSizeBytes);
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;

        while (size >= 1024.0 && unitIndex < 4) {
            size /= 1024.0;
            unitIndex++;
        }

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
        stats.humanReadableSize = ss.str();

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error getting cache stats: " << e.what() << std::endl;
    }

    return stats;
}

// ============================================================================
// Helper Methods
// ============================================================================

bool CacheManager::copyDirectory(const fs::path& src, const fs::path& dest) const {
    try {
        // Create destination directory
        if (!fs::exists(dest)) {
            fs::create_directories(dest);
        }

        // Copy recursively
        fs::copy(src, dest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);

        return true;

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Copy error: " << e.what() << std::endl;
        return false;
    }
}

bool CacheManager::createSymlink(const std::string& target, const std::string& link) const {
    try {
        // Remove existing link if present
        if (fs::exists(link)) {
            fs::remove(link);
        }

        // Create symlink
        fs::create_directory_symlink(target, link);
        return true;

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Symlink error: " << e.what() << std::endl;
        return false;
    }
}

} // namespace stratos
