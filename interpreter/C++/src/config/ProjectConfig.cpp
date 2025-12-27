#include "stratos/ProjectConfig.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <stack>

namespace stratos {

// ============================================================================
// HOCON Parser Implementation
// ============================================================================

class HOCONParser {
private:
    std::string content;
    size_t pos;

    void skipWhitespace() {
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
    }

    void skipComments() {
        skipWhitespace();
        // Handle # comments
        if (pos < content.length() && content[pos] == '#') {
            while (pos < content.length() && content[pos] != '\n') {
                pos++;
            }
            skipWhitespace();
        }
        // Handle // comments
        if (pos + 1 < content.length() && content[pos] == '/' && content[pos + 1] == '/') {
            while (pos < content.length() && content[pos] != '\n') {
                pos++;
            }
            skipWhitespace();
        }
    }

    std::string parseString() {
        skipComments();

        if (pos >= content.length()) {
            return "";
        }

        // Check if quoted
        bool isQuoted = (content[pos] == '"' || content[pos] == '\'');
        char quoteChar = content[pos];

        if (isQuoted) {
            pos++; // Skip opening quote
            std::string result;
            while (pos < content.length() && content[pos] != quoteChar) {
                if (content[pos] == '\\' && pos + 1 < content.length()) {
                    pos++; // Skip escape character
                }
                result += content[pos++];
            }
            if (pos < content.length()) {
                pos++; // Skip closing quote
            }
            return result;
        }

        // Unquoted string - read until delimiter
        std::string result;
        while (pos < content.length()) {
            char ch = content[pos];
            if (ch == '\n' || ch == ',' || ch == '}' || ch == ']' || ch == '#' ||
                (ch == '/' && pos + 1 < content.length() && content[pos + 1] == '/')) {
                break;
            }
            result += ch;
            pos++;
        }

        // Trim trailing whitespace
        while (!result.empty() && std::isspace(result.back())) {
            result.pop_back();
        }

        return result;
    }

    std::string parseKey() {
        skipComments();

        std::string key;
        while (pos < content.length() && content[pos] != '=' &&
               content[pos] != '{' && content[pos] != '\n') {
            if (!std::isspace(content[pos])) {
                key += content[pos];
            }
            pos++;
        }

        return key;
    }

    bool expect(char ch) {
        skipComments();
        if (pos < content.length() && content[pos] == ch) {
            pos++;
            return true;
        }
        return false;
    }

    bool peek(char ch) {
        skipComments();
        return pos < content.length() && content[pos] == ch;
    }

    std::map<std::string, std::string> parseObject() {
        std::map<std::string, std::string> obj;

        expect('{');

        while (!peek('}')) {
            skipComments();

            if (peek('}')) break;

            std::string key = parseKey();
            if (key.empty()) break;

            expect('=');
            std::string value = parseString();

            obj[key] = value;

            // Optional comma
            expect(',');
        }

        expect('}');

        return obj;
    }

    std::vector<std::map<std::string, std::string>> parseArray() {
        std::vector<std::map<std::string, std::string>> arr;

        expect('[');

        while (!peek(']')) {
            skipComments();

            if (peek(']')) break;

            if (peek('{')) {
                arr.push_back(parseObject());
            }

            // Optional comma
            expect(',');
        }

        expect(']');

        return arr;
    }

public:
    HOCONParser(const std::string& content) : content(content), pos(0) {}

    ProjectConfig parse() {
        ProjectConfig config;
        config.type = "executable"; // Default

        while (pos < content.length()) {
            skipComments();

            if (pos >= content.length()) break;

            std::string key = parseKey();
            if (key.empty()) {
                pos++;
                continue;
            }

            // Check for object definition (e.g., "project {")
            if (peek('{')) {
                auto obj = parseObject();

                if (key == "project") {
                    if (obj.count("name")) config.name = obj["name"];
                    if (obj.count("version")) config.version = obj["version"];
                    if (obj.count("author")) config.author = obj["author"];
                    if (obj.count("type")) config.type = obj["type"];
                    if (obj.count("description")) config.description = obj["description"];
                }
                else if (key == "build") {
                    if (obj.count("entry")) config.entry = obj["entry"];
                    if (obj.count("source_dir")) config.source_dir = obj["source_dir"];
                    if (obj.count("output")) config.output = obj["output"];
                }
                else if (key == "exports") {
                    config.exports = obj;
                }
            }
            // Check for array definition (e.g., "dependencies = [")
            else if (expect('=')) {
                skipComments();

                if (peek('[')) {
                    // Parse array
                    auto arr = parseArray();

                    if (key == "dependencies") {
                        for (const auto& obj : arr) {
                            DependencyEntry dep;
                            if (obj.count("name")) dep.name = obj.at("name");
                            if (obj.count("url")) dep.url = obj.at("url");
                            if (obj.count("tag")) dep.tag = obj.at("tag");
                            if (obj.count("branch")) dep.branch = obj.at("branch");
                            if (obj.count("hash")) dep.hash = obj.at("hash");
                            config.dependencies.push_back(dep);
                        }
                    }
                }
                else {
                    // Parse simple value
                    std::string value = parseString();

                    // Handle compile.sources as comma-separated list
                    if (key == "sources") {
                        std::stringstream ss(value);
                        std::string item;
                        while (std::getline(ss, item, ',')) {
                            // Trim whitespace
                            item.erase(0, item.find_first_not_of(" \t"));
                            item.erase(item.find_last_not_of(" \t") + 1);
                            if (!item.empty()) {
                                config.sources.push_back(item);
                            }
                        }
                    }
                }

                // Optional comma or newline
                expect(',');
            }
        }

        return config;
    }
};

// ============================================================================
// ProjectConfigParser Implementation
// ============================================================================

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

    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Check if it's HOCON format (contains {)
    // Note: Don't check for [ because INI section headers use [section] syntax
    // HOCON uses { } for objects and doesn't have [section] headers
    bool isHOCON = (content.find('{') != std::string::npos);

    if (isHOCON) {
        // Use HOCON parser
        try {
            HOCONParser parser(content);
            return parser.parse();
        } catch (const std::exception& e) {
            std::cerr << "HOCON parse error: " << e.what() << std::endl;
            return std::nullopt;
        }
    }

    // Fallback to INI-style parser for backward compatibility
    ProjectConfig config;
    config.type = "executable"; // Default

    file.clear();
    file.seekg(0);
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
            else if (key == "description") config.description = value;
        }
        else if (currentSection == "build") {
            if (key == "entry") config.entry = value;
            else if (key == "source_dir") config.source_dir = value;
            else if (key == "output") config.output = value;
        }
        else if (currentSection == "dependencies") {
            // Old INI format: key = value (URL)
            // Convert to new format
            DependencyEntry dep;
            dep.name = key;
            dep.url = value;
            config.dependencies.push_back(dep);
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
