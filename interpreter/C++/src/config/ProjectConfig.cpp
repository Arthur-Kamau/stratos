#include "stratos/ProjectConfig.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace stratos {

void ProjectConfigParser::trim(std::string& s) {
    // Trim leading whitespace
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    // Trim trailing whitespace
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::pair<std::string, std::string> ProjectConfigParser::parseLine(const std::string& line) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return {"", ""};
    }

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    trim(key);
    trim(value);

    return {key, value};
}

std::string ProjectConfigParser::getProjectRoot(const std::string& configPath) {
    std::filesystem::path p(configPath);
    return p.parent_path().string();
}

std::optional<ProjectConfig> ProjectConfigParser::parse(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << configPath << std::endl;
        return std::nullopt;
    }

    ProjectConfig config;
    config.type = "executable"; // Default

    std::string line;
    std::string currentSection;

    while (std::getline(file, line)) {
        trim(line);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Check for section headers
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }

        // Parse key-value pairs
        auto [key, value] = parseLine(line);
        if (key.empty()) continue;

        // Handle different sections
        if (currentSection == "project") {
            if (key == "name") config.name = value;
            else if (key == "version") config.version = value;
            else if (key == "author") config.author = value;
            else if (key == "type") config.type = value;
        }
        else if (currentSection == "build") {
            if (key == "entry") config.entry = value;
            else if (key == "source_dir") config.source_dir = value;
            else if (key == "output") config.output = value;
        }
        else if (currentSection == "dependencies") {
            config.dependencies[key] = value;
        }
        else if (currentSection == "compile") {
            if (key == "sources") {
                // Parse comma-separated list
                std::stringstream ss(value);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    trim(item);
                    if (!item.empty()) {
                        config.sources.push_back(item);
                    }
                }
            }
        }
        else if (currentSection == "exports") {
            config.exports[key] = value;
        }
    }

    return config;
}

} // namespace stratos
