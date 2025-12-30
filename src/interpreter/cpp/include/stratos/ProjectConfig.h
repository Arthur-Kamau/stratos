#ifndef STRATOS_PROJECT_CONFIG_H
#define STRATOS_PROJECT_CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace stratos {

struct DependencyEntry {
    std::string name;
    std::string url;
    std::string tag;      // optional - git tag
    std::string branch;   // optional - git branch
    std::string hash;     // optional - git commit hash
};

struct ProjectConfig {
    // project section
    std::string name;
    std::string version;
    std::string author;
    std::string type; // "executable" or "library"
    std::string description;

    // build section
    std::string entry;      // Entry point file (for executables)
    std::string source_dir; // Source directory
    std::string output;     // Output file path

    // dependencies array
    std::vector<DependencyEntry> dependencies;

    // compile section
    std::vector<std::string> sources; // Explicit source file list

    // exports section
    std::map<std::string, std::string> exports; // Public API exports

    // Helper methods
    bool isLibrary() const { return type == "library"; }
    bool isExecutable() const { return type == "executable" || type.empty(); }
};

class ProjectConfigParser {
public:
    // Parse a stratos.conf file
    static std::optional<ProjectConfig> parse(const std::string& configPath);

    // Get the directory containing the config file
    static std::string getProjectRoot(const std::string& configPath);

private:
    static void trim(std::string& s);
    static std::pair<std::string, std::string> parseLine(const std::string& line);
};

} // namespace stratos

#endif // STRATOS_PROJECT_CONFIG_H
