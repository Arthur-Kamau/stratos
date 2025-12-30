#ifndef STRATOS_CACHE_MANAGER_H
#define STRATOS_CACHE_MANAGER_H

#include <string>
#include <optional>
#include <filesystem>

namespace stratos {

/**
 * Manages global dependency cache at ~/.stratos/cache
 *
 * Purpose:
 * - Avoid re-downloading dependencies across projects
 * - Reduce disk usage by sharing dependencies
 * - Speed up dependency resolution
 *
 * Cache Structure:
 * ~/.stratos/cache/
 *   ├── git/
 *   │   ├── github.com_user_repo_v1.0.0/
 *   │   └── github.com_user_repo_abc1234/
 *   └── metadata/
 *       └── github.com_user_repo_v1.0.0.json
 */
class CacheManager {
public:
    /**
     * Initialize cache manager
     * Creates ~/.stratos/cache directory if it doesn't exist
     */
    CacheManager();

    /**
     * Get the cache directory path
     * Default: ~/.stratos/cache
     * Can be overridden with STRATOS_CACHE env variable
     */
    std::string getCacheDir() const;

    /**
     * Get cached dependency path
     * @param url Repository URL
     * @param version Tag, branch, or commit hash
     * @return Path to cached dependency if exists, nullopt otherwise
     */
    std::optional<std::string> getCachedDependency(const std::string& url,
                                                     const std::string& version) const;

    /**
     * Add dependency to cache
     * @param url Repository URL
     * @param version Tag, branch, or commit hash
     * @param sourcePath Path to dependency source
     * @return Path to cached dependency on success, empty string on failure
     */
    std::string cacheDependency(const std::string& url,
                                const std::string& version,
                                const std::string& sourcePath);

    /**
     * Check if a dependency is cached
     * @param url Repository URL
     * @param version Tag, branch, or commit hash
     * @return true if cached, false otherwise
     */
    bool isCached(const std::string& url, const std::string& version) const;

    /**
     * Get cache key for dependency
     * Converts URL and version to safe filesystem name
     * Example: "github.com/user/repo@v1.0.0" -> "github.com_user_repo_v1.0.0"
     */
    std::string getCacheKey(const std::string& url, const std::string& version) const;

    /**
     * Clear entire cache
     * @return true on success
     */
    bool clearCache();

    /**
     * Clean old/unused cache entries
     * Removes entries not accessed in the last N days
     * @param days Number of days of inactivity before cleaning
     * @return Number of entries cleaned
     */
    int cleanOldEntries(int days = 30);

    /**
     * Get cache statistics
     * @return Map with cache stats (size, entries, etc.)
     */
    struct CacheStats {
        size_t totalEntries;
        size_t totalSizeBytes;
        std::string humanReadableSize;
    };
    CacheStats getStats() const;

private:
    std::string cacheDir_;
    std::string gitCacheDir_;
    std::string metadataDir_;

    /**
     * Initialize cache directories
     */
    void initializeCacheDirectories();

    /**
     * Sanitize URL for filesystem use
     * Replaces special characters with underscores
     */
    std::string sanitizeForFilesystem(const std::string& input) const;

    /**
     * Copy directory recursively
     */
    bool copyDirectory(const std::filesystem::path& src,
                      const std::filesystem::path& dest) const;

    /**
     * Create symlink to cache
     */
    bool createSymlink(const std::string& target, const std::string& link) const;
};

} // namespace stratos

#endif // STRATOS_CACHE_MANAGER_H
