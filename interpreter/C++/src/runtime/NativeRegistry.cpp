#include "stratos/NativeRegistry.h"
#include <cmath>
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cstring>

namespace stratos {

NativeRegistry& NativeRegistry::getInstance() {
    static NativeRegistry instance;
    return instance;
}

void NativeRegistry::registerFunction(const std::string& moduleName, const std::string& functionName, NativeFunction func) {
    std::string qualifiedName = getQualifiedName(moduleName, functionName);
    functions_[qualifiedName] = func;
}

bool NativeRegistry::isNative(const std::string& moduleName, const std::string& functionName) const {
    std::string qualifiedName = getQualifiedName(moduleName, functionName);
    return functions_.find(qualifiedName) != functions_.end();
}

NativeFunction NativeRegistry::getFunction(const std::string& moduleName, const std::string& functionName) const {
    std::string qualifiedName = getQualifiedName(moduleName, functionName);
    auto it = functions_.find(qualifiedName);
    if (it != functions_.end()) {
        return it->second;
    }
    throw std::runtime_error("Native function not found: " + qualifiedName);
}

std::string NativeRegistry::getQualifiedName(const std::string& moduleName, const std::string& functionName) const {
    return moduleName + "::" + functionName;
}

void NativeRegistry::initializeStdlib() {
    initMath();
    initStrings();
    initIO();
    initLog();
    initTime();
    // Collections, JSON, etc. will be initialized separately
}

// ============================================================================
// Math Module Native Functions
// ============================================================================

void NativeRegistry::initMath() {
    // Trigonometric functions
    registerFunction("math", "sin", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::sin(x);
    });

    registerFunction("math", "cos", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::cos(x);
    });

    registerFunction("math", "tan", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::tan(x);
    });

    registerFunction("math", "asin", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::asin(x);
    });

    registerFunction("math", "acos", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::acos(x);
    });

    registerFunction("math", "atan", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::atan(x);
    });

    registerFunction("math", "atan2", [](const std::vector<std::any>& args) -> std::any {
        double y = std::any_cast<double>(args[0]);
        double x = std::any_cast<double>(args[1]);
        return std::atan2(y, x);
    });

    // Hyperbolic functions
    registerFunction("math", "sinh", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::sinh(x);
    });

    registerFunction("math", "cosh", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::cosh(x);
    });

    registerFunction("math", "tanh", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::tanh(x);
    });

    // Exponential and logarithmic
    registerFunction("math", "exp", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::exp(x);
    });

    registerFunction("math", "log", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::log(x);
    });

    registerFunction("math", "log10", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::log10(x);
    });

    registerFunction("math", "log2", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::log2(x);
    });

    registerFunction("math", "pow", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        double y = std::any_cast<double>(args[1]);
        return std::pow(x, y);
    });

    registerFunction("math", "sqrt", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::sqrt(x);
    });

    registerFunction("math", "cbrt", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::cbrt(x);
    });

    // Rounding
    registerFunction("math", "ceil", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::ceil(x);
    });

    registerFunction("math", "floor", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::floor(x);
    });

    registerFunction("math", "round", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::round(x);
    });

    registerFunction("math", "trunc", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::trunc(x);
    });

    // Absolute and sign
    registerFunction("math", "abs", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::abs(x);
    });

    registerFunction("math", "sign", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return (x > 0) ? 1 : (x < 0) ? -1 : 0;
    });

    // Min/Max
    registerFunction("math", "min", [](const std::vector<std::any>& args) -> std::any {
        double a = std::any_cast<double>(args[0]);
        double b = std::any_cast<double>(args[1]);
        return std::min(a, b);
    });

    registerFunction("math", "max", [](const std::vector<std::any>& args) -> std::any {
        double a = std::any_cast<double>(args[0]);
        double b = std::any_cast<double>(args[1]);
        return std::max(a, b);
    });

    // Random
    registerFunction("math", "random", [](const std::vector<std::any>& args) -> std::any {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(0.0, 1.0);
        return dis(gen);
    });

    registerFunction("math", "randomInt", [](const std::vector<std::any>& args) -> std::any {
        int min = std::any_cast<int>(args[0]);
        int max = std::any_cast<int>(args[1]);
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    });

    registerFunction("math", "randomSeed", [](const std::vector<std::any>& args) -> std::any {
        int seed = std::any_cast<int>(args[0]);
        // This would set the seed for the random generator
        // For now, just return void
        return std::any();
    });
}

// ============================================================================
// Strings Module Native Functions
// ============================================================================

void NativeRegistry::initStrings() {
    // Case conversion
    registerFunction("strings", "toUpper", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        return s;
    });

    registerFunction("strings", "toLower", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    });

    registerFunction("strings", "toTitle", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        bool capitalizeNext = true;
        for (char& c : s) {
            if (std::isspace(c)) {
                capitalizeNext = true;
            } else if (capitalizeNext) {
                c = std::toupper(c);
                capitalizeNext = false;
            } else {
                c = std::tolower(c);
            }
        }
        return s;
    });

    // Trimming
    registerFunction("strings", "trim", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        size_t start = s.find_first_not_of(" \t\n\r");
        size_t end = s.find_last_not_of(" \t\n\r");
        if (start == std::string::npos) return std::string("");
        return s.substr(start, end - start + 1);
    });

    registerFunction("strings", "trimLeft", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        size_t start = s.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return std::string("");
        return s.substr(start);
    });

    registerFunction("strings", "trimRight", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        size_t end = s.find_last_not_of(" \t\n\r");
        if (end == std::string::npos) return std::string("");
        return s.substr(0, end + 1);
    });

    registerFunction("strings", "trimPrefix", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::string prefix = std::any_cast<std::string>(args[1]);
        if (s.substr(0, prefix.length()) == prefix) {
            return s.substr(prefix.length());
        }
        return s;
    });

    registerFunction("strings", "trimSuffix", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::string suffix = std::any_cast<std::string>(args[1]);
        if (s.length() >= suffix.length() &&
            s.substr(s.length() - suffix.length()) == suffix) {
            return s.substr(0, s.length() - suffix.length());
        }
        return s;
    });

    // Splitting and joining
    registerFunction("strings", "split", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::string sep = std::any_cast<std::string>(args[1]);
        std::vector<std::string> result;
        size_t pos = 0;
        while (pos < s.length()) {
            size_t found = s.find(sep, pos);
            if (found == std::string::npos) {
                result.push_back(s.substr(pos));
                break;
            }
            result.push_back(s.substr(pos, found - pos));
            pos = found + sep.length();
        }
        return result;
    });

    registerFunction("strings", "join", [](const std::vector<std::any>& args) -> std::any {
        auto parts = std::any_cast<std::vector<std::string>>(args[0]);
        std::string sep = std::any_cast<std::string>(args[1]);
        std::string result;
        for (size_t i = 0; i < parts.size(); i++) {
            result += parts[i];
            if (i < parts.size() - 1) result += sep;
        }
        return result;
    });

    // Searching
    registerFunction("strings", "contains", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::string substr = std::any_cast<std::string>(args[1]);
        return s.find(substr) != std::string::npos;
    });

    registerFunction("strings", "indexOf", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::string substr = std::any_cast<std::string>(args[1]);
        size_t pos = s.find(substr);
        return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
    });

    registerFunction("strings", "lastIndexOf", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::string substr = std::any_cast<std::string>(args[1]);
        size_t pos = s.rfind(substr);
        return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
    });

    registerFunction("strings", "startsWith", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::string prefix = std::any_cast<std::string>(args[1]);
        return s.substr(0, prefix.length()) == prefix;
    });

    registerFunction("strings", "endsWith", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::string suffix = std::any_cast<std::string>(args[1]);
        return s.length() >= suffix.length() &&
               s.substr(s.length() - suffix.length()) == suffix;
    });

    registerFunction("strings", "count", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::string substr = std::any_cast<std::string>(args[1]);
        int count = 0;
        size_t pos = 0;
        while ((pos = s.find(substr, pos)) != std::string::npos) {
            count++;
            pos += substr.length();
        }
        return count;
    });

    // Replacement
    registerFunction("strings", "replace", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::string old = std::any_cast<std::string>(args[1]);
        std::string newStr = std::any_cast<std::string>(args[2]);
        size_t pos = s.find(old);
        if (pos != std::string::npos) {
            s.replace(pos, old.length(), newStr);
        }
        return s;
    });

    registerFunction("strings", "replaceAll", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::string old = std::any_cast<std::string>(args[1]);
        std::string newStr = std::any_cast<std::string>(args[2]);
        size_t pos = 0;
        while ((pos = s.find(old, pos)) != std::string::npos) {
            s.replace(pos, old.length(), newStr);
            pos += newStr.length();
        }
        return s;
    });

    // Extraction
    registerFunction("strings", "substring", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        int start = std::any_cast<int>(args[1]);
        int end = std::any_cast<int>(args[2]);
        return s.substr(start, end - start);
    });

    registerFunction("strings", "charAt", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        int index = std::any_cast<int>(args[1]);
        return std::string(1, s[index]);
    });

    // Repetition and padding
    registerFunction("strings", "repeat", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        int count = std::any_cast<int>(args[1]);
        std::string result;
        for (int i = 0; i < count; i++) {
            result += s;
        }
        return result;
    });

    registerFunction("strings", "padLeft", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        int length = std::any_cast<int>(args[1]);
        std::string pad = std::any_cast<std::string>(args[2]);
        while (s.length() < static_cast<size_t>(length)) {
            s = pad + s;
        }
        return s;
    });

    registerFunction("strings", "padRight", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        int length = std::any_cast<int>(args[1]);
        std::string pad = std::any_cast<std::string>(args[2]);
        while (s.length() < static_cast<size_t>(length)) {
            s = s + pad;
        }
        return s;
    });

    // Comparison
    registerFunction("strings", "compare", [](const std::vector<std::any>& args) -> std::any {
        std::string a = std::any_cast<std::string>(args[0]);
        std::string b = std::any_cast<std::string>(args[1]);
        return a.compare(b);
    });

    registerFunction("strings", "equals", [](const std::vector<std::any>& args) -> std::any {
        std::string a = std::any_cast<std::string>(args[0]);
        std::string b = std::any_cast<std::string>(args[1]);
        return a == b;
    });

    registerFunction("strings", "equalsIgnoreCase", [](const std::vector<std::any>& args) -> std::any {
        std::string a = std::any_cast<std::string>(args[0]);
        std::string b = std::any_cast<std::string>(args[1]);
        std::transform(a.begin(), a.end(), a.begin(), ::tolower);
        std::transform(b.begin(), b.end(), b.begin(), ::tolower);
        return a == b;
    });

    // Utility
    registerFunction("strings", "length", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        return static_cast<int>(s.length());
    });

    registerFunction("strings", "isEmpty", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        return s.empty();
    });

    registerFunction("strings", "isBlank", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        return s.find_first_not_of(" \t\n\r") == std::string::npos;
    });

    registerFunction("strings", "reverse", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::reverse(s.begin(), s.end());
        return s;
    });
}

// ============================================================================
// IO Module Native Functions
// ============================================================================

void NativeRegistry::initIO() {
    namespace fs = std::filesystem;

    // File reading
    registerFunction("io", "readFile", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        std::ifstream file(path);
        if (!file.is_open()) {
            // Return error result
            return std::string(""); // Simplified - should return Result type
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    });

    registerFunction("io", "readBytes", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return std::vector<uint8_t>();
        }
        std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
        return bytes;
    });

    // File writing
    registerFunction("io", "writeFile", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        std::string content = std::any_cast<std::string>(args[1]);
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        return true;
    });

    registerFunction("io", "appendFile", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        std::string content = std::any_cast<std::string>(args[1]);
        std::ofstream file(path, std::ios::app);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        return true;
    });

    registerFunction("io", "writeBytes", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        auto bytes = std::any_cast<std::vector<uint8_t>>(args[1]);
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        return true;
    });

    // File operations
    registerFunction("io", "remove", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::remove(path);
    });

    registerFunction("io", "rename", [](const std::vector<std::any>& args) -> std::any {
        std::string oldPath = std::any_cast<std::string>(args[0]);
        std::string newPath = std::any_cast<std::string>(args[1]);
        try {
            fs::rename(oldPath, newPath);
            return true;
        } catch (...) {
            return false;
        }
    });

    registerFunction("io", "copy", [](const std::vector<std::any>& args) -> std::any {
        std::string src = std::any_cast<std::string>(args[0]);
        std::string dst = std::any_cast<std::string>(args[1]);
        try {
            fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
            return true;
        } catch (...) {
            return false;
        }
    });

    // Directory operations
    registerFunction("io", "mkdir", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::create_directory(path);
    });

    registerFunction("io", "mkdirAll", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::create_directories(path);
    });

    registerFunction("io", "removeDir", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::remove(path);
    });

    registerFunction("io", "removeDirAll", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::remove_all(path) > 0;
    });

    // File info
    registerFunction("io", "exists", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::exists(path);
    });

    registerFunction("io", "isFile", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::is_regular_file(path);
    });

    registerFunction("io", "isDirectory", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::is_directory(path);
    });

    registerFunction("io", "fileSize", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return static_cast<int>(fs::file_size(path));
    });

    // Path operations
    registerFunction("io", "join", [](const std::vector<std::any>& args) -> std::any {
        auto parts = std::any_cast<std::vector<std::string>>(args[0]);
        fs::path result;
        for (const auto& part : parts) {
            result /= part;
        }
        return result.string();
    });

    registerFunction("io", "basename", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::path(path).filename().string();
    });

    registerFunction("io", "dirname", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::path(path).parent_path().string();
    });

    registerFunction("io", "extension", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::path(path).extension().string();
    });

    registerFunction("io", "absolute", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        return fs::absolute(path).string();
    });
}

// ============================================================================
// Log Module Native Functions
// ============================================================================

void NativeRegistry::initLog() {
    registerFunction("log", "debug", [](const std::vector<std::any>& args) -> std::any {
        std::string message = std::any_cast<std::string>(args[0]);
        std::cout << "[DEBUG] " << message << std::endl;
        return std::any();
    });

    registerFunction("log", "info", [](const std::vector<std::any>& args) -> std::any {
        std::string message = std::any_cast<std::string>(args[0]);
        std::cout << "[INFO] " << message << std::endl;
        return std::any();
    });

    registerFunction("log", "warn", [](const std::vector<std::any>& args) -> std::any {
        std::string message = std::any_cast<std::string>(args[0]);
        std::cout << "[WARN] " << message << std::endl;
        return std::any();
    });

    registerFunction("log", "error", [](const std::vector<std::any>& args) -> std::any {
        std::string message = std::any_cast<std::string>(args[0]);
        std::cerr << "[ERROR] " << message << std::endl;
        return std::any();
    });

    registerFunction("log", "fatal", [](const std::vector<std::any>& args) -> std::any {
        std::string message = std::any_cast<std::string>(args[0]);
        std::cerr << "[FATAL] " << message << std::endl;
        std::exit(1);
        return std::any();
    });
}

// ============================================================================
// Time Module Native Functions
// ============================================================================

void NativeRegistry::initTime() {
    using namespace std::chrono;

    registerFunction("time", "now", [](const std::vector<std::any>& args) -> std::any {
        auto now = system_clock::now();
        auto millis = duration_cast<milliseconds>(now.time_since_epoch()).count();
        return static_cast<int64_t>(millis);
    });

    registerFunction("time", "unix", [](const std::vector<std::any>& args) -> std::any {
        int64_t seconds = std::any_cast<int64_t>(args[0]);
        return seconds * 1000; // Convert to milliseconds
    });

    registerFunction("time", "fromMillis", [](const std::vector<std::any>& args) -> std::any {
        int64_t millis = std::any_cast<int64_t>(args[0]);
        return millis;
    });

    registerFunction("time", "sleep", [](const std::vector<std::any>& args) -> std::any {
        int64_t millis = std::any_cast<int64_t>(args[0]);
        std::this_thread::sleep_for(milliseconds(millis));
        return std::any();
    });
}

} // namespace stratos
