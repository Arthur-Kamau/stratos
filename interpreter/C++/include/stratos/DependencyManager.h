#ifndef STRATOS_DEPENDENCY_MANAGER_H
#define STRATOS_DEPENDENCY_MANAGER_H

#include <string>
#include <map>
#include <optional>
#include <vector>
#include <set>

namespace stratos {

/**
 * Represents a dependency specification
 */
struct Dependency {
    std::string name;           // Package name
    std::string url;            // Full URL or path
    std::string type;           // "git" or "local"
    std::string tag;            // Git tag (optional)
    std::string branch;         // Git branch (optional)
    std::string hash;           // Git commit hash (optional)
    std::string localPath;      // For local dependencies

    // Parse dependency string (e.g., "github.com/user/repo@v1.0.0")
    static std::optional<Dependency> parse(const std::string& name, const std::string& spec);

    // Get the version to use (tag, branch, or hash)
    std::string getVersion() const;
};

/**
 * Manages downloading and caching of dependencies
 */
class DependencyManager {
public:
    DependencyManager(const std::string& projectRoot);

    // Fetch a single dependency by URL
    bool fetchDependency(const std::string& url, bool verbose = false);

    // Fetch all dependencies from stratos.conf
    bool fetchAllDependencies(bool verbose = false);

    // Get the dependencies directory path
    std::string getDepsDir() const;

    // Check if a dependency exists locally
    bool dependencyExists(const std::string& name) const;

    // Get path to a dependency
    std::string getDependencyPath(const std::string& name) const;

    // List all installed dependencies
    std::vector<std::string> listDependencies() const;

    // Update all dependencies (refetch and regenerate lock file)
    bool updateAllDependencies(bool verbose = false);

private:
    std::string projectRoot_;
    std::string depsDir_;

    // Clone a git repository with retry logic
    bool cloneGitRepo(const std::string& url, const std::string& destPath,
                     const std::string& tag, const std::string& branch,
                     const std::string& hash, bool verbose);

    // Copy local dependency
    bool copyLocalDependency(const std::string& srcPath, const std::string& destPath,
                            bool verbose);

    // Fetch dependencies recursively (for transitive dependencies)
    bool fetchDependenciesRecursive(const std::string& depPath,
                                   std::set<std::string>& visited,
                                   int depth, bool verbose);

    // Validate dependency (check for stratos.conf and src/)
    bool validateDependency(const std::string& depPath, bool verbose = false) const;

    // Parse GitHub-style URL
    static std::optional<Dependency> parseGitHubURL(const std::string& url);

    // Save dependency metadata for caching
    bool saveMetadata(const std::string& name, const Dependency& dep,
                     const std::string& resolvedCommit = "");

    // Load dependency metadata
    std::optional<Dependency> loadMetadata(const std::string& name);

    // Check if dependency needs updating
    bool needsUpdate(const std::string& name, const Dependency& dep);
};

} // namespace stratos

#endif // STRATOS_DEPENDENCY_MANAGER_H
