#ifndef STRATOS_LOCKFILE_H
#define STRATOS_LOCKFILE_H

#include <string>
#include <vector>
#include <optional>

namespace stratos {

/**
 * Represents a locked dependency with exact version information
 */
struct LockedDependency {
    std::string name;
    std::string url;
    std::string type;                      // "git" or "local"
    std::string tag;                       // optional
    std::string branch;                    // optional
    std::string hash;                      // optional
    std::string resolvedCommit;            // For git: actual commit hash fetched
    std::string checksum;                  // For local: SHA256 checksum
    std::string via;                       // For transitive deps: parent dependency name
};

/**
 * Manages the stratos.lock file for reproducible builds
 */
class LockFileManager {
public:
    /**
     * Load lock file from path
     * @param lockFilePath Path to stratos.lock file
     * @return Vector of locked dependencies, or nullopt if file doesn't exist or is invalid
     */
    static std::optional<std::vector<LockedDependency>> load(const std::string& lockFilePath);

    /**
     * Save lock file to path
     * @param lockFilePath Path to stratos.lock file
     * @param deps Vector of locked dependencies to save
     * @param stratosVersion Version of Stratos that generated the lock file
     * @return true if successful
     */
    static bool save(const std::string& lockFilePath,
                    const std::vector<LockedDependency>& deps,
                    const std::string& stratosVersion = "0.1.0");

    /**
     * Verify that lock file matches installed dependencies
     * @param projectRoot Project root directory
     * @return true if lock file matches installed dependencies
     */
    static bool verify(const std::string& projectRoot);

    /**
     * Generate lock file from currently installed dependencies
     * @param projectRoot Project root directory
     * @return true if successful
     */
    static bool generate(const std::string& projectRoot);

private:
    /**
     * Parse a single dependency object from HOCON content
     */
    static std::optional<LockedDependency> parseDependencyObject(const std::string& content,
                                                                 size_t& pos);

    /**
     * Skip whitespace and comments in HOCON content
     */
    static void skipWhitespaceAndComments(const std::string& content, size_t& pos);

    /**
     * Parse a string value (quoted or unquoted) from HOCON content
     */
    static std::string parseValue(const std::string& content, size_t& pos);

    /**
     * Parse a key from HOCON content
     */
    static std::string parseKey(const std::string& content, size_t& pos);

    /**
     * Calculate SHA256 checksum of a directory
     */
    static std::string calculateChecksum(const std::string& dirPath);
};

} // namespace stratos

#endif // STRATOS_LOCKFILE_H
