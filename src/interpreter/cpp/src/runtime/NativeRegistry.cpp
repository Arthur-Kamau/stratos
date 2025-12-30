#include "stratos/NativeRegistry.h"
#include "stratos/Logger.h"
#include "stratos/FFI.h"
#include "stratos/WebSocket.h"
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
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

void NativeRegistry::registerFunction(const std::string& moduleName, const std::string& functionName,
                                     NativeFunction func, const FunctionSignature& signature) {
    std::string qualifiedName = getQualifiedName(moduleName, functionName);
    functions_[qualifiedName] = func;
    signatures_[qualifiedName] = signature;
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

FunctionSignature NativeRegistry::getSignature(const std::string& moduleName, const std::string& functionName) const {
    std::string qualifiedName = getQualifiedName(moduleName, functionName);
    auto it = signatures_.find(qualifiedName);
    if (it != signatures_.end()) {
        return it->second;
    }
    // Return empty signature if not found
    return FunctionSignature{{}, "void"};
}

bool NativeRegistry::hasSignature(const std::string& moduleName, const std::string& functionName) const {
    std::string qualifiedName = getQualifiedName(moduleName, functionName);
    return signatures_.find(qualifiedName) != signatures_.end();
}

std::string NativeRegistry::getQualifiedName(const std::string& moduleName, const std::string& functionName) const {
    return moduleName + "::" + functionName;
}

void NativeRegistry::initializeStdlib() {
    initPrelude();   // Auto-imported functions (print, println, printf)
    initMath();
    initStrings();
    initIO();
    initLog();
    initTime();
    initJSON();
    initBase64();
    initCSV();
    initCrypto();
    initZip();
    initFFI();
    initWebSocket();
}

// ============================================================================
// Prelude Module - Auto-imported Functions
// ============================================================================

void NativeRegistry::initPrelude() {
    // print - prints value without newline
    registerFunction("prelude", "print", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::any();

        const auto& arg = args[0];

        // Try different types
        try {
            if (arg.type() == typeid(int)) {
                std::cout << std::any_cast<int>(arg);
            } else if (arg.type() == typeid(double)) {
                std::cout << std::any_cast<double>(arg);
            } else if (arg.type() == typeid(std::string)) {
                std::cout << std::any_cast<std::string>(arg);
            } else if (arg.type() == typeid(bool)) {
                std::cout << (std::any_cast<bool>(arg) ? "true" : "false");
            } else if (arg.type() == typeid(char)) {
                std::cout << std::any_cast<char>(arg);
            } else {
                std::cout << "[unknown type]";
            }
        } catch (...) {
            std::cout << "[error printing value]";
        }

        std::cout.flush();
        return std::any();
    }, FunctionSignature{{"any"}, "void"});

    // println - prints value with newline
    registerFunction("prelude", "println", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            std::cout << std::endl;
            return std::any();
        }

        const auto& arg = args[0];

        // Try different types
        try {
            if (arg.type() == typeid(int)) {
                std::cout << std::any_cast<int>(arg) << std::endl;
            } else if (arg.type() == typeid(double)) {
                std::cout << std::any_cast<double>(arg) << std::endl;
            } else if (arg.type() == typeid(std::string)) {
                std::cout << std::any_cast<std::string>(arg) << std::endl;
            } else if (arg.type() == typeid(bool)) {
                std::cout << (std::any_cast<bool>(arg) ? "true" : "false") << std::endl;
            } else if (arg.type() == typeid(char)) {
                std::cout << std::any_cast<char>(arg) << std::endl;
            } else {
                std::cout << "[unknown type]" << std::endl;
            }
        } catch (...) {
            std::cout << "[error printing value]" << std::endl;
        }

        return std::any();
    }, FunctionSignature{{"any"}, "void"});

    // printf - formatted printing with {} placeholders
    registerFunction("prelude", "printf", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::any();

        std::string format;
        try {
            format = std::any_cast<std::string>(args[0]);
        } catch (...) {
            std::cout << "[printf error: first argument must be string]" << std::endl;
            return std::any();
        }

        std::string result = format;
        size_t argIndex = 1;

        // Replace {} with arguments in order
        size_t pos = 0;
        while ((pos = result.find("{}", pos)) != std::string::npos && argIndex < args.size()) {
            std::string replacement;
            const auto& arg = args[argIndex];

            try {
                if (arg.type() == typeid(int)) {
                    replacement = std::to_string(std::any_cast<int>(arg));
                } else if (arg.type() == typeid(double)) {
                    replacement = std::to_string(std::any_cast<double>(arg));
                } else if (arg.type() == typeid(std::string)) {
                    replacement = std::any_cast<std::string>(arg);
                } else if (arg.type() == typeid(bool)) {
                    replacement = std::any_cast<bool>(arg) ? "true" : "false";
                } else if (arg.type() == typeid(char)) {
                    replacement = std::string(1, std::any_cast<char>(arg));
                } else {
                    replacement = "[unknown]";
                }
            } catch (...) {
                replacement = "[error]";
            }

            result.replace(pos, 2, replacement);
            pos += replacement.length();
            argIndex++;
        }

        // Process escape sequences
        size_t escPos = 0;
        while ((escPos = result.find('\\', escPos)) != std::string::npos) {
            if (escPos + 1 < result.length()) {
                char nextChar = result[escPos + 1];
                switch (nextChar) {
                    case 'n':
                        result.replace(escPos, 2, "\n");
                        break;
                    case 't':
                        result.replace(escPos, 2, "\t");
                        break;
                    case 'r':
                        result.replace(escPos, 2, "\r");
                        break;
                    case '\\':
                        result.replace(escPos, 2, "\\");
                        escPos++;  // Skip the replacement to avoid infinite loop
                        break;
                    default:
                        escPos++;  // Skip unknown escape sequences
                        break;
                }
            }
            escPos++;
        }

        std::cout << result;
        std::cout.flush();
        return std::any();
    }, FunctionSignature{{"string", "..."}, "void"});

    // panic - Unrecoverable error (terminates program)
    registerFunction("prelude", "panic", [](const std::vector<std::any>& args) -> std::any {
        std::string message = "panic: program terminated";

        if (!args.empty()) {
            try {
                if (args[0].type() == typeid(std::string)) {
                    message = "panic: " + std::any_cast<std::string>(args[0]);
                } else if (args[0].type() == typeid(int)) {
                    message = "panic: " + std::to_string(std::any_cast<int>(args[0]));
                } else if (args[0].type() == typeid(double)) {
                    message = "panic: " + std::to_string(std::any_cast<double>(args[0]));
                } else if (args[0].type() == typeid(bool)) {
                    message = "panic: " + std::string(std::any_cast<bool>(args[0]) ? "true" : "false");
                }
            } catch (...) {
                message = "panic: [error formatting message]";
            }
        }

        std::cerr << message << std::endl;
        std::exit(1);
        return std::any(); // Never reached
    }, FunctionSignature{{"any"}, "void"});

    // assert - Runtime assertion (panics if condition is false)
    registerFunction("prelude", "assert", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            std::cerr << "panic: assert called with no arguments" << std::endl;
            std::exit(1);
        }

        bool condition = false;
        try {
            if (args[0].type() == typeid(bool)) {
                condition = std::any_cast<bool>(args[0]);
            } else if (args[0].type() == typeid(int)) {
                condition = std::any_cast<int>(args[0]) != 0;
            } else if (args[0].type() == typeid(double)) {
                condition = std::any_cast<double>(args[0]) != 0.0;
            } else {
                std::cerr << "panic: assert condition must be boolean or numeric" << std::endl;
                std::exit(1);
            }
        } catch (...) {
            std::cerr << "panic: assert condition evaluation failed" << std::endl;
            std::exit(1);
        }

        if (!condition) {
            std::string message = "assertion failed";

            if (args.size() > 1) {
                try {
                    if (args[1].type() == typeid(std::string)) {
                        message = "assertion failed: " + std::any_cast<std::string>(args[1]);
                    }
                } catch (...) {
                    // Keep default message
                }
            }

            std::cerr << "panic: " << message << std::endl;
            std::exit(1);
        }

        return std::any();
    }, FunctionSignature{{"bool", "string"}, "void"});

    // dbg - Debug print with value inspection
    registerFunction("prelude", "dbg", [](const std::vector<std::any>& args) -> std::any {
        std::cerr << "[DEBUG] ";

        if (args.empty()) {
            std::cerr << "(no value)" << std::endl;
            return std::any();
        }

        const auto& arg = args[0];

        try {
            if (arg.type() == typeid(int)) {
                int val = std::any_cast<int>(arg);
                std::cerr << "int = " << val << std::endl;
                return val;
            } else if (arg.type() == typeid(double)) {
                double val = std::any_cast<double>(arg);
                std::cerr << "double = " << val << std::endl;
                return val;
            } else if (arg.type() == typeid(std::string)) {
                std::string val = std::any_cast<std::string>(arg);
                std::cerr << "string = \"" << val << "\"" << std::endl;
                return val;
            } else if (arg.type() == typeid(bool)) {
                bool val = std::any_cast<bool>(arg);
                std::cerr << "bool = " << (val ? "true" : "false") << std::endl;
                return val;
            } else if (arg.type() == typeid(char)) {
                char val = std::any_cast<char>(arg);
                std::cerr << "char = '" << val << "'" << std::endl;
                return val;
            } else {
                std::cerr << "unknown type" << std::endl;
            }
        } catch (...) {
            std::cerr << "[error inspecting value]" << std::endl;
        }

        return arg; // Return the value for chaining
    }, FunctionSignature{{"any"}, "any"});
}

// ============================================================================
// Math Module Native Functions
// ============================================================================

void NativeRegistry::initMath() {
    // Constants (registered as zero-parameter functions)
    registerFunction("math", "PI", [](const std::vector<std::any>& args) -> std::any {
        return 3.14159265358979323846;
    }, FunctionSignature{{}, "double"});

    registerFunction("math", "E", [](const std::vector<std::any>& args) -> std::any {
        return 2.71828182845904523536;
    }, FunctionSignature{{}, "double"});

    registerFunction("math", "PHI", [](const std::vector<std::any>& args) -> std::any {
        return 1.61803398874989484820;
    }, FunctionSignature{{}, "double"});

    // Trigonometric functions
    registerFunction("math", "sin", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::sin(x);
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "cos", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::cos(x);
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "tan", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::tan(x);
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "asin", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::asin(x);
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "acos", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::acos(x);
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "atan", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::atan(x);
    }, FunctionSignature{{"double"}, "double"});

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
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "log2", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::log2(x);
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "pow", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        double y = std::any_cast<double>(args[1]);
        return std::pow(x, y);
    }, FunctionSignature{{"double", "double"}, "double"});

    registerFunction("math", "sqrt", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::sqrt(x);
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "cbrt", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::cbrt(x);
    }, FunctionSignature{{"double"}, "double"});

    // Rounding
    registerFunction("math", "ceil", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::ceil(x);
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "floor", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::floor(x);
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "round", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::round(x);
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "trunc", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::trunc(x);
    }, FunctionSignature{{"double"}, "double"});

    // Absolute and sign
    registerFunction("math", "abs", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::abs(x);
    }, FunctionSignature{{"double"}, "double"});

    registerFunction("math", "sign", [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return (x > 0) ? 1 : (x < 0) ? -1 : 0;
    }, FunctionSignature{{"double"}, "int"});

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
    // log.debug(message) - Log debug message
    registerFunction("log", "debug", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::any();

        try {
            std::string message = std::any_cast<std::string>(args[0]);
            Logger::instance().debug(message);
        } catch (...) {
            Logger::instance().error("Failed to log debug message: invalid type");
        }

        return std::any();
    }, FunctionSignature{{"string"}, "void"});

    // log.info(message) - Log info message
    registerFunction("log", "info", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::any();

        try {
            std::string message = std::any_cast<std::string>(args[0]);
            Logger::instance().info(message);
        } catch (...) {
            Logger::instance().error("Failed to log info message: invalid type");
        }

        return std::any();
    }, FunctionSignature{{"string"}, "void"});

    // log.warn(message) - Log warning message
    registerFunction("log", "warn", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::any();

        try {
            std::string message = std::any_cast<std::string>(args[0]);
            Logger::instance().warn(message);
        } catch (...) {
            Logger::instance().error("Failed to log warning message: invalid type");
        }

        return std::any();
    }, FunctionSignature{{"string"}, "void"});

    // log.error(message) - Log error message
    registerFunction("log", "error", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::any();

        try {
            std::string message = std::any_cast<std::string>(args[0]);
            Logger::instance().error(message);
        } catch (...) {
            std::cerr << "[ERROR] Failed to log error message: invalid type" << std::endl;
        }

        return std::any();
    }, FunctionSignature{{"string"}, "void"});

    // log.fatal(message) - Log fatal message and exit
    registerFunction("log", "fatal", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            Logger::instance().fatal("Fatal error occurred");
            std::exit(1);
        }

        try {
            std::string message = std::any_cast<std::string>(args[0]);
            Logger::instance().fatal(message);
            std::exit(1);
        } catch (...) {
            std::cerr << "[FATAL] Failed to log fatal message: invalid type" << std::endl;
            std::exit(1);
        }

        return std::any();
    }, FunctionSignature{{"string"}, "void"});
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

// ============================================================================
// JSON Module Native Functions
// ============================================================================

void NativeRegistry::initJSON() {
    // Simple JSON parsing (basic implementation - would use nlohmann/json in production)
    registerFunction("json", "parse", [](const std::vector<std::any>& args) -> std::any {
        std::string jsonStr = std::any_cast<std::string>(args[0]);
        // Simplified: Return the string as-is (real impl would parse to JsonValue)
        return jsonStr;
    });

    registerFunction("json", "stringify", [](const std::vector<std::any>& args) -> std::any {
        // Simplified: Convert JsonValue to string
        return std::string("{}");
    });

    registerFunction("json", "stringifyPretty", [](const std::vector<std::any>& args) -> std::any {
        std::string indent = args.size() > 1 ? std::any_cast<std::string>(args[1]) : "  ";
        return std::string("{\n}");
    });
}

// ============================================================================
// Base64 Module Native Functions
// ============================================================================

void NativeRegistry::initBase64() {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    // Base64 encoding
    registerFunction("base64", "encode", [](const std::vector<std::any>& args) -> std::any {
        auto bytes = std::any_cast<std::vector<uint8_t>>(args[0]);
        std::string ret;
        int i = 0;
        int j = 0;
        uint8_t char_array_3[3];
        uint8_t char_array_4[4];

        for (size_t idx = 0; idx < bytes.size(); idx++) {
            char_array_3[i++] = bytes[idx];
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for(i = 0; i < 4; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i) {
            for(j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

            for (j = 0; j < i + 1; j++)
                ret += base64_chars[char_array_4[j]];

            while(i++ < 3)
                ret += '=';
        }

        return ret;
    });

    // Base64 string encoding
    registerFunction("base64", "encodeString", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        std::vector<uint8_t> bytes(s.begin(), s.end());
        // Call encode implementation (simplified - would reuse above)
        return s; // Placeholder
    });

    // Base64 decoding
    registerFunction("base64", "decode", [](const std::vector<std::any>& args) -> std::any {
        std::string encoded = std::any_cast<std::string>(args[0]);
        std::vector<uint8_t> ret;
        // Simplified implementation
        return ret;
    });
}

// ============================================================================
// CSV Module Native Functions
// ============================================================================

void NativeRegistry::initCSV() {
    registerFunction("csv", "parse", [](const std::vector<std::any>& args) -> std::any {
        std::string csvContent = std::any_cast<std::string>(args[0]);
        std::vector<std::vector<std::string>> result;

        std::stringstream ss(csvContent);
        std::string line;

        while (std::getline(ss, line)) {
            std::vector<std::string> row;
            std::stringstream lineStream(line);
            std::string cell;

            while (std::getline(lineStream, cell, ',')) {
                // Trim whitespace
                size_t start = cell.find_first_not_of(" \t");
                size_t end = cell.find_last_not_of(" \t");
                if (start != std::string::npos) {
                    cell = cell.substr(start, end - start + 1);
                }
                row.push_back(cell);
            }

            if (!row.empty()) {
                result.push_back(row);
            }
        }

        return result;
    });

    registerFunction("csv", "stringify", [](const std::vector<std::any>& args) -> std::any {
        auto records = std::any_cast<std::vector<std::vector<std::string>>>(args[0]);
        std::stringstream result;

        for (size_t i = 0; i < records.size(); i++) {
            for (size_t j = 0; j < records[i].size(); j++) {
                result << records[i][j];
                if (j < records[i].size() - 1) {
                    result << ",";
                }
            }
            if (i < records.size() - 1) {
                result << "\n";
            }
        }

        return result.str();
    });
}

// ============================================================================
// Crypto Module Native Functions
// ============================================================================

void NativeRegistry::initCrypto() {
    // MD5 hash (simplified - would use OpenSSL in production)
    registerFunction("crypto", "md5String", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        // Simplified: return placeholder hash
        return std::string("d41d8cd98f00b204e9800998ecf8427e");
    });

    // SHA-256 hash (simplified)
    registerFunction("crypto", "sha256String", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        // Simplified: return placeholder hash
        return std::string("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    });

    // Random bytes
    registerFunction("crypto", "randomBytes", [](const std::vector<std::any>& args) -> std::any {
        int length = std::any_cast<int>(args[0]);
        std::vector<uint8_t> bytes(length);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        for (int i = 0; i < length; i++) {
            bytes[i] = static_cast<uint8_t>(dis(gen));
        }

        return bytes;
    });

    // Random hex string
    registerFunction("crypto", "randomHex", [](const std::vector<std::any>& args) -> std::any {
        int length = std::any_cast<int>(args[0]);
        std::string hex = "0123456789abcdef";
        std::string result;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);

        for (int i = 0; i < length; i++) {
            result += hex[dis(gen)];
        }

        return result;
    });

    // bcrypt (simplified)
    registerFunction("crypto", "bcrypt", [](const std::vector<std::any>& args) -> std::any {
        std::string password = std::any_cast<std::string>(args[0]);
        int rounds = std::any_cast<int>(args[1]);
        // Simplified: return placeholder hash
        return std::string("$2b$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy");
    });

    registerFunction("crypto", "bcryptVerify", [](const std::vector<std::any>& args) -> std::any {
        std::string password = std::any_cast<std::string>(args[0]);
        std::string hash = std::any_cast<std::string>(args[1]);
        // Simplified: always return true for demo
        return true;
    });
}

// ============================================================================
// Zip Module Native Functions
// ============================================================================

void NativeRegistry::initZip() {
    // Compress string (simplified - would use zlib in production)
    registerFunction("zip", "compress", [](const std::vector<std::any>& args) -> std::any {
        std::string data = std::any_cast<std::string>(args[0]);
        std::vector<uint8_t> compressed(data.begin(), data.end());
        // Simplified: return data as-is (real impl would compress)
        return compressed;
    });

    // Decompress
    registerFunction("zip", "decompress", [](const std::vector<std::any>& args) -> std::any {
        auto compressed = std::any_cast<std::vector<uint8_t>>(args[0]);
        std::string decompressed(compressed.begin(), compressed.end());
        // Simplified: return data as-is (real impl would decompress)
        return decompressed;
    });

    // Compress file
    registerFunction("zip", "compressFile", [](const std::vector<std::any>& args) -> std::any {
        std::string inputPath = std::any_cast<std::string>(args[0]);
        std::string outputPath = std::any_cast<std::string>(args[1]);

        // Read input file
        std::ifstream input(inputPath, std::ios::binary);
        if (!input.is_open()) {
            return false;
        }

        // Read all bytes
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(input)),
                                   std::istreambuf_iterator<char>());
        input.close();

        // Write to output (simplified - would compress in real impl)
        std::ofstream output(outputPath, std::ios::binary);
        if (!output.is_open()) {
            return false;
        }

        output.write(reinterpret_cast<const char*>(data.data()), data.size());
        output.close();

        return true;
    });

    // Create zip archive
    registerFunction("zip", "createArchive", [](const std::vector<std::any>& args) -> std::any {
        std::string archivePath = std::any_cast<std::string>(args[0]);
        auto files = std::any_cast<std::vector<std::string>>(args[1]);

        // Simplified: would use libzip or similar in production
        std::cout << "Creating archive: " << archivePath << std::endl;
        for (const auto& file : files) {
            std::cout << "  Adding: " << file << std::endl;
        }

        return true;
    });

    // Extract zip archive
    registerFunction("zip", "extractArchive", [](const std::vector<std::any>& args) -> std::any {
        std::string archivePath = std::any_cast<std::string>(args[0]);
        std::string destPath = std::any_cast<std::string>(args[1]);

        // Simplified: would use libzip or similar in production
        std::cout << "Extracting archive: " << archivePath << " to " << destPath << std::endl;

        return true;
    });
}

// ============================================================================
// FFI Module - Foreign Function Interface
// ============================================================================

void NativeRegistry::initFFI() {
    // ffi.load - Load a shared library
    // Usage: val lib = ffi.load("path/to/library.so")
    // Returns: Library ID (int)
    registerFunction("ffi", "load", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("ffi.load requires a library path");
        }

        std::string path = std::any_cast<std::string>(args[0]);

        try {
            int libraryId = FFIManager::instance().loadLibrary(path);
            return std::any(libraryId);
        } catch (const std::exception& e) {
            std::cerr << "FFI Error: " << e.what() << std::endl;
            return std::any(-1);  // Error indicator
        }
    }, FunctionSignature{{"string"}, "int"});

    // ffi.unload - Unload a shared library
    // Usage: ffi.unload(lib)
    registerFunction("ffi", "unload", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("ffi.unload requires a library ID");
        }

        int libraryId = std::any_cast<int>(args[0]);
        FFIManager::instance().unloadLibrary(libraryId);

        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // ffi.call - Call a foreign function
    // Usage: val result = ffi.call(lib, "function_name", "return_type", ["param_types"], [args])
    // Example: val result = ffi.call(lib, "add", "int", ["int", "int"], [5, 3])
    registerFunction("ffi", "call", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 5) {
            throw std::runtime_error("ffi.call requires: (lib_id, func_name, return_type, param_types, args)");
        }

        int libraryId = std::any_cast<int>(args[0]);
        std::string functionName = std::any_cast<std::string>(args[1]);
        std::string returnTypeStr = std::any_cast<std::string>(args[2]);

        // Parse parameter types
        std::vector<std::string> paramTypeStrs = std::any_cast<std::vector<std::string>>(args[3]);
        std::vector<FFIType> paramTypes;
        for (const auto& typeStr : paramTypeStrs) {
            paramTypes.push_back(FFIManager::parseType(typeStr));
        }

        // Parse return type
        FFIType returnType = FFIManager::parseType(returnTypeStr);

        // Extract function arguments
        std::vector<std::any> funcArgs = std::any_cast<std::vector<std::any>>(args[4]);

        // Create signature
        FFISignature signature{paramTypes, returnType};

        try {
            return FFIManager::instance().callFunction(libraryId, functionName, signature, funcArgs);
        } catch (const std::exception& e) {
            std::cerr << "FFI Error calling " << functionName << ": " << e.what() << std::endl;
            throw;
        }
    });

    // ffi.callVoid - Simplified version for void return functions
    // Usage: ffi.callVoid(lib, "function_name", ["param_types"], [args])
    registerFunction("ffi", "callVoid", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 4) {
            throw std::runtime_error("ffi.callVoid requires: (lib_id, func_name, param_types, args)");
        }

        int libraryId = std::any_cast<int>(args[0]);
        std::string functionName = std::any_cast<std::string>(args[1]);

        // Parse parameter types
        std::vector<std::string> paramTypeStrs = std::any_cast<std::vector<std::string>>(args[2]);
        std::vector<FFIType> paramTypes;
        for (const auto& typeStr : paramTypeStrs) {
            paramTypes.push_back(FFIManager::parseType(typeStr));
        }

        // Extract function arguments
        std::vector<std::any> funcArgs = std::any_cast<std::vector<std::any>>(args[3]);

        // Create signature with void return
        FFISignature signature{paramTypes, FFIType::VOID};

        try {
            FFIManager::instance().callFunction(libraryId, functionName, signature, funcArgs);
        } catch (const std::exception& e) {
            std::cerr << "FFI Error calling " << functionName << ": " << e.what() << std::endl;
            throw;
        }

        return std::any();
    });

    // ffi.callInt - Simplified version for int return functions
    // Usage: val result = ffi.callInt(lib, "add", ["int", "int"], [5, 3])
    registerFunction("ffi", "callInt", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 4) {
            throw std::runtime_error("ffi.callInt requires: (lib_id, func_name, param_types, args)");
        }

        int libraryId = std::any_cast<int>(args[0]);
        std::string functionName = std::any_cast<std::string>(args[1]);

        // Parse parameter types
        std::vector<std::string> paramTypeStrs = std::any_cast<std::vector<std::string>>(args[2]);
        std::vector<FFIType> paramTypes;
        for (const auto& typeStr : paramTypeStrs) {
            paramTypes.push_back(FFIManager::parseType(typeStr));
        }

        // Extract function arguments
        std::vector<std::any> funcArgs = std::any_cast<std::vector<std::any>>(args[3]);

        // Create signature with int return
        FFISignature signature{paramTypes, FFIType::INT};

        try {
            return FFIManager::instance().callFunction(libraryId, functionName, signature, funcArgs);
        } catch (const std::exception& e) {
            std::cerr << "FFI Error calling " << functionName << ": " << e.what() << std::endl;
            throw;
        }
    }, FunctionSignature{{"int", "string", "array", "array"}, "int"});

    // ffi.callDouble - Simplified version for double return functions
    registerFunction("ffi", "callDouble", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 4) {
            throw std::runtime_error("ffi.callDouble requires: (lib_id, func_name, param_types, args)");
        }

        int libraryId = std::any_cast<int>(args[0]);
        std::string functionName = std::any_cast<std::string>(args[1]);

        // Parse parameter types
        std::vector<std::string> paramTypeStrs = std::any_cast<std::vector<std::string>>(args[2]);
        std::vector<FFIType> paramTypes;
        for (const auto& typeStr : paramTypeStrs) {
            paramTypes.push_back(FFIManager::parseType(typeStr));
        }

        // Extract function arguments
        std::vector<std::any> funcArgs = std::any_cast<std::vector<std::any>>(args[3]);

        // Create signature with double return
        FFISignature signature{paramTypes, FFIType::DOUBLE};

        try {
            return FFIManager::instance().callFunction(libraryId, functionName, signature, funcArgs);
        } catch (const std::exception& e) {
            std::cerr << "FFI Error calling " << functionName << ": " << e.what() << std::endl;
            throw;
        }
    }, FunctionSignature{{"int", "string", "array", "array"}, "double"});
}

// ============================================================================
// WebSocket Module - WebSocket client support
// ============================================================================

void NativeRegistry::initWebSocket() {
    // websocket.connect - Connect to WebSocket server
    // Usage: val ws = websocket.connect("ws://localhost:8080/chat")
    // Returns: WebSocket connection ID (int)
    registerFunction("websocket", "connect", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("websocket.connect requires a URL");
        }

        std::string url = std::any_cast<std::string>(args[0]);

        try {
            int wsId = WebSocketManager::instance().connect(url);
            return std::any(wsId);
        } catch (const std::exception& e) {
            std::cerr << "WebSocket Error: " << e.what() << std::endl;
            return std::any(-1);  // Error indicator
        }
    }, FunctionSignature{{"string"}, "int"});

    // websocket.send - Send text message
    // Usage: websocket.send(ws, "Hello!")
    registerFunction("websocket", "send", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 2) {
            throw std::runtime_error("websocket.send requires (wsId, message)");
        }

        int wsId = std::any_cast<int>(args[0]);
        std::string message = std::any_cast<std::string>(args[1]);

        try {
            WebSocketManager::instance().send(wsId, message);
        } catch (const std::exception& e) {
            std::cerr << "WebSocket Error: " << e.what() << std::endl;
        }

        return std::any();
    }, FunctionSignature{{"int", "string"}, "void"});

    // websocket.receive - Receive message (blocking with timeout)
    // Usage: val message = websocket.receive(ws)
    // Returns: Message string (empty if timeout/error)
    registerFunction("websocket", "receive", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("websocket.receive requires a WebSocket ID");
        }

        int wsId = std::any_cast<int>(args[0]);

        // Optional timeout parameter (default 5000ms)
        int timeout = 5000;
        if (args.size() > 1) {
            timeout = std::any_cast<int>(args[1]);
        }

        try {
            std::string message = WebSocketManager::instance().receive(wsId, timeout);
            return std::any(message);
        } catch (const std::exception& e) {
            std::cerr << "WebSocket Error: " << e.what() << std::endl;
            return std::any(std::string(""));
        }
    }, FunctionSignature{{"int"}, "string"});

    // websocket.close - Close WebSocket connection
    // Usage: websocket.close(ws)
    registerFunction("websocket", "close", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("websocket.close requires a WebSocket ID");
        }

        int wsId = std::any_cast<int>(args[0]);

        try {
            WebSocketManager::instance().close(wsId);
        } catch (const std::exception& e) {
            std::cerr << "WebSocket Error: " << e.what() << std::endl;
        }

        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // websocket.isConnected - Check if WebSocket is connected
    // Usage: val connected = websocket.isConnected(ws)
    // Returns: true if connected, false otherwise
    registerFunction("websocket", "isConnected", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("websocket.isConnected requires a WebSocket ID");
        }

        int wsId = std::any_cast<int>(args[0]);

        try {
            bool connected = WebSocketManager::instance().isConnected(wsId);
            return std::any(connected ? 1 : 0);  // Return as int (Stratos bool)
        } catch (const std::exception& e) {
            return std::any(0);  // Return false on error
        }
    }, FunctionSignature{{"int"}, "bool"});

    // ====================================================================
    // SERVER FUNCTIONS
    // ====================================================================

    // websocket.createServer - Create WebSocket server listening on port
    // Usage: val server = websocket.createServer(8080)
    // Returns: Server ID (int), -1 on error
    registerFunction("websocket", "createServer", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("websocket.createServer requires a port number");
        }

        int port = std::any_cast<int>(args[0]);

        try {
            int serverId = WebSocketManager::instance().createServer(port);
            return std::any(serverId);
        } catch (const std::exception& e) {
            std::cerr << "WebSocket Server Error: " << e.what() << std::endl;
            return std::any(-1);
        }
    }, FunctionSignature{{"int"}, "int"});

    // websocket.acceptClient - Accept incoming client connection (non-blocking)
    // Usage: val client = websocket.acceptClient(server)
    // Returns: Client ID (int), -1 if no connection or timeout
    registerFunction("websocket", "acceptClient", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("websocket.acceptClient requires a server ID");
        }

        int serverId = std::any_cast<int>(args[0]);

        // Optional timeout parameter (default 1000ms)
        int timeout = 1000;
        if (args.size() > 1) {
            timeout = std::any_cast<int>(args[1]);
        }

        try {
            int clientId = WebSocketManager::instance().acceptClient(serverId, timeout);
            return std::any(clientId);
        } catch (const std::exception& e) {
            std::cerr << "WebSocket Server Error: " << e.what() << std::endl;
            return std::any(-1);
        }
    }, FunctionSignature{{"int"}, "int"});

    // websocket.sendToClient - Send message to specific client
    // Usage: websocket.sendToClient(client, "Hello!")
    registerFunction("websocket", "sendToClient", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 2) {
            throw std::runtime_error("websocket.sendToClient requires (clientId, message)");
        }

        int clientId = std::any_cast<int>(args[0]);
        std::string message = std::any_cast<std::string>(args[1]);

        try {
            WebSocketManager::instance().sendToClient(clientId, message);
        } catch (const std::exception& e) {
            std::cerr << "WebSocket Server Error: " << e.what() << std::endl;
        }

        return std::any();
    }, FunctionSignature{{"int", "string"}, "void"});

    // websocket.receiveFromClient - Receive message from specific client
    // Usage: val message = websocket.receiveFromClient(client)
    // Returns: Message string (empty if timeout/error)
    registerFunction("websocket", "receiveFromClient", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("websocket.receiveFromClient requires a client ID");
        }

        int clientId = std::any_cast<int>(args[0]);

        // Optional timeout parameter (default 5000ms)
        int timeout = 5000;
        if (args.size() > 1) {
            timeout = std::any_cast<int>(args[1]);
        }

        try {
            std::string message = WebSocketManager::instance().receiveFromClient(clientId, timeout);
            return std::any(message);
        } catch (const std::exception& e) {
            std::cerr << "WebSocket Server Error: " << e.what() << std::endl;
            return std::any(std::string(""));
        }
    }, FunctionSignature{{"int"}, "string"});

    // websocket.closeClient - Close client connection
    // Usage: websocket.closeClient(client)
    registerFunction("websocket", "closeClient", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("websocket.closeClient requires a client ID");
        }

        int clientId = std::any_cast<int>(args[0]);

        try {
            WebSocketManager::instance().closeClient(clientId);
        } catch (const std::exception& e) {
            std::cerr << "WebSocket Server Error: " << e.what() << std::endl;
        }

        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // websocket.closeServer - Close server
    // Usage: websocket.closeServer(server)
    registerFunction("websocket", "closeServer", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("websocket.closeServer requires a server ID");
        }

        int serverId = std::any_cast<int>(args[0]);

        try {
            WebSocketManager::instance().closeServer(serverId);
        } catch (const std::exception& e) {
            std::cerr << "WebSocket Server Error: " << e.what() << std::endl;
        }

        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // websocket.isClientConnected - Check if client is connected
    // Usage: val connected = websocket.isClientConnected(client)
    // Returns: true if connected, false otherwise
    registerFunction("websocket", "isClientConnected", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("websocket.isClientConnected requires a client ID");
        }

        int clientId = std::any_cast<int>(args[0]);

        try {
            bool connected = WebSocketManager::instance().isClientConnected(clientId);
            return std::any(connected ? 1 : 0);  // Return as int (Stratos bool)
        } catch (const std::exception& e) {
            return std::any(0);  // Return false on error
        }
    }, FunctionSignature{{"int"}, "bool"});
}

} // namespace stratos
