#include "stratos/DependencyManager.h"
#include "stratos/ProjectConfig.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <regex>
#include <chrono>
#include <thread>
#include <iomanip>

namespace fs = std::filesystem;

namespace stratos {

// ============================================================================
// Dependency Implementation
// ============================================================================

std::string Dependency::getVersion() const {
    if (!tag.empty()) return tag;
    if (!branch.empty()) return branch;
    if (!hash.empty()) return hash;
    return "main"; // Default
}

std::optional<Dependency> Dependency::parse(const std::string& name, const std::string& spec) {
    Dependency dep;
    dep.name = name;

    // Check if it's a local path (starts with "path:" or "/" or "./" or "../")
    if (spec.starts_with("path:")) {
        dep.type = "local";
        dep.localPath = spec.substr(5); // Remove "path:" prefix
        return dep;
    }
    if (spec.starts_with("/") || spec.starts_with("./") || spec.starts_with("../")) {
        dep.type = "local";
        dep.localPath = spec;
        return dep;
    }

    // Parse version/tag/branch (format: "url@version")
    size_t atPos = spec.find('@');
    std::string version;
    if (atPos != std::string::npos) {
        dep.url = spec.substr(0, atPos);
        version = spec.substr(atPos + 1);
    } else {
        dep.url = spec;
        version = "main"; // Default branch
    }

    // Determine type from URL
    if (dep.url.find("github.com") != std::string::npos ||
        dep.url.find("gitlab.com") != std::string::npos ||
        dep.url.starts_with("https://") || dep.url.starts_with("http://") ||
        dep.url.starts_with("git@")) {
        dep.type = "git";
    } else if (dep.url.find("/") != std::string::npos) {
        // Assume github.com if it looks like "user/repo"
        dep.type = "git";
        dep.url = "https://github.com/" + dep.url;
    } else {
        return std::nullopt; // Invalid format
    }

    // Determine if version is tag, branch, or hash
    if (!version.empty()) {
        // Check if it's a commit hash (7-40 hex characters)
        std::regex hashRegex("^[0-9a-f]{7,40}$");
        if (std::regex_match(version, hashRegex)) {
            dep.hash = version;
        }
        // Check if it's a semantic version tag (starts with 'v' or is numeric)
        else if (version[0] == 'v' || std::isdigit(version[0])) {
            dep.tag = version;
        }
        // Otherwise, it's a branch name
        else {
            dep.branch = version;
        }
    }

    return dep;
}

// ============================================================================
// DependencyManager Implementation
// ============================================================================

DependencyManager::DependencyManager(const std::string& projectRoot)
    : projectRoot_(projectRoot) {
    depsDir_ = projectRoot_ + "/deps";
}

std::string DependencyManager::getDepsDir() const {
    return depsDir_;
}

bool DependencyManager::dependencyExists(const std::string& name) const {
    std::string depPath = depsDir_ + "/" + name;
    return fs::exists(depPath) && fs::is_directory(depPath);
}

std::string DependencyManager::getDependencyPath(const std::string& name) const {
    return depsDir_ + "/" + name;
}

std::vector<std::string> DependencyManager::listDependencies() const {
    std::vector<std::string> deps;

    if (!fs::exists(depsDir_)) {
        return deps;
    }

    for (const auto& entry : fs::directory_iterator(depsDir_)) {
        if (entry.is_directory()) {
            deps.push_back(entry.path().filename().string());
        }
    }

    return deps;
}

// ============================================================================
// Dependency Validation
// ============================================================================

bool DependencyManager::validateDependency(const std::string& depPath, bool verbose) const {
    std::vector<std::string> errors;

    // Check for stratos.conf
    std::string configPath = depPath + "/stratos.conf";
    if (!fs::exists(configPath)) {
        errors.push_back("Missing stratos.conf");
    } else {
        // Try to parse config
        auto configOpt = ProjectConfigParser::parse(configPath);
        if (!configOpt) {
            errors.push_back("Invalid stratos.conf format");
        } else {
            // Validate required fields
            if (configOpt->name.empty()) {
                errors.push_back("Missing project name in stratos.conf");
            }
            if (configOpt->version.empty()) {
                errors.push_back("Missing version in stratos.conf");
            }
        }
    }

    // Check for src/ directory
    std::string srcPath = depPath + "/src";
    if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) {
        errors.push_back("Missing src/ directory");
    } else {
        // Check for at least one .st file
        bool hasSourceFiles = false;
        for (const auto& entry : fs::directory_iterator(srcPath)) {
            if (entry.path().extension() == ".st") {
                hasSourceFiles = true;
                break;
            }
        }
        if (!hasSourceFiles) {
            errors.push_back("No .st source files in src/ directory");
        }
    }

    // Report errors
    if (!errors.empty()) {
        std::cerr << "Dependency validation failed for: " << depPath << std::endl;
        for (const auto& error : errors) {
            std::cerr << "  - " << error << std::endl;
        }
        return false;
    }

    if (verbose) {
        std::cout << "✓ Dependency validated successfully: " << depPath << std::endl;
    }

    return true;
}

// ============================================================================
// Metadata Management (Dependency Cache)
// ============================================================================

bool DependencyManager::saveMetadata(const std::string& name, const Dependency& dep,
                                     const std::string& resolvedCommit) {
    std::string metaPath = depsDir_ + "/" + name + "/.stratos-meta.json";

    std::ofstream file(metaPath);
    if (!file.is_open()) {
        std::cerr << "Warning: Failed to save metadata for " << name << std::endl;
        return false;
    }

    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream timestamp;
    timestamp << std::put_time(std::gmtime(&time_t_now), "%Y-%m-%dT%H:%M:%SZ");

    // Write JSON metadata
    file << "{\n";
    file << "  \"name\": \"" << dep.name << "\",\n";
    file << "  \"url\": \"" << dep.url << "\",\n";
    file << "  \"type\": \"" << dep.type << "\",\n";
    if (!dep.tag.empty()) {
        file << "  \"tag\": \"" << dep.tag << "\",\n";
    }
    if (!dep.branch.empty()) {
        file << "  \"branch\": \"" << dep.branch << "\",\n";
    }
    if (!dep.hash.empty()) {
        file << "  \"hash\": \"" << dep.hash << "\",\n";
    }
    if (!resolvedCommit.empty()) {
        file << "  \"resolved_commit\": \"" << resolvedCommit << "\",\n";
    }
    file << "  \"fetched_at\": \"" << timestamp.str() << "\"\n";
    file << "}\n";

    return true;
}

std::optional<Dependency> DependencyManager::loadMetadata(const std::string& name) {
    std::string metaPath = depsDir_ + "/" + name + "/.stratos-meta.json";

    if (!fs::exists(metaPath)) {
        return std::nullopt;
    }

    // Simple JSON parsing (we only need basic fields)
    std::ifstream file(metaPath);
    if (!file.is_open()) {
        return std::nullopt;
    }

    Dependency dep;
    std::string line;
    while (std::getline(file, line)) {
        // Extract key-value pairs (simple parsing)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            // Remove quotes, spaces, commas
            key.erase(std::remove_if(key.begin(), key.end(),
                [](char c) { return c == '"' || c == ' ' || c == '\t'; }), key.end());
            value.erase(std::remove_if(value.begin(), value.end(),
                [](char c) { return c == '"' || c == ' ' || c == '\t' || c == ','; }), value.end());

            if (key == "name") dep.name = value;
            else if (key == "url") dep.url = value;
            else if (key == "type") dep.type = value;
            else if (key == "tag") dep.tag = value;
            else if (key == "branch") dep.branch = value;
            else if (key == "hash") dep.hash = value;
        }
    }

    if (dep.name.empty() || dep.url.empty()) {
        return std::nullopt;
    }

    return dep;
}

bool DependencyManager::needsUpdate(const std::string& name, const Dependency& dep) {
    auto existingOpt = loadMetadata(name);
    if (!existingOpt) {
        return true; // No metadata, needs fetch
    }

    Dependency existing = *existingOpt;

    // Compare URLs
    if (existing.url != dep.url) {
        return true;
    }

    // Compare versions
    if (existing.tag != dep.tag || existing.branch != dep.branch || existing.hash != dep.hash) {
        return true;
    }

    return false;
}

// ============================================================================
// Git Operations with Retry Logic
// ============================================================================

bool DependencyManager::cloneGitRepo(const std::string& url, const std::string& destPath,
                                     const std::string& tag, const std::string& branch,
                                     const std::string& hash, bool verbose) {
    // Create deps directory if it doesn't exist
    if (!fs::exists(depsDir_)) {
        fs::create_directories(depsDir_);
    }

    // Remove existing directory if present
    if (fs::exists(destPath)) {
        if (verbose) {
            std::cout << "Removing existing dependency at: " << destPath << std::endl;
        }
        fs::remove_all(destPath);
    }

    // Determine version to use (priority: tag > branch > hash > default)
    std::string version;
    if (!tag.empty()) {
        version = tag;
    } else if (!branch.empty()) {
        version = branch;
    } else if (!hash.empty()) {
        version = hash;
    } else {
        version = "main";
    }

    // Build git clone command
    std::string command = "git clone";

    // For tags and branches (not commit hashes), use --branch for efficiency
    if (!hash.empty()) {
        // For commit hashes, clone full repo then checkout
        command += " " + url + " " + destPath + " 2>&1";
    } else {
        // For tags/branches, use --single-branch
        command += " --branch " + version + " --single-branch --depth 1 ";
        command += url + " " + destPath + " 2>&1";
    }

    if (verbose) {
        std::cout << "Cloning: " << url;
        if (!version.empty() && version != "main") {
            std::cout << " (version: " << version << ")";
        }
        std::cout << std::endl;
        std::cout << "Command: " << command << std::endl;
    }

    // Retry logic: up to 3 attempts with 2-second delays
    const int MAX_RETRIES = 3;
    const int RETRY_DELAY_SECONDS = 2;
    int result = -1;

    for (int attempt = 1; attempt <= MAX_RETRIES; attempt++) {
        if (attempt > 1) {
            std::cout << "Retry attempt " << attempt << "/" << MAX_RETRIES << "..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(RETRY_DELAY_SECONDS));
        }

        // Execute git clone
        result = std::system(command.c_str());

        if (result == 0) {
            break; // Success
        } else if (attempt < MAX_RETRIES) {
            std::cerr << "Clone failed, retrying..." << std::endl;
        }
    }

    if (result != 0) {
        std::cerr << "Failed to clone after " << MAX_RETRIES << " attempts: " << url << std::endl;
        return false;
    }

    // If version is a commit hash, checkout that commit
    if (!hash.empty()) {
        std::string checkoutCmd = "cd " + destPath + " && git checkout " + hash + " 2>&1";
        if (verbose) {
            std::cout << "Checking out commit: " << hash << std::endl;
        }
        int checkoutResult = std::system(checkoutCmd.c_str());
        if (checkoutResult != 0) {
            std::cerr << "Warning: Failed to checkout commit " << hash << std::endl;
            return false;
        }
    }

    // Get the actual commit hash for metadata
    std::string resolvedCommit;
    std::string getCommitCmd = "cd " + destPath + " && git rev-parse HEAD 2>&1";
    FILE* pipe = popen(getCommitCmd.c_str(), "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            resolvedCommit = buffer;
            // Remove trailing newline
            if (!resolvedCommit.empty() && resolvedCommit.back() == '\n') {
                resolvedCommit.pop_back();
            }
        }
        pclose(pipe);
    }

    // Remove .git directory to save space
    std::string gitDir = destPath + "/.git";
    if (fs::exists(gitDir)) {
        fs::remove_all(gitDir);
    }

    // Validate dependency
    if (!validateDependency(destPath, verbose)) {
        return false;
    }

    // Save metadata for caching
    Dependency metaDep;
    metaDep.name = fs::path(destPath).filename().string();
    metaDep.url = url;
    metaDep.type = "git";
    metaDep.tag = tag;
    metaDep.branch = branch;
    metaDep.hash = hash;
    saveMetadata(metaDep.name, metaDep, resolvedCommit);

    return true;
}

// ============================================================================
// Local Dependency Operations
// ============================================================================

bool DependencyManager::copyLocalDependency(const std::string& srcPath, const std::string& destPath,
                                            bool verbose) {
    if (!fs::exists(srcPath)) {
        std::cerr << "Local dependency not found: " << srcPath << std::endl;
        return false;
    }

    // Create deps directory if it doesn't exist
    if (!fs::exists(depsDir_)) {
        fs::create_directories(depsDir_);
    }

    // Remove existing directory if present
    if (fs::exists(destPath)) {
        if (verbose) {
            std::cout << "Removing existing dependency at: " << destPath << std::endl;
        }
        fs::remove_all(destPath);
    }

    if (verbose) {
        std::cout << "Copying local dependency: " << srcPath << " -> " << destPath << std::endl;
    }

    // Copy directory
    try {
        fs::copy(srcPath, destPath, fs::copy_options::recursive);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Failed to copy local dependency: " << e.what() << std::endl;
        return false;
    }

    // Validate dependency
    if (!validateDependency(destPath, verbose)) {
        return false;
    }

    // Save metadata
    Dependency metaDep;
    metaDep.name = fs::path(destPath).filename().string();
    metaDep.url = srcPath;
    metaDep.type = "local";
    metaDep.localPath = srcPath;
    saveMetadata(metaDep.name, metaDep);

    return true;
}

// ============================================================================
// Transitive Dependency Resolution
// ============================================================================

bool DependencyManager::fetchDependenciesRecursive(const std::string& depPath,
                                                   std::set<std::string>& visited,
                                                   int depth, bool verbose) {
    // Check for circular dependencies
    std::string depName = fs::path(depPath).filename().string();

    if (visited.count(depName) > 0) {
        std::cerr << "Circular dependency detected: " << depName << std::endl;
        return false;
    }

    visited.insert(depName);

    // Limit recursion depth
    if (depth > 10) {
        std::cerr << "Maximum dependency depth exceeded (10 levels)" << std::endl;
        return false;
    }

    // Check if dependency has its own stratos.conf
    std::string configPath = depPath + "/stratos.conf";
    if (!fs::exists(configPath)) {
        // No config, no transitive dependencies
        return true;
    }

    // Parse config
    auto configOpt = ProjectConfigParser::parse(configPath);
    if (!configOpt) {
        std::cerr << "Warning: Failed to parse stratos.conf in " << depPath << std::endl;
        return true; // Don't fail the whole operation
    }

    const auto& deps = configOpt->dependencies;

    if (deps.empty()) {
        // No transitive dependencies
        return true;
    }

    if (verbose) {
        std::cout << std::string(depth * 2, ' ') << "├─ " << depName
                  << " has " << deps.size() << " transitive dependencies" << std::endl;
    }

    // Fetch each transitive dependency
    for (const auto& depEntry : deps) {
        Dependency dep;
        dep.name = depEntry.name;
        dep.url = depEntry.url;
        dep.tag = depEntry.tag;
        dep.branch = depEntry.branch;
        dep.hash = depEntry.hash;
        dep.type = (dep.url.find("http") == 0 || dep.url.find("git@") == 0) ? "git" : "local";

        std::string transDest = depsDir_ + "/" + dep.name;

        if (verbose) {
            std::cout << std::string(depth * 2, ' ') << "├── Fetching transitive: "
                      << dep.name << std::endl;
        }

        bool success = false;

        if (dep.type == "local") {
            // Resolve relative path from the parent dependency
            fs::path srcPath = fs::path(depPath) / dep.url;
            success = copyLocalDependency(srcPath.string(), transDest, verbose);
        } else {
            success = cloneGitRepo(dep.url, transDest, dep.tag, dep.branch, dep.hash, verbose);
        }

        if (!success) {
            std::cerr << "Failed to fetch transitive dependency: " << dep.name << std::endl;
            return false;
        }

        // Recursively fetch dependencies of this dependency
        if (!fetchDependenciesRecursive(transDest, visited, depth + 1, verbose)) {
            return false;
        }
    }

    return true;
}

// ============================================================================
// Public API
// ============================================================================

std::optional<Dependency> DependencyManager::parseGitHubURL(const std::string& url) {
    // Parse GitHub URL: https://github.com/user/repo or github.com/user/repo
    std::regex githubRegex(R"((?:https?://)?github\.com/([^/]+)/([^/@]+)(?:@(.+))?)");
    std::smatch match;

    if (std::regex_match(url, match, githubRegex)) {
        Dependency dep;
        dep.type = "git";
        dep.url = "https://github.com/" + match[1].str() + "/" + match[2].str();
        dep.name = match[2].str(); // Use repo name as package name

        std::string version = match[3].matched ? match[3].str() : "main";

        // Determine if it's a tag, branch, or hash
        std::regex hashRegex("^[0-9a-f]{7,40}$");
        if (std::regex_match(version, hashRegex)) {
            dep.hash = version;
        } else if (version[0] == 'v' || std::isdigit(version[0])) {
            dep.tag = version;
        } else {
            dep.branch = version;
        }

        return dep;
    }

    return std::nullopt;
}

bool DependencyManager::fetchDependency(const std::string& url, bool verbose) {
    // Try to parse as GitHub URL
    auto depOpt = parseGitHubURL(url);
    if (!depOpt) {
        // Try to parse as generic dependency spec
        depOpt = Dependency::parse("temp", url);
        if (!depOpt) {
            std::cerr << "Invalid dependency URL: " << url << std::endl;
            return false;
        }
    }

    Dependency dep = *depOpt;
    std::string destPath = depsDir_ + "/" + dep.name;

    if (verbose) {
        std::cout << "Fetching dependency: " << dep.name << std::endl;
        std::cout << "  Type: " << dep.type << std::endl;
        std::cout << "  URL: " << dep.url << std::endl;
        if (!dep.tag.empty()) {
            std::cout << "  Tag: " << dep.tag << std::endl;
        }
        if (!dep.branch.empty()) {
            std::cout << "  Branch: " << dep.branch << std::endl;
        }
        if (!dep.hash.empty()) {
            std::cout << "  Hash: " << dep.hash << std::endl;
        }
    }

    bool success = false;

    if (dep.type == "local") {
        // Resolve relative path
        fs::path srcPath = fs::absolute(projectRoot_) / dep.localPath;
        success = copyLocalDependency(srcPath.string(), destPath, verbose);
    } else {
        // Git-based dependency
        success = cloneGitRepo(dep.url, destPath, dep.tag, dep.branch, dep.hash, verbose);
    }

    if (!success) {
        std::cerr << "✗ Failed to fetch: " << dep.name << std::endl;
        return false;
    }

    // Fetch transitive dependencies
    std::set<std::string> visited;
    if (!fetchDependenciesRecursive(destPath, visited, 1, verbose)) {
        std::cerr << "✗ Failed to fetch transitive dependencies for: " << dep.name << std::endl;
        return false;
    }

    std::cout << "✓ Successfully fetched: " << dep.name << std::endl;

    return true;
}

bool DependencyManager::fetchAllDependencies(bool verbose) {
    // Look for stratos.conf
    std::string configPath = projectRoot_ + "/stratos.conf";
    if (!fs::exists(configPath)) {
        std::cerr << "No stratos.conf found in " << projectRoot_ << std::endl;
        return false;
    }

    // Parse config
    auto configOpt = ProjectConfigParser::parse(configPath);
    if (!configOpt) {
        std::cerr << "Failed to parse stratos.conf" << std::endl;
        return false;
    }

    const auto& deps = configOpt->dependencies;

    if (deps.empty()) {
        std::cout << "No dependencies specified in stratos.conf" << std::endl;
        return true;
    }

    std::cout << "Fetching " << deps.size() << " dependencies..." << std::endl;

    int successCount = 0;
    int failCount = 0;
    std::set<std::string> visited; // For circular dependency detection

    for (const auto& depEntry : deps) {
        if (verbose) {
            std::cout << "\n=== Dependency: " << depEntry.name << " ===" << std::endl;
        }

        Dependency dep;
        dep.name = depEntry.name;
        dep.url = depEntry.url;
        dep.tag = depEntry.tag;
        dep.branch = depEntry.branch;
        dep.hash = depEntry.hash;

        // Determine type
        if (dep.url.starts_with("/") || dep.url.starts_with("./") || dep.url.starts_with("../")) {
            dep.type = "local";
            dep.localPath = dep.url;
        } else {
            dep.type = "git";
        }

        std::string destPath = depsDir_ + "/" + dep.name;

        bool success = false;

        if (dep.type == "local") {
            // Resolve relative path
            fs::path srcPath = fs::absolute(projectRoot_) / dep.url;
            success = copyLocalDependency(srcPath.string(), destPath, verbose);
        } else {
            success = cloneGitRepo(dep.url, destPath, dep.tag, dep.branch, dep.hash, verbose);
        }

        if (success) {
            // Fetch transitive dependencies
            std::set<std::string> depVisited;
            if (fetchDependenciesRecursive(destPath, depVisited, 1, verbose)) {
                successCount++;
            } else {
                failCount++;
            }
        } else {
            failCount++;
        }
    }

    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Successfully fetched: " << successCount << std::endl;
    if (failCount > 0) {
        std::cout << "Failed: " << failCount << std::endl;
    }

    return failCount == 0;
}

bool DependencyManager::updateAllDependencies(bool verbose) {
    std::cout << "Updating all dependencies..." << std::endl;
    // Simply refetch all dependencies
    return fetchAllDependencies(verbose);
}

} // namespace stratos
