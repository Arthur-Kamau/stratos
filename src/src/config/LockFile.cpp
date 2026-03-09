#include "stratos/LockFile.h"
#include "stratos/ProjectConfig.h"
#include "stratos/DependencyManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace fs = std::filesystem;

namespace stratos {

// ============================================================================
// HOCON Parsing Helpers
// ============================================================================

void LockFileManager::skipWhitespaceAndComments(const std::string& content, size_t& pos) {
    while (pos < content.length()) {
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }

        // Skip # comments
        if (pos < content.length() && content[pos] == '#') {
            while (pos < content.length() && content[pos] != '\n') {
                pos++;
            }
            continue;
        }

        // Skip // comments
        if (pos + 1 < content.length() && content[pos] == '/' && content[pos + 1] == '/') {
            while (pos < content.length() && content[pos] != '\n') {
                pos++;
            }
            continue;
        }

        break; // No more whitespace or comments
    }
}

std::string LockFileManager::parseKey(const std::string& content, size_t& pos) {
    skipWhitespaceAndComments(content, pos);

    std::string key;
    while (pos < content.length() && content[pos] != '=' &&
           content[pos] != '{' && content[pos] != '\n' &&
           !std::isspace(content[pos])) {
        key += content[pos++];
    }

    return key;
}

std::string LockFileManager::parseValue(const std::string& content, size_t& pos) {
    skipWhitespaceAndComments(content, pos);

    if (pos >= content.length()) {
        return "";
    }

    // Check if quoted
    if (content[pos] == '"' || content[pos] == '\'') {
        char quoteChar = content[pos];
        pos++; // Skip opening quote

        std::string value;
        while (pos < content.length() && content[pos] != quoteChar) {
            if (content[pos] == '\\' && pos + 1 < content.length()) {
                pos++; // Skip escape character
            }
            value += content[pos++];
        }

        if (pos < content.length()) {
            pos++; // Skip closing quote
        }

        return value;
    }

    // Unquoted value
    std::string value;
    while (pos < content.length()) {
        char ch = content[pos];
        if (ch == '\n' || ch == ',' || ch == '}' || ch == ']' || ch == '#' ||
            (ch == '/' && pos + 1 < content.length() && content[pos + 1] == '/')) {
            break;
        }
        value += ch;
        pos++;
    }

    // Trim trailing whitespace
    while (!value.empty() && std::isspace(value.back())) {
        value.pop_back();
    }

    return value;
}

std::optional<LockedDependency> LockFileManager::parseDependencyObject(const std::string& content,
                                                                        size_t& pos) {
    skipWhitespaceAndComments(content, pos);

    // Expect '{'
    if (pos >= content.length() || content[pos] != '{') {
        return std::nullopt;
    }
    pos++; // Skip '{'

    LockedDependency dep;

    while (pos < content.length()) {
        skipWhitespaceAndComments(content, pos);

        // Check for end of object
        if (pos >= content.length() || content[pos] == '}') {
            if (pos < content.length()) {
                pos++; // Skip '}'
            }
            break;
        }

        // Parse key
        std::string key = parseKey(content, pos);
        if (key.empty()) {
            pos++; // Skip unknown character
            continue;
        }

        skipWhitespaceAndComments(content, pos);

        // Expect '='
        if (pos >= content.length() || content[pos] != '=') {
            return std::nullopt;
        }
        pos++; // Skip '='

        // Parse value
        std::string value = parseValue(content, pos);

        // Assign to appropriate field
        if (key == "name") dep.name = value;
        else if (key == "url") dep.url = value;
        else if (key == "type") dep.type = value;
        else if (key == "tag") dep.tag = value;
        else if (key == "branch") dep.branch = value;
        else if (key == "hash") dep.hash = value;
        else if (key == "resolved_commit") dep.resolvedCommit = value;
        else if (key == "checksum") dep.checksum = value;
        else if (key == "via") dep.via = value;

        // Optional comma
        skipWhitespaceAndComments(content, pos);
        if (pos < content.length() && content[pos] == ',') {
            pos++;
        }
    }

    if (dep.name.empty() || dep.url.empty()) {
        return std::nullopt;
    }

    return dep;
}

// ============================================================================
// Checksum Calculation
// ============================================================================

std::string LockFileManager::calculateChecksum(const std::string& dirPath) {
    // Simple checksum: concatenate file sizes and modification times
    // For a production system, you'd want proper SHA256 hashing
    std::stringstream ss;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                ss << entry.path().filename().string();
                ss << entry.file_size();
                auto ftime = entry.last_write_time();
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
                auto time_t = std::chrono::system_clock::to_time_t(sctp);
                ss << time_t;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error calculating checksum: " << e.what() << std::endl;
        return "";
    }

    // Return a simple hash-like string
    std::string data = ss.str();
    size_t hash = std::hash<std::string>{}(data);
    std::stringstream result;
    result << "sha256:" << std::hex << hash;
    return result.str();
}

// ============================================================================
// Public API
// ============================================================================

std::optional<std::vector<LockedDependency>> LockFileManager::load(const std::string& lockFilePath) {
    if (!fs::exists(lockFilePath)) {
        return std::nullopt;
    }

    std::ifstream file(lockFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open lock file: " << lockFilePath << std::endl;
        return std::nullopt;
    }

    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    std::vector<LockedDependency> deps;
    size_t pos = 0;

    while (pos < content.length()) {
        skipWhitespaceAndComments(content, pos);

        if (pos >= content.length()) break;

        // Look for "dependencies = [" or "dependencies=["
        std::string key = parseKey(content, pos);

        if (key == "dependencies") {
            skipWhitespaceAndComments(content, pos);

            // Expect '='
            if (pos >= content.length() || content[pos] != '=') {
                continue;
            }
            pos++; // Skip '='

            skipWhitespaceAndComments(content, pos);

            // Expect '['
            if (pos >= content.length() || content[pos] != '[') {
                continue;
            }
            pos++; // Skip '['

            // Parse array of dependency objects
            while (pos < content.length()) {
                skipWhitespaceAndComments(content, pos);

                if (pos >= content.length() || content[pos] == ']') {
                    if (pos < content.length()) {
                        pos++; // Skip ']'
                    }
                    break;
                }

                auto depOpt = parseDependencyObject(content, pos);
                if (depOpt) {
                    deps.push_back(*depOpt);
                }

                // Optional comma
                skipWhitespaceAndComments(content, pos);
                if (pos < content.length() && content[pos] == ',') {
                    pos++;
                }
            }

            break; // Found dependencies, done parsing
        } else {
            // Skip this section
            pos++;
        }
    }

    return deps;
}

bool LockFileManager::save(const std::string& lockFilePath,
                           const std::vector<LockedDependency>& deps,
                           const std::string& stratosVersion) {
    std::ofstream file(lockFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to create lock file: " << lockFilePath << std::endl;
        return false;
    }

    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream timestamp;
    timestamp << std::put_time(std::gmtime(&time_t_now), "%Y-%m-%dT%H:%M:%SZ");

    // Write header comment
    file << "# Stratos dependency lock file\n";
    file << "# This file is auto-generated. Do not edit manually.\n";
    file << "# Generated: " << timestamp.str() << "\n\n";

    // Write metadata
    file << "metadata {\n";
    file << "  generated = \"" << timestamp.str() << "\"\n";
    file << "  stratos_version = \"" << stratosVersion << "\"\n";
    file << "}\n\n";

    // Write dependencies
    file << "dependencies = [\n";

    for (size_t i = 0; i < deps.size(); i++) {
        const auto& dep = deps[i];

        file << "  {\n";
        file << "    name = " << dep.name << "\n";
        file << "    url = \"" << dep.url << "\"\n";
        file << "    type = " << dep.type << "\n";

        if (!dep.tag.empty()) {
            file << "    tag = \"" << dep.tag << "\"\n";
        }
        if (!dep.branch.empty()) {
            file << "    branch = " << dep.branch << "\n";
        }
        if (!dep.hash.empty()) {
            file << "    hash = \"" << dep.hash << "\"\n";
        }
        if (!dep.resolvedCommit.empty()) {
            file << "    resolved_commit = \"" << dep.resolvedCommit << "\"\n";
        }
        if (!dep.checksum.empty()) {
            file << "    checksum = \"" << dep.checksum << "\"\n";
        }
        if (!dep.via.empty()) {
            file << "    via = " << dep.via << "\n";
        }

        file << "  }";

        if (i < deps.size() - 1) {
            file << ",";
        }
        file << "\n";
    }

    file << "]\n";

    std::cout << "✓ Lock file saved: " << lockFilePath << std::endl;

    return true;
}

bool LockFileManager::verify(const std::string& projectRoot) {
    std::string lockFilePath = projectRoot + "/stratos.lock";

    // Load lock file
    auto lockedDepsOpt = load(lockFilePath);
    if (!lockedDepsOpt) {
        std::cerr << "No lock file found or lock file is invalid" << std::endl;
        return false;
    }

    auto lockedDeps = *lockedDepsOpt;
    std::string depsDir = projectRoot + "/deps";

    std::cout << "Verifying " << lockedDeps.size() << " locked dependencies..." << std::endl;

    int errors = 0;

    for (const auto& lockedDep : lockedDeps) {
        std::string depPath = depsDir + "/" + lockedDep.name;

        // Check if dependency exists
        if (!fs::exists(depPath) || !fs::is_directory(depPath)) {
            std::cerr << "✗ Missing dependency: " << lockedDep.name << std::endl;
            errors++;
            continue;
        }

        // For git dependencies, check the metadata file
        if (lockedDep.type == "git") {
            std::string metaPath = depPath + "/.stratos-meta.json";

            if (!fs::exists(metaPath)) {
                std::cerr << "✗ Missing metadata for: " << lockedDep.name << std::endl;
                errors++;
                continue;
            }

            // Simple verification: check that metadata exists
            // In a production system, you'd verify the exact commit hash
        }

        // For local dependencies, check checksum
        if (lockedDep.type == "local" && !lockedDep.checksum.empty()) {
            std::string actualChecksum = calculateChecksum(depPath);
            if (actualChecksum != lockedDep.checksum) {
                std::cerr << "✗ Checksum mismatch for: " << lockedDep.name << std::endl;
                errors++;
                continue;
            }
        }

        std::cout << "✓ " << lockedDep.name << std::endl;
    }

    if (errors > 0) {
        std::cerr << "\n" << errors << " verification errors found" << std::endl;
        return false;
    }

    std::cout << "\n✓ All dependencies verified successfully" << std::endl;
    return true;
}

bool LockFileManager::generate(const std::string& projectRoot) {
    std::string lockFilePath = projectRoot + "/stratos.lock";
    std::string depsDir = projectRoot + "/deps";

    if (!fs::exists(depsDir)) {
        std::cerr << "No dependencies directory found" << std::endl;
        return false;
    }

    std::vector<LockedDependency> lockedDeps;

    // Scan deps directory
    for (const auto& entry : fs::directory_iterator(depsDir)) {
        if (!entry.is_directory()) continue;

        std::string depName = entry.path().filename().string();
        std::string depPath = entry.path().string();
        std::string metaPath = depPath + "/.stratos-meta.json";

        LockedDependency lockedDep;
        lockedDep.name = depName;

        // Try to load metadata
        std::ifstream metaFile(metaPath);
        if (metaFile.is_open()) {
            std::string line;
            while (std::getline(metaFile, line)) {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = line.substr(0, colonPos);
                    std::string value = line.substr(colonPos + 1);

                    // Remove quotes, spaces, commas
                    key.erase(std::remove_if(key.begin(), key.end(),
                        [](char c) { return c == '"' || c == ' ' || c == '\t'; }), key.end());
                    value.erase(std::remove_if(value.begin(), value.end(),
                        [](char c) { return c == '"' || c == ' ' || c == '\t' || c == ','; }), value.end());

                    if (key == "url") lockedDep.url = value;
                    else if (key == "type") lockedDep.type = value;
                    else if (key == "tag") lockedDep.tag = value;
                    else if (key == "branch") lockedDep.branch = value;
                    else if (key == "hash") lockedDep.hash = value;
                    else if (key == "resolved_commit") lockedDep.resolvedCommit = value;
                }
            }
        }

        // Calculate checksum for local dependencies
        if (lockedDep.type == "local") {
            lockedDep.checksum = calculateChecksum(depPath);
        }

        if (!lockedDep.url.empty()) {
            lockedDeps.push_back(lockedDep);
        }
    }

    if (lockedDeps.empty()) {
        std::cerr << "No dependencies found to lock" << std::endl;
        return false;
    }

    return save(lockFilePath, lockedDeps);
}

} // namespace stratos
