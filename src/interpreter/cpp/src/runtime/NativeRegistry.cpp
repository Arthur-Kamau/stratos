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
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <regex>
#include <sqlite3.h>
#include "stratos/Interpreter.h"

// Platform-specific includes for terminal control
#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    #include <io.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
#endif

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

bool NativeRegistry::hasModule(const std::string& moduleName) const {
    std::string modulePrefix = moduleName + "::";
    for (const auto& [name, _] : functions_) {
        if (name.starts_with(modulePrefix)) {
            return true;
        }
    }
    return false;
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
    initConvert();   // Number/type conversion functions
    initIO();
    initLog();
    initTime();
    initJSON();
    initYAML();
    initBase64();
    initBase64();
    initCSV();
    initCrypto();
    initZip();
    initFFI();
    initWebSocket();
    initTerminal();
    initRegex();
    initSQLite();    // SQLite database support
    initCollections(); // Collections module
}

// ============================================================================
// Collections Module Native Functions
// ============================================================================

void NativeRegistry::initCollections() {
    // newMap<K, V>() - Implementation returns a native map
    // We return a map<string, any> as a safe default for generic maps
    // In a real implementation we might specialize based on inferred types if passed
    registerFunction("collections", "newMap", [](const std::vector<std::any>& args) -> std::any {
        // Return an empty map<string, any>
        std::unordered_map<std::string, std::any> map;
        return map;
    }, FunctionSignature{{}, "Map<K, V>"});

    // mapOf<K, V>(entries) - Create map from entries
    registerFunction("collections", "mapOf", [](const std::vector<std::any>& args) -> std::any {
        std::unordered_map<std::string, std::any> map;
        // Logic to populate map from entries would go here
        // For now return empty or simple implementation if args provided
        // This is complex as entries is Array<Pair<K,V>> which is List<Object>...
        return map;
    }, FunctionSignature{{"Array<Pair<K, V>>"}, "Map<K, V>"});

    // newList<T>() - Implementation returns a native array (vector)
    registerFunction("collections", "newList", [](const std::vector<std::any>& args) -> std::any {
        std::vector<std::any> list;
        return list; // standard vector<any>
    }, FunctionSignature{{}, "List<T>"});
    
    // newSet<T>() - Implementation returns a native set (if supported) or array
    // Since native set support is limited in Interpreter right now, we can return a vector
    // and let Stratos class wrap it, OR implementing Set is out of scope for this 'Array/Map' task.
    // We register it to satisfy symbol resolution.
    registerFunction("collections", "newSet", [](const std::vector<std::any>& args) -> std::any {
        std::vector<std::any> set; // Placeholder using vector
        return set;
    }, FunctionSignature{{}, "Set<T>"});
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

        bool useFormatting = false;
        if (args.size() > 1 && args[0].type() == typeid(std::string)) {
            std::string fmt = std::any_cast<std::string>(args[0]);
            if (fmt.find("{}") != std::string::npos) {
                useFormatting = true;
            }
        }

        if (useFormatting) {
            // Printf-style formatting
            std::string result;
            try {
                result = std::any_cast<std::string>(args[0]);
            } catch (...) {
                result = ""; 
            }

            size_t argIndex = 1;
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
            std::cout << result;
        } else {
            // Space-separated printing
            for (size_t i = 0; i < args.size(); ++i) {
                const auto& arg = args[i];
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
                
                if (i < args.size() - 1) {
                    std::cout << " ";
                }
            }
        }

        std::cout << std::endl;
        return std::any();
    }, FunctionSignature{{"any", "..."}, "void"});

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

    registerFunction("math", "ln", [](const std::vector<std::any>& args) -> std::any {
        double val = std::any_cast<double>(args[0]);
        return std::log(val);
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
    }, FunctionSignature{{"string", "string"}, "array<string>"});

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
// Convert Module - Number and Type Conversions
// ============================================================================

void NativeRegistry::initConvert() {
    // String to number conversions
    registerFunction("convert", "toInt", [](const std::vector<std::any>& args) -> std::any {
        try {
            if (args[0].type() == typeid(std::string)) {
                std::string s = std::any_cast<std::string>(args[0]);
                return std::stoi(s);
            } else if (args[0].type() == typeid(double)) {
                return static_cast<int>(std::any_cast<double>(args[0]));
            } else if (args[0].type() == typeid(int)) {
                return std::any_cast<int>(args[0]);
            } else if (args[0].type() == typeid(bool)) {
                return std::any_cast<bool>(args[0]) ? 1 : 0;
            }
        } catch (...) {
            return 0;  // Default on error
        }
        return 0;
    }, FunctionSignature{{"any"}, "int"});

    registerFunction("convert", "toDouble", [](const std::vector<std::any>& args) -> std::any {
        try {
            if (args[0].type() == typeid(std::string)) {
                std::string s = std::any_cast<std::string>(args[0]);
                return std::stod(s);
            } else if (args[0].type() == typeid(int)) {
                return static_cast<double>(std::any_cast<int>(args[0]));
            } else if (args[0].type() == typeid(double)) {
                return std::any_cast<double>(args[0]);
            } else if (args[0].type() == typeid(bool)) {
                return std::any_cast<bool>(args[0]) ? 1.0 : 0.0;
            }
        } catch (...) {
            return 0.0;  // Default on error
        }
        return 0.0;
    }, FunctionSignature{{"any"}, "double"});

    registerFunction("convert", "toString", [](const std::vector<std::any>& args) -> std::any {
        try {
            if (args[0].type() == typeid(std::string)) {
                return std::any_cast<std::string>(args[0]);
            } else if (args[0].type() == typeid(int)) {
                return std::to_string(std::any_cast<int>(args[0]));
            } else if (args[0].type() == typeid(double)) {
                return std::to_string(std::any_cast<double>(args[0]));
            } else if (args[0].type() == typeid(bool)) {
                return std::any_cast<bool>(args[0]) ? std::string("true") : std::string("false");
            } else if (args[0].type() == typeid(char)) {
                return std::string(1, std::any_cast<char>(args[0]));
            }
        } catch (...) {
            return std::string("");
        }
        return std::string("");
    }, FunctionSignature{{"any"}, "string"});

    registerFunction("convert", "toBool", [](const std::vector<std::any>& args) -> std::any {
        try {
            if (args[0].type() == typeid(bool)) {
                return std::any_cast<bool>(args[0]);
            } else if (args[0].type() == typeid(int)) {
                return std::any_cast<int>(args[0]) != 0;
            } else if (args[0].type() == typeid(double)) {
                return std::any_cast<double>(args[0]) != 0.0;
            } else if (args[0].type() == typeid(std::string)) {
                std::string s = std::any_cast<std::string>(args[0]);
                return s == "true" || s == "1" || s == "yes";
            }
        } catch (...) {
            return false;
        }
        return false;
    }, FunctionSignature{{"any"}, "bool"});

    // Number formatting functions
    registerFunction("convert", "toFixed", [](const std::vector<std::any>& args) -> std::any {
        double value = std::any_cast<double>(args[0]);
        int decimals = std::any_cast<int>(args[1]);

        std::ostringstream out;
        out << std::fixed << std::setprecision(decimals) << value;
        return out.str();
    }, FunctionSignature{{"double", "int"}, "string"});

    registerFunction("convert", "toPrecision", [](const std::vector<std::any>& args) -> std::any {
        double value = std::any_cast<double>(args[0]);
        int precision = std::any_cast<int>(args[1]);

        std::ostringstream out;
        out << std::setprecision(precision) << value;
        return out.str();
    }, FunctionSignature{{"double", "int"}, "string"});

    registerFunction("convert", "toExponential", [](const std::vector<std::any>& args) -> std::any {
        double value = std::any_cast<double>(args[0]);
        int decimals = args.size() > 1 ? std::any_cast<int>(args[1]) : 2;

        std::ostringstream out;
        out << std::scientific << std::setprecision(decimals) << value;
        return out.str();
    }, FunctionSignature{{"double", "int"}, "string"});

    // Integer conversions with base
    registerFunction("convert", "toHex", [](const std::vector<std::any>& args) -> std::any {
        int value = std::any_cast<int>(args[0]);
        std::ostringstream out;
        out << "0x" << std::hex << value;
        return out.str();
    }, FunctionSignature{{"int"}, "string"});

    registerFunction("convert", "toOct", [](const std::vector<std::any>& args) -> std::any {
        int value = std::any_cast<int>(args[0]);
        std::ostringstream out;
        out << "0" << std::oct << value;
        return out.str();
    }, FunctionSignature{{"int"}, "string"});

    registerFunction("convert", "toBinary", [](const std::vector<std::any>& args) -> std::any {
        int value = std::any_cast<int>(args[0]);
        if (value == 0) return std::string("0b0");

        std::string binary;
        int n = value;
        bool negative = n < 0;
        if (negative) n = -n;

        while (n > 0) {
            binary = (char)('0' + (n % 2)) + binary;
            n /= 2;
        }

        return (negative ? std::string("-0b") : std::string("0b")) + binary;
    }, FunctionSignature{{"int"}, "string"});

    // Parse from different bases
    registerFunction("convert", "parseHex", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        // Remove 0x prefix if present
        if (s.substr(0, 2) == "0x" || s.substr(0, 2) == "0X") {
            s = s.substr(2);
        }
        return static_cast<int>(std::strtol(s.c_str(), nullptr, 16));
    }, FunctionSignature{{"string"}, "int"});

    registerFunction("convert", "parseOct", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        // Remove 0 prefix if present
        if (s[0] == '0' && s.length() > 1) {
            s = s.substr(1);
        }
        return static_cast<int>(std::strtol(s.c_str(), nullptr, 8));
    }, FunctionSignature{{"string"}, "int"});

    registerFunction("convert", "parseBinary", [](const std::vector<std::any>& args) -> std::any {
        std::string s = std::any_cast<std::string>(args[0]);
        // Remove 0b prefix if present
        if (s.substr(0, 2) == "0b" || s.substr(0, 2) == "0B") {
            s = s.substr(2);
        }
        return static_cast<int>(std::strtol(s.c_str(), nullptr, 2));
    }, FunctionSignature{{"string"}, "int"});
}

// ============================================================================
// Simple JSON Parser
// ============================================================================

class SimpleJsonParser {
    std::string json;
    size_t pos = 0;

    void skipWhitespace() {
        while (pos < json.length() && isspace(json[pos])) pos++;
    }

    char peek() {
        skipWhitespace();
        if (pos >= json.length()) return 0;
        return json[pos];
    }
    
    char rawPeek() {
        if (pos >= json.length()) return 0;
        return json[pos];
    }
    
    char consume() {
        char c = peek();
        if (c) pos++;
        return c;
    }

    bool expect(char c) {
        if (peek() == c) {
            consume();
            return true;
        }
        return false;
    }

public:
    SimpleJsonParser(std::string s) : json(s) {}

    RuntimeValue parse(const RuntimeValue& prototype) {
        skipWhitespace();
        char c = peek();
        if (c == '{') return parseObject(prototype);
        if (c == '[') return parseArray(prototype);
        if (c == '"') return parseString();
        if (isdigit(c) || c == '-') return parseNumber(prototype);
        if (c == 't' || c == 'f') return parseBool();
        return RuntimeValue(); 
    }
    
    // Implementations follow...
    RuntimeValue parseObject(const RuntimeValue& prototype) {
        consume(); // '{'
        
        auto instance = std::make_shared<ClassInstance>();
        if (prototype.type == "object") {
             // Clone prototype structure
            instance->className = prototype.asObject()->className;
            instance->fields = prototype.asObject()->fields;
        } else {
             instance->className = "Object";
        }

        while (peek() != '}' && peek() != 0) {
            std::string key = parseString().asString();
            expect(':');
            
            RuntimeValue fieldProto;
            if (instance->fields.count(key)) {
                fieldProto = instance->fields[key];
            } else {
                 fieldProto = RuntimeValue(std::any(), "any"); 
            }
            
            RuntimeValue val = parse(fieldProto);
            
            if (instance->fields.count(key)) {
                instance->fields[key] = val;
            } else if (prototype.type != "object") { // Dynamic object
                instance->fields[key] = val;
            }
            
            if (peek() == ',') consume();
        }
        consume(); // '}'
        return RuntimeValue(instance);
    }

    RuntimeValue parseArray(const RuntimeValue& prototype) {
        consume(); // '['
        std::vector<std::any> list;
        while (peek() != ']' && peek() != 0) {
            list.push_back(std::any()); // Placeholder
             // Skip generic value
             int braces = 0;
             while(pos < json.length()) {
                  char p = json[pos];
                  if (p == ',' && braces == 0) break;
                  if (p == ']' && braces == 0) break;
                  if (p == '{' || p == '[') braces++;
                  if (p == '}' || p == ']') braces--;
                  pos++;
             }
             if (peek() == ',') consume();
        }
        consume();
        return RuntimeValue(); 
    }

    RuntimeValue parseString() {
        consume(); // '"' - this consumes the opening quote (skipping leading whitespace before it)
        std::string s;
        while (rawPeek() != '"' && rawPeek() != 0) {
            if (rawPeek() == '\\') {
                pos++;
                if (rawPeek() == '"') s += '"';
                else if (rawPeek() == 'n') s += '\n';
                else if (rawPeek() == 't') s += '\t';
                else s += rawPeek();
                pos++;
            } else {
                s += json[pos++];
            }
        }
        if (rawPeek() == '"') pos++; 
        return RuntimeValue(s);
    }

    RuntimeValue parseNumber(const RuntimeValue& prototype) {
        size_t start = pos;
        if (peek() == '-') consume();
        while (isdigit(peek())) consume();
        if (peek() == '.') {
            consume();
            while (isdigit(peek())) consume();
            try {
                double d = std::stod(json.substr(start, pos - start));
                return RuntimeValue(d);
            } catch (...) { return RuntimeValue(0.0); }
        }
        try {
            int i = std::stoi(json.substr(start, pos - start));
            return RuntimeValue(i);
        } catch (...) { return RuntimeValue(0); }
    }
    
    RuntimeValue parseBool() {
        if (json.substr(pos, 4) == "true") {
            pos += 4;
            return RuntimeValue(true);
        }
         if (json.substr(pos, 5) == "false") {
            pos += 5;
            return RuntimeValue(false);
        }
        return RuntimeValue(false);
    }
};


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
    }, FunctionSignature{{"string"}, "string"});

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
        bool exists = fs::exists(path);
        return exists;
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

// Forward declaration
std::string serializeAny(const std::any& val);

std::string serializeRuntimeValue(const RuntimeValue& val) {
    if (val.type == "int") {
        return std::to_string(val.asInt());
    } else if (val.type == "double") {
        return std::to_string(val.asDouble());
    } else if (val.type == "bool") {
        return val.asBool() ? "true" : "false";
    } else if (val.type == "string") {
        std::string s = val.asString();
        // Escape string
        std::stringstream ss;
        ss << "\"";
        for (char c : s) {
            if (c == '"') ss << "\\\"";
            else if (c == '\\') ss << "\\\\";
            else if (c == '\b') ss << "\\b";
            else if (c == '\f') ss << "\\f";
            else if (c == '\n') ss << "\\n";
            else if (c == '\r') ss << "\\r";
            else if (c == '\t') ss << "\\t";
            else if (static_cast<unsigned char>(c) < 32 || static_cast<unsigned char>(c) > 126) {
                ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
            } else {
                ss << c;
            }
        }
        ss << "\"";
        return ss.str();
    } else if (val.type == "object") {
        auto instance = val.asObject();
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (const auto& [name, field] : instance->fields) {
            if (!first) ss << ", ";
            ss << "\"" << name << "\": " << serializeRuntimeValue(field);
            first = false;
        }
        ss << "}";
        return ss.str();
    } else if (val.type.starts_with("array")) {
         if (std::holds_alternative<std::any>(val.value)) {
             return serializeAny(std::get<std::any>(val.value));
         }
    }
    return "null";
}

std::string serializeAny(const std::any& val) {
    if (val.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(val));
    } else if (val.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(val));
    } else if (val.type() == typeid(bool)) {
        return std::any_cast<bool>(val) ? "true" : "false";
    } else if (val.type() == typeid(std::string)) {
         std::string s = std::any_cast<std::string>(val);
         return serializeRuntimeValue(RuntimeValue(s)); // Reuse escape logic
    } else if (val.type() == typeid(std::shared_ptr<ClassInstance>)) {
        return serializeRuntimeValue(RuntimeValue(std::any_cast<std::shared_ptr<ClassInstance>>(val)));
    } else if (val.type() == typeid(std::vector<int>)) {
        auto vec = std::any_cast<std::vector<int>>(val);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
        return ss.str();
    } else if (val.type() == typeid(std::vector<double>)) {
        auto vec = std::any_cast<std::vector<double>>(val);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
        return ss.str();
    } else if (val.type() == typeid(std::vector<std::string>)) {
        auto vec = std::any_cast<std::vector<std::string>>(val);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << serializeRuntimeValue(RuntimeValue(vec[i]));
        }
        ss << "]";
        return ss.str();
    } else if (val.type() == typeid(std::vector<bool>)) {
        auto vec = std::any_cast<std::vector<bool>>(val); // vector<bool> is special, might fail if not careful but any_cast handles it?
        // std::vector<bool> is partial specialization, any_cast might work if it was stored as such
        // But usually interpreter stores vector<bool> as vector<char> or generic.
        // Assuming vector<uint8_t> or similar for boolean arrays?
        // Let's assume standard vector<bool> if that's what Runtime stores.
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << (vec[i] ? "true" : "false");
        }
        ss << "]";
        return ss.str();
    }
     // Add support for vector<shared_ptr<ClassInstance>>
    else if (val.type() == typeid(std::vector<std::shared_ptr<ClassInstance>>)) {
        auto vec = std::any_cast<std::vector<std::shared_ptr<ClassInstance>>>(val);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
             ss << serializeRuntimeValue(RuntimeValue(vec[i]));
        }
        ss << "]";
        return ss.str();
    }
    
    return "null"; // Unknown type
}

void NativeRegistry::initJSON() {
    // Simple JSON parsing (basic implementation - would use nlohmann/json in production)
    registerFunction("json", "jsonParse", [](const std::vector<std::any>& args) -> std::any {
        std::string jsonStr = std::any_cast<std::string>(args[0]);
        // Simplified: Return the string as-is (real impl would parse to JsonValue)
        return jsonStr;
    });

    registerFunction("json", "jsonStringify", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::string("{}");
        return serializeAny(args[0]);
    }, FunctionSignature{{"any"}, "string"});

    registerFunction("json", "jsonStringifyPretty", [](const std::vector<std::any>& args) -> std::any {
        std::string indent = args.size() > 1 ? std::any_cast<std::string>(args[1]) : "  ";
        return std::string("{\n}");
    }, FunctionSignature{{"any", "string"}, "string"});

    registerFunction("json", "jsonUnmarshal", [](const std::vector<std::any>& args) -> std::any {
        std::string jsonStr = std::any_cast<std::string>(args[0]);
        // Reconstruct prototype from std::any (if it's a ClassInstance)
        RuntimeValue prototype;
        if (args.size() > 1 && args[1].type() == typeid(std::shared_ptr<ClassInstance>)) {
             prototype = RuntimeValue(std::any_cast<std::shared_ptr<ClassInstance>>(args[1]));
        } else {
             // Default generic object
             prototype = RuntimeValue(std::make_shared<ClassInstance>());
        }

        SimpleJsonParser parser(jsonStr);
        RuntimeValue result = parser.parse(prototype);
        
        // Convert back to std::any
        if (result.type == "int") return result.asInt();
        if (result.type == "double") return result.asDouble();
        if (result.type == "bool") return result.asBool();
        if (result.type == "string") return result.asString();
        if (result.type == "object") return result.asObject();
        return std::shared_ptr<ClassInstance>();
    }, FunctionSignature{{"string", "any"}, "any"});
}


// ============================================================================
// YAML Module Native Functions
// ============================================================================

std::string serializeToYaml(const std::any& val, int indentLevel = 0);

std::string serializeToYamlRuntimeValue(const RuntimeValue& val, int indentLevel) {
    std::string indent(indentLevel * 2, ' ');
    
    if (val.type == "int") {
        return std::to_string(val.asInt());
    } else if (val.type == "double") {
        return std::to_string(val.asDouble());
    } else if (val.type == "bool") {
        return val.asBool() ? "true" : "false";
    } else if (val.type == "string") {
        return "\"" + val.asString() + "\""; // Simple quoting
    } else if (val.type == "object") {
        auto instance = val.asObject();
        std::stringstream ss;
        if (instance->fields.empty()) return "{}";
        
        // If nested (indentLevel > 0), prepend logical newline if needed?
        // YAML is sensitive.
        // For simple struct dump:
        bool first = true;
        for (const auto& [name, field] : instance->fields) {
            if (!first) ss << "\n" << indent;
            else if (indentLevel > 0) ss << "\n" << indent; // First field of nested object needs newline+indent
            
            ss << name << ": ";
            
            // If field is object or array, handle nesting
            if (field.type == "object" || field.type.starts_with("array")) {
                 // But wait, if it's object, we need to defer newline?
                 // Or just let recursive call handle it?
                 // Simple recursive approach:
                 // key: 
                 //   field...
                 // unless it's empty
                 
                 // This simple serializer assumes flow style for some things or block style?
                 // Let's implement block style for structs
                 
                 // If value is complex:
                 if (field.type == "object" && !field.asObject()->fields.empty()) {
                     ss << serializeToYamlRuntimeValue(field, indentLevel + 1);
                 } else if (field.type.starts_with("array")) { // Simple array check
                     ss << serializeToYamlRuntimeValue(field, indentLevel);
                 } else {
                     ss << serializeToYamlRuntimeValue(field, 0); // Primitive, no indent change
                 }
            } else {
                 ss << serializeToYamlRuntimeValue(field, 0);
            }
            first = false;
        }
        return ss.str();
    } 
    // Simplified: Arrays not implemented fully for deep nesting here to save time/complexity
    // But for struct fields conversion it's useful.
    // Let's fallback to JSON-like representation for arrays for now or basic list
    
    return serializeAny(std::any()); // Fallback to JSON-like or null
}

std::string serializeToYamlAny(const std::any& val, int indentLevel) {
     if (val.type() == typeid(int)) return std::to_string(std::any_cast<int>(val));
     // ... primitives ...
     // Delegate to serializedRuntimeValue if possible or duplicate logic
     // Duplicating logic is easier for now
     if (val.type() == typeid(std::shared_ptr<ClassInstance>)) {
         auto instance = std::any_cast<std::shared_ptr<ClassInstance>>(val);
         std::stringstream ss;
         if (instance->fields.empty()) return "{}";
         
         std::string indent(indentLevel * 2, ' ');
         bool first = true;
         for (const auto& [name, field] : instance->fields) {
             if (!first) ss << "\n" << indent;
             else if (indentLevel > 0) {} // Start inline? No, usually block starts on new line 
             
             ss << name << ": ";
             if (field.type == "object") {
                  ss << "\n" << indent << "  " << serializeToYamlRuntimeValue(field, indentLevel + 1);
             } else if (field.type.starts_with("array")) {
                  ss << "\n" << indent << "  " << serializeToYamlRuntimeValue(field, indentLevel + 1);
             } else {
                  ss << serializeToYamlRuntimeValue(field, 0);
             }
             first = false;
         }
         return ss.str();
     }
     // Arrays
     if (val.type() == typeid(std::vector<std::string>)) {
         auto vec = std::any_cast<std::vector<std::string>>(val);
         std::stringstream ss;
         std::string indent(indentLevel * 2, ' ');
         for(auto& s : vec) {
             ss << "\n" << indent << "- \"" << s << "\"";
         }
         return ss.str();
     }

     return serializeAny(val); // Fallback to JSON
}

void NativeRegistry::initYAML() {
    registerFunction("yaml", "yamlStringify", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::string("");
        // Use serializeToYamlAny logic (simplified implementation)
        return serializeToYamlAny(args[0], 0);
    }, FunctionSignature{{"any"}, "string"});
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

    // Generate UUID v4
    registerFunction("crypto", "uuid", [](const std::vector<std::any>& args) -> std::any {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        // Generate 16 random bytes
        uint8_t bytes[16];
        for (int i = 0; i < 16; i++) {
            bytes[i] = static_cast<uint8_t>(dis(gen));
        }

        // Set version to 4 (random UUID)
        bytes[6] = (bytes[6] & 0x0f) | 0x40;

        // Set variant to RFC 4122
        bytes[8] = (bytes[8] & 0x3f) | 0x80;

        // Format as UUID string (xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx)
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < 16; i++) {
            if (i == 4 || i == 6 || i == 8 || i == 10) {
                ss << '-';
            }
            ss << std::setw(2) << static_cast<int>(bytes[i]);
        }

        return ss.str();
    }, FunctionSignature{{}, "string"});

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

// ============================================================================
// Terminal Module Native Functions
// ============================================================================

void NativeRegistry::initTerminal() {
    // Terminal state for raw mode
    static bool rawModeEnabled = false;

#ifdef _WIN32
    static HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    static HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    static DWORD originalMode = 0;
#else
    static struct termios originalTermios;
#endif

    // terminal.clear - Clear the entire screen
    registerFunction("terminal", "clear", [](const std::vector<std::any>& args) -> std::any {
        std::cout << "\x1b[2J\x1b[H" << std::flush;
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.clearLine - Clear from cursor to end of line
    registerFunction("terminal", "clearLine", [](const std::vector<std::any>& args) -> std::any {
        std::cout << "\x1b[K" << std::flush;
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.clearToBottom - Clear from cursor to end of screen
    registerFunction("terminal", "clearToBottom", [](const std::vector<std::any>& args) -> std::any {
        std::cout << "\x1b[J" << std::flush;
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.clearToTop - Clear from cursor to start of screen
    registerFunction("terminal", "clearToTop", [](const std::vector<std::any>& args) -> std::any {
        std::cout << "\x1b[1J" << std::flush;
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.moveCursor - Move cursor to specific position (1-based)
    registerFunction("terminal", "moveCursor", [](const std::vector<std::any>& args) -> std::any {
        int row = std::any_cast<int>(args[0]);
        int col = std::any_cast<int>(args[1]);
        std::cout << "\x1b[" << row << ";" << col << "H" << std::flush;
        return std::any();
    }, FunctionSignature{{"int", "int"}, "void"});

    // terminal.cursorUp - Move cursor up by n lines
    registerFunction("terminal", "cursorUp", [](const std::vector<std::any>& args) -> std::any {
        int n = std::any_cast<int>(args[0]);
        std::cout << "\x1b[" << n << "A" << std::flush;
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // terminal.cursorDown - Move cursor down by n lines
    registerFunction("terminal", "cursorDown", [](const std::vector<std::any>& args) -> std::any {
        int n = std::any_cast<int>(args[0]);
        std::cout << "\x1b[" << n << "B" << std::flush;
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // terminal.cursorLeft - Move cursor left by n columns
    registerFunction("terminal", "cursorLeft", [](const std::vector<std::any>& args) -> std::any {
        int n = std::any_cast<int>(args[0]);
        std::cout << "\x1b[" << n << "D" << std::flush;
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // terminal.cursorRight - Move cursor right by n columns
    registerFunction("terminal", "cursorRight", [](const std::vector<std::any>& args) -> std::any {
        int n = std::any_cast<int>(args[0]);
        std::cout << "\x1b[" << n << "C" << std::flush;
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // terminal.saveCursor - Save cursor position
    registerFunction("terminal", "saveCursor", [](const std::vector<std::any>& args) -> std::any {
        std::cout << "\x1b[s" << std::flush;
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.restoreCursor - Restore cursor position
    registerFunction("terminal", "restoreCursor", [](const std::vector<std::any>& args) -> std::any {
        std::cout << "\x1b[u" << std::flush;
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.hideCursor - Hide cursor
    registerFunction("terminal", "hideCursor", [](const std::vector<std::any>& args) -> std::any {
        std::cout << "\x1b[?25l" << std::flush;
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.showCursor - Show cursor
    registerFunction("terminal", "showCursor", [](const std::vector<std::any>& args) -> std::any {
        std::cout << "\x1b[?25h" << std::flush;
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.setForeground - Set foreground color
    registerFunction("terminal", "setForeground", [](const std::vector<std::any>& args) -> std::any {
        int color = std::any_cast<int>(args[0]);

        // Map color enum to ANSI code
        int code = 39; // Default
        if (color >= 0 && color <= 7) code = 30 + color;       // Normal colors
        else if (color >= 8 && color <= 15) code = 90 + (color - 8);  // Bright colors

        std::cout << "\x1b[" << code << "m" << std::flush;
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // terminal.setBackground - Set background color
    registerFunction("terminal", "setBackground", [](const std::vector<std::any>& args) -> std::any {
        int color = std::any_cast<int>(args[0]);

        // Map color enum to ANSI code
        int code = 49; // Default
        if (color >= 0 && color <= 7) code = 40 + color;       // Normal colors
        else if (color >= 8 && color <= 15) code = 100 + (color - 8);  // Bright colors

        std::cout << "\x1b[" << code << "m" << std::flush;
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // terminal.setForegroundRGB - Set foreground color using RGB
    registerFunction("terminal", "setForegroundRGB", [](const std::vector<std::any>& args) -> std::any {
        int r = std::any_cast<int>(args[0]);
        int g = std::any_cast<int>(args[1]);
        int b = std::any_cast<int>(args[2]);
        std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m" << std::flush;
        return std::any();
    }, FunctionSignature{{"int", "int", "int"}, "void"});

    // terminal.setBackgroundRGB - Set background color using RGB
    registerFunction("terminal", "setBackgroundRGB", [](const std::vector<std::any>& args) -> std::any {
        int r = std::any_cast<int>(args[0]);
        int g = std::any_cast<int>(args[1]);
        int b = std::any_cast<int>(args[2]);
        std::cout << "\x1b[48;2;" << r << ";" << g << ";" << b << "m" << std::flush;
        return std::any();
    }, FunctionSignature{{"int", "int", "int"}, "void"});

    // terminal.setStyle - Set text style
    registerFunction("terminal", "setStyle", [](const std::vector<std::any>& args) -> std::any {
        int style = std::any_cast<int>(args[0]);

        // Style codes: 0=reset, 1=bold, 2=dim, 3=italic, 4=underline, 5=blink, 7=reverse, 8=hidden, 9=strikethrough
        std::cout << "\x1b[" << style << "m" << std::flush;
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // terminal.reset - Reset all styles and colors
    registerFunction("terminal", "reset", [](const std::vector<std::any>& args) -> std::any {
        std::cout << "\x1b[0m" << std::flush;
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.getSize - Get terminal size
    registerFunction("terminal", "getSize", [](const std::vector<std::any>& args) -> std::any {
        int width = 80;
        int height = 24;

#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        }
#else
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
            width = w.ws_col;
            height = w.ws_row;
        }
#endif

        // Return as a simple array-like structure (for now, just return width)
        // TODO: Return proper TerminalSize struct when struct support is ready
        return std::any(width);
    }, FunctionSignature{{}, "int"});

    // terminal.getWidth - Get terminal width
    registerFunction("terminal", "getWidth", [](const std::vector<std::any>& args) -> std::any {
        int width = 80;

#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        }
#else
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
            width = w.ws_col;
        }
#endif

        return std::any(width);
    }, FunctionSignature{{}, "int"});

    // terminal.getHeight - Get terminal height
    registerFunction("terminal", "getHeight", [](const std::vector<std::any>& args) -> std::any {
        int height = 24;

#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        }
#else
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
            height = w.ws_row;
        }
#endif

        return std::any(height);
    }, FunctionSignature{{}, "int"});

    // terminal.supportsColor - Check if terminal supports colors
    registerFunction("terminal", "supportsColor", [](const std::vector<std::any>& args) -> std::any {
        // Check COLORTERM or TERM environment variables
        const char* colorterm = std::getenv("COLORTERM");
        const char* term = std::getenv("TERM");

        bool supportsColor = false;
        if (colorterm != nullptr) {
            supportsColor = true;
        } else if (term != nullptr) {
            std::string termStr(term);
            supportsColor = (termStr.find("color") != std::string::npos ||
                           termStr.find("xterm") != std::string::npos ||
                           termStr.find("screen") != std::string::npos);
        }

        return std::any(supportsColor ? 1 : 0);
    }, FunctionSignature{{}, "bool"});

    // terminal.isTTY - Check if running in a TTY
    registerFunction("terminal", "isTTY", [](const std::vector<std::any>& args) -> std::any {
#ifdef _WIN32
        bool isTty = _isatty(_fileno(stdout)) != 0;
#else
        bool isTty = isatty(STDOUT_FILENO) != 0;
#endif
        return std::any(isTty ? 1 : 0);
    }, FunctionSignature{{}, "bool"});

    // terminal.enableRawMode - Enable raw mode
    registerFunction("terminal", "enableRawMode", [](const std::vector<std::any>& args) -> std::any {
#ifdef _WIN32
        if (!rawModeEnabled) {
            GetConsoleMode(hStdin, &originalMode);
            DWORD mode = originalMode;
            mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
            mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
            SetConsoleMode(hStdin, mode);
            rawModeEnabled = true;
        }
#else
        if (!rawModeEnabled) {
            tcgetattr(STDIN_FILENO, &originalTermios);
            struct termios raw = originalTermios;
            raw.c_lflag &= ~(ECHO | ICANON);
            raw.c_cc[VMIN] = 0;
            raw.c_cc[VTIME] = 1;
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
            rawModeEnabled = true;
        }
#endif
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.disableRawMode - Disable raw mode
    registerFunction("terminal", "disableRawMode", [](const std::vector<std::any>& args) -> std::any {
#ifdef _WIN32
        if (rawModeEnabled) {
            SetConsoleMode(hStdin, originalMode);
            rawModeEnabled = false;
        }
#else
        if (rawModeEnabled) {
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios);
            rawModeEnabled = false;
        }
#endif
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.readChar - Read a single character
    registerFunction("terminal", "readChar", [](const std::vector<std::any>& args) -> std::any {
        char c = 0;
#ifdef _WIN32
        if (_kbhit()) {
            c = _getch();
        }
#else
        if (read(STDIN_FILENO, &c, 1) != 1) {
            c = 0;
        }
#endif
        return std::any(std::string(1, c));
    }, FunctionSignature{{}, "string"});

    // terminal.readLine - Read a line of input
    registerFunction("terminal", "readLine", [](const std::vector<std::any>& args) -> std::any {
        std::string line;
        std::getline(std::cin, line);
        return std::any(line);
    }, FunctionSignature{{}, "string"});

    // terminal.input - Read input with a prompt
    registerFunction("terminal", "input", [](const std::vector<std::any>& args) -> std::any {
        std::string prompt = std::any_cast<std::string>(args[0]);
        std::cout << prompt << std::flush;
        std::string line;
        std::getline(std::cin, line);
        return std::any(line);
    }, FunctionSignature{{"string"}, "string"});

    // terminal.hasInput - Check if input is available (non-blocking)
    registerFunction("terminal", "hasInput", [](const std::vector<std::any>& args) -> std::any {
#ifdef _WIN32
        bool hasInput = _kbhit() != 0;
#else
        // Use select to check if input is available
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        struct timeval timeout = {0, 0};
        bool hasInput = select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout) > 0;
#endif
        return std::any(hasInput ? 1 : 0);
    }, FunctionSignature{{}, "bool"});

    // terminal.useAlternateScreen - Switch to alternate screen buffer
    registerFunction("terminal", "useAlternateScreen", [](const std::vector<std::any>& args) -> std::any {
        std::cout << "\x1b[?1049h" << std::flush;
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.useMainScreen - Switch back to main screen buffer
    registerFunction("terminal", "useMainScreen", [](const std::vector<std::any>& args) -> std::any {
        std::cout << "\x1b[?1049l" << std::flush;
        return std::any();
    }, FunctionSignature{{}, "void"});

    // terminal.readKey - Read a single key press (handles escape sequences)
    // Returns a simplified int code for the key
    registerFunction("terminal", "readKey", [](const std::vector<std::any>& args) -> std::any {
        char c = 0;

#ifdef _WIN32
        if (_kbhit()) {
            c = _getch();

            // Handle special keys
            if (c == 0 || c == 0xE0) {
                c = _getch();
                switch (c) {
                    case 72: return std::any(1001); // Up arrow
                    case 80: return std::any(1002); // Down arrow
                    case 75: return std::any(1003); // Left arrow
                    case 77: return std::any(1004); // Right arrow
                    case 83: return std::any(1005); // Delete
                    case 71: return std::any(1006); // Home
                    case 79: return std::any(1007); // End
                    case 73: return std::any(1008); // Page Up
                    case 81: return std::any(1009); // Page Down
                    case 59: return std::any(1010); // F1
                    case 60: return std::any(1011); // F2
                    case 61: return std::any(1012); // F3
                    case 62: return std::any(1013); // F4
                    case 63: return std::any(1014); // F5
                    case 64: return std::any(1015); // F6
                    case 65: return std::any(1016); // F7
                    case 66: return std::any(1017); // F8
                    case 67: return std::any(1018); // F9
                    case 68: return std::any(1019); // F10
                }
            }
        }
#else
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == 27) { // ESC sequence
                char seq[3];
                if (read(STDIN_FILENO, &seq[0], 1) == 1) {
                    if (seq[0] == '[') {
                        if (read(STDIN_FILENO, &seq[1], 1) == 1) {
                            switch (seq[1]) {
                                case 'A': return std::any(1001); // Up arrow
                                case 'B': return std::any(1002); // Down arrow
                                case 'D': return std::any(1003); // Left arrow
                                case 'C': return std::any(1004); // Right arrow
                                case 'H': return std::any(1006); // Home
                                case 'F': return std::any(1007); // End
                                case '3': // Delete
                                    if (read(STDIN_FILENO, &seq[2], 1) == 1 && seq[2] == '~') {
                                        return std::any(1005);
                                    }
                                    break;
                                case '5': // Page Up
                                    if (read(STDIN_FILENO, &seq[2], 1) == 1 && seq[2] == '~') {
                                        return std::any(1008);
                                    }
                                    break;
                                case '6': // Page Down
                                    if (read(STDIN_FILENO, &seq[2], 1) == 1 && seq[2] == '~') {
                                        return std::any(1009);
                                    }
                                    break;
                            }
                        }
                    } else if (seq[0] == 'O') {
                        if (read(STDIN_FILENO, &seq[1], 1) == 1) {
                            switch (seq[1]) {
                                case 'P': return std::any(1010); // F1
                                case 'Q': return std::any(1011); // F2
                                case 'R': return std::any(1012); // F3
                                case 'S': return std::any(1013); // F4
                            }
                        }
                    }
                }
                return std::any(27); // ESC
            }
        }
#endif

        return std::any(static_cast<int>(c));
    }, FunctionSignature{{}, "int"});
}

// ============================================================================
// Regex Module Native Functions
// ============================================================================

void NativeRegistry::initRegex() {
    // regex.compile - Compile and validate a regex pattern
    registerFunction("regex", "compile", [](const std::vector<std::any>& args) -> std::any {
        std::string pattern = std::any_cast<std::string>(args[0]);

        try {
            std::regex re(pattern);
            return std::any(1);  // Valid
        } catch (const std::regex_error& e) {
            return std::any(0);  // Invalid
        }
    }, FunctionSignature{{"string"}, "int"});

    // regex.compileWithFlags - Compile with flags
    registerFunction("regex", "compileWithFlags", [](const std::vector<std::any>& args) -> std::any {
        std::string pattern = std::any_cast<std::string>(args[0]);
        int flags = std::any_cast<int>(args[1]);

        try {
            std::regex_constants::syntax_option_type regexFlags = std::regex_constants::ECMAScript;

            if (flags & 1) regexFlags |= std::regex_constants::icase;  // Case insensitive

            std::regex re(pattern, regexFlags);
            return std::any(1);  // Valid
        } catch (const std::regex_error& e) {
            return std::any(0);  // Invalid
        }
    }, FunctionSignature{{"string", "int"}, "int"});

    // regex.matches - Test if entire string matches pattern
    registerFunction("regex", "matches", [](const std::vector<std::any>& args) -> std::any {
        std::string pattern = std::any_cast<std::string>(args[0]);
        std::string text = std::any_cast<std::string>(args[1]);

        try {
            std::regex re(pattern);
            bool result = std::regex_match(text, re);
            return std::any(result ? 1 : 0);
        } catch (const std::regex_error& e) {
            return std::any(0);  // Return false on error
        }
    }, FunctionSignature{{"string", "string"}, "bool"});

    // regex.contains - Test if pattern is found anywhere in string
    registerFunction("regex", "contains", [](const std::vector<std::any>& args) -> std::any {
        std::string pattern = std::any_cast<std::string>(args[0]);
        std::string text = std::any_cast<std::string>(args[1]);

        try {
            std::regex re(pattern);
            bool result = std::regex_search(text, re);
            return std::any(result ? 1 : 0);
        } catch (const std::regex_error& e) {
            return std::any(0);
        }
    }, FunctionSignature{{"string", "string"}, "bool"});

    // regex.find - Find first match position
    registerFunction("regex", "find", [](const std::vector<std::any>& args) -> std::any {
        std::string pattern = std::any_cast<std::string>(args[0]);
        std::string text = std::any_cast<std::string>(args[1]);

        try {
            std::regex re(pattern);
            std::smatch match;

            if (std::regex_search(text, match, re)) {
                return std::any(static_cast<int>(match.position(0)));
            }

            return std::any(-1);  // Not found
        } catch (const std::regex_error& e) {
            return std::any(-1);
        }
    }, FunctionSignature{{"string", "string"}, "int"});

    // regex.replace - Replace first occurrence
    registerFunction("regex", "replace", [](const std::vector<std::any>& args) -> std::any {
        std::string pattern = std::any_cast<std::string>(args[0]);
        std::string text = std::any_cast<std::string>(args[1]);
        std::string replacement = std::any_cast<std::string>(args[2]);

        try {
            std::regex re(pattern);
            std::string result = std::regex_replace(text, re, replacement,
                std::regex_constants::format_first_only);
            return std::any(result);
        } catch (const std::regex_error& e) {
            return std::any(text);  // Return original on error
        }
    }, FunctionSignature{{"string", "string", "string"}, "string"});

    // regex.replaceAll - Replace all occurrences
    registerFunction("regex", "replaceAll", [](const std::vector<std::any>& args) -> std::any {
        std::string pattern = std::any_cast<std::string>(args[0]);
        std::string text = std::any_cast<std::string>(args[1]);
        std::string replacement = std::any_cast<std::string>(args[2]);

        try {
            std::regex re(pattern);
            std::string result = std::regex_replace(text, re, replacement);
            return std::any(result);
        } catch (const std::regex_error& e) {
            return std::any(text);
        }
    }, FunctionSignature{{"string", "string", "string"}, "string"});

    // regex.split - Split string by pattern
    registerFunction("regex", "split", [](const std::vector<std::any>& args) -> std::any {
        std::string pattern = std::any_cast<std::string>(args[0]);
        std::string text = std::any_cast<std::string>(args[1]);

        try {
            std::regex re(pattern);
            std::sregex_token_iterator iter(text.begin(), text.end(), re, -1);
            std::sregex_token_iterator end;

            // For now, we'll return the count of parts
            // TODO: Return actual array when array support is ready
            int count = 0;
            for (; iter != end; ++iter) {
                count++;
            }

            return std::any(count);
        } catch (const std::regex_error& e) {
            return std::any(1);  // Return 1 (original string) on error
        }
    }, FunctionSignature{{"string", "string"}, "int"});

    // regex.escape - Escape special regex characters
    registerFunction("regex", "escape", [](const std::vector<std::any>& args) -> std::any {
        std::string text = std::any_cast<std::string>(args[0]);
        std::string escaped;

        // Characters that need escaping in regex
        const std::string specialChars = ".^$*+?()[]{}|\\";

        for (char c : text) {
            if (specialChars.find(c) != std::string::npos) {
                escaped += '\\';
            }
            escaped += c;
        }

        return std::any(escaped);
    }, FunctionSignature{{"string"}, "string"});

    // regex.groups - Get capture groups from first match
    // Returns semicolon-separated string of groups
    registerFunction("regex", "groups", [](const std::vector<std::any>& args) -> std::any {
        std::string pattern = std::any_cast<std::string>(args[0]);
        std::string text = std::any_cast<std::string>(args[1]);

        try {
            std::regex re(pattern);
            std::smatch match;

            if (std::regex_search(text, match, re)) {
                std::string result;
                for (size_t i = 0; i < match.size(); ++i) {
                    if (i > 0) result += ";";
                    result += match[i].str();
                }
                return std::any(result);
            }

            return std::any(std::string(""));
        } catch (const std::regex_error& e) {
            return std::any(std::string(""));
        }
    }, FunctionSignature{{"string", "string"}, "string"});

    // regex.findAll - Find all match positions
    // Returns semicolon-separated positions
    registerFunction("regex", "findAll", [](const std::vector<std::any>& args) -> std::any {
        std::string pattern = std::any_cast<std::string>(args[0]);
        std::string text = std::any_cast<std::string>(args[1]);

        try {
            std::regex re(pattern);
            std::sregex_iterator iter(text.begin(), text.end(), re);
            std::sregex_iterator end;

            std::string positions;
            for (; iter != end; ++iter) {
                if (!positions.empty()) positions += ";";
                positions += std::to_string(iter->position(0));
            }

            return std::any(positions);
        } catch (const std::regex_error& e) {
            return std::any(std::string(""));
        }
    }, FunctionSignature{{"string", "string"}, "string"});

    // regex.matchAll - Get all matches with capture groups
    // Returns pipe-separated matches, with groups semicolon-separated
    registerFunction("regex", "matchAll", [](const std::vector<std::any>& args) -> std::any {
        std::string pattern = std::any_cast<std::string>(args[0]);
        std::string text = std::any_cast<std::string>(args[1]);

        try {
            std::regex re(pattern);
            std::sregex_iterator iter(text.begin(), text.end(), re);
            std::sregex_iterator end;

            std::string result;
            for (; iter != end; ++iter) {
                if (!result.empty()) result += "|";

                std::string matchGroups;
                for (size_t i = 0; i < iter->size(); ++i) {
                    if (i > 0) matchGroups += ";";
                    matchGroups += (*iter)[i].str();
                }
                result += matchGroups;
            }

            return std::any(result);
        } catch (const std::regex_error& e) {
            return std::any(std::string(""));
        }
    }, FunctionSignature{{"string", "string"}, "string"});

    // ========================================================================
    // MAPS MODULE - HashMap/Dictionary operations
    // ========================================================================

    // maps.create - Create a new empty map
    registerFunction("maps", "create", [](const std::vector<std::any>& args) -> std::any {
        return std::unordered_map<std::string, std::string>();
    }, FunctionSignature{{}, "map<string,string>"});

    // maps.set - Set a key-value pair in the map
    registerFunction("maps", "set", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        std::string key = std::any_cast<std::string>(args[1]);
        std::string value = std::any_cast<std::string>(args[2]);
        map[key] = value;
        return map;
    }, FunctionSignature{{"map<string,string>", "string", "string"}, "map<string,string>"});

    // maps.get - Get a value by key (returns empty string if not found)
    registerFunction("maps", "get", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        std::string key = std::any_cast<std::string>(args[1]);
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return std::string("");
    }, FunctionSignature{{"map<string,string>", "string"}, "string"});

    // maps.has - Check if a key exists in the map
    registerFunction("maps", "has", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        std::string key = std::any_cast<std::string>(args[1]);
        return map.find(key) != map.end();
    }, FunctionSignature{{"map<string,string>", "string"}, "bool"});

    // maps.remove - Remove a key from the map
    registerFunction("maps", "remove", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        std::string key = std::any_cast<std::string>(args[1]);
        map.erase(key);
        return map;
    }, FunctionSignature{{"map<string,string>", "string"}, "map<string,string>"});

    // maps.size - Get the number of entries in the map
    registerFunction("maps", "size", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        return static_cast<int>(map.size());
    }, FunctionSignature{{"map<string,string>"}, "int"});

    // maps.keys - Get all keys as an array
    registerFunction("maps", "keys", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        std::vector<std::string> keys;
        for (const auto& pair : map) {
            keys.push_back(pair.first);
        }
        return keys;
    }, FunctionSignature{{"map<string,string>"}, "array<string>"});

    // maps.values - Get all values as an array
    registerFunction("maps", "values", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        std::vector<std::string> values;
        for (const auto& pair : map) {
            values.push_back(pair.second);
        }
        return values;
    }, FunctionSignature{{"map<string,string>"}, "array<string>"});

    // maps.clear - Remove all entries from the map
    registerFunction("maps", "clear", [](const std::vector<std::any>& args) -> std::any {
        return std::unordered_map<std::string, std::string>();
    }, FunctionSignature{{"map<string,string>"}, "map<string,string>"});

    // maps.isEmpty - Check if the map is empty
    registerFunction("maps", "isEmpty", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        return map.empty();
    }, FunctionSignature{{"map<string,string>"}, "bool"});

    // maps.first - Get the first key in the map (useful for iteration)
    registerFunction("maps", "first", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        if (!map.empty()) {
            return map.begin()->first;
        }
        return std::string("");
    }, FunctionSignature{{"map<string,string>"}, "string"});

    // maps.last - Get the last key in the map (note: unordered, so "last" means any element)
    registerFunction("maps", "last", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        if (!map.empty()) {
            // For unordered_map, get the last element we can access
            auto it = map.begin();
            std::advance(it, map.size() - 1);
            return it->first;
        }
        return std::string("");
    }, FunctionSignature{{"map<string,string>"}, "string"});

    // maps.firstValue - Get the first value in the map
    registerFunction("maps", "firstValue", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        if (!map.empty()) {
            return map.begin()->second;
        }
        return std::string("");
    }, FunctionSignature{{"map<string,string>"}, "string"});

    // maps.lastValue - Get the last value in the map
    registerFunction("maps", "lastValue", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        if (!map.empty()) {
            auto it = map.begin();
            std::advance(it, map.size() - 1);
            return it->second;
        }
        return std::string("");
    }, FunctionSignature{{"map<string,string>"}, "string"});

    // maps.merge - Merge two maps (second map overwrites first on conflicts)
    registerFunction("maps", "merge", [](const std::vector<std::any>& args) -> std::any {
        auto map1 = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        auto map2 = std::any_cast<std::unordered_map<std::string, std::string>>(args[1]);
        
        // Merge map2 into map1
        for (const auto& pair : map2) {
            map1[pair.first] = pair.second;
        }
        return map1;
    }, FunctionSignature{{"map<string,string>", "map<string,string>"}, "map<string,string>"});

    // maps.entries - Get all entries as array of "key:value" strings
    registerFunction("maps", "entries", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        std::vector<std::string> entries;
        for (const auto& pair : map) {
            entries.push_back(pair.first + ":" + pair.second);
        }
        return entries;
    }, FunctionSignature{{"map<string,string>"}, "array<string>"});

    // maps.containsValue - Check if a value exists in the map
    registerFunction("maps", "containsValue", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        std::string value = std::any_cast<std::string>(args[1]);
        
        for (const auto& pair : map) {
            if (pair.second == value) {
                return true;
            }
        }
        return false;
    }, FunctionSignature{{"map<string,string>", "string"}, "bool"});

    // maps.fromEntries - Create a map from an array of "key:value" strings
    registerFunction("maps", "fromEntries", [](const std::vector<std::any>& args) -> std::any {
        auto entries = std::any_cast<std::vector<std::string>>(args[0]);
        std::unordered_map<std::string, std::string> map;
        
        for (const auto& entry : entries) {
            size_t colonPos = entry.find(':');
            if (colonPos != std::string::npos) {
                std::string key = entry.substr(0, colonPos);
                std::string value = entry.substr(colonPos + 1);
                map[key] = value;
            }
        }
        return map;
    }, FunctionSignature{{"array<string>"}, "map<string,string>"});
}



// ============================================================================
// SQLite Module - Database Support
// ============================================================================

void NativeRegistry::initSQLite() {
    // __sqlite_open(path: string): any
    registerFunction("db", "__sqlite_open", [](const std::vector<std::any>& args) -> std::any {
        auto path = std::any_cast<std::string>(args[0]);
        sqlite3* db = nullptr;

        int rc = sqlite3_open(path.c_str(), &db);
        if (rc != SQLITE_OK) {
            std::string error = "Failed to open database: " + std::string(sqlite3_errmsg(db));
            sqlite3_close(db);
            throw std::runtime_error(error);
        }

        // Return database handle as void pointer wrapped in any
        return std::any(reinterpret_cast<void*>(db));
    }, FunctionSignature{{"string"}, "any"});

    // __sqlite_close(handle: any): void
    registerFunction("db", "__sqlite_close", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        sqlite3* db = reinterpret_cast<sqlite3*>(handle);
        sqlite3_close(db);
        return std::any();
    }, FunctionSignature{{"any"}, "void"});

    // __sqlite_exec(handle: any, sql: string): int
    registerFunction("db", "__sqlite_exec", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        sqlite3* db = reinterpret_cast<sqlite3*>(handle);

        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

        if (rc != SQLITE_OK) {
            std::string error = "SQL error: " + std::string(errMsg);
            sqlite3_free(errMsg);
            throw std::runtime_error(error);
        }

        return std::any(sqlite3_changes(db));
    }, FunctionSignature{{"any", "string"}, "int"});

    // __sqlite_prepare(handle: any, sql: string): any
    registerFunction("db", "__sqlite_prepare", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        sqlite3* db = reinterpret_cast<sqlite3*>(handle);

        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::string error = "Failed to prepare statement: " + std::string(sqlite3_errmsg(db));
            throw std::runtime_error(error);
        }

        return std::any(reinterpret_cast<void*>(stmt));
    }, FunctionSignature{{"any", "string"}, "any"});

    // __sqlite_query(handle: any, sql: string): any
    registerFunction("db", "__sqlite_query", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        sqlite3* db = reinterpret_cast<sqlite3*>(handle);

        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::string error = "Failed to prepare query: " + std::string(sqlite3_errmsg(db));
            throw std::runtime_error(error);
        }

        return std::any(reinterpret_cast<void*>(stmt));
    }, FunctionSignature{{"any", "string"}, "any"});

    // __sqlite_bind_int(handle: any, index: int, value: int): void
    registerFunction("db", "__sqlite_bind_int", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        auto value = std::any_cast<int>(args[2]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        int rc = sqlite3_bind_int(stmt, index, value);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to bind int parameter");
        }
        return std::any();
    }, FunctionSignature{{"any", "int", "int"}, "void"});

    // __sqlite_bind_double(handle: any, index: int, value: double): void
    registerFunction("db", "__sqlite_bind_double", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        auto value = std::any_cast<double>(args[2]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        int rc = sqlite3_bind_double(stmt, index, value);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to bind double parameter");
        }
        return std::any();
    }, FunctionSignature{{"any", "int", "double"}, "void"});

    // __sqlite_bind_string(handle: any, index: int, value: string): void
    registerFunction("db", "__sqlite_bind_string", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        auto value = std::any_cast<std::string>(args[2]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        int rc = sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to bind string parameter");
        }
        return std::any();
    }, FunctionSignature{{"any", "int", "string"}, "void"});

    // __sqlite_bind_null(handle: any, index: int): void
    registerFunction("db", "__sqlite_bind_null", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        int rc = sqlite3_bind_null(stmt, index);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to bind null parameter");
        }
        return std::any();
    }, FunctionSignature{{"any", "int"}, "void"});

    // __sqlite_bind(handle: any, index: int, value: any): void
    registerFunction("db", "__sqlite_bind", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        auto& value = args[2];
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        int rc = SQLITE_OK;

        // Try to bind based on the type of value
        if (value.type() == typeid(int)) {
            rc = sqlite3_bind_int(stmt, index, std::any_cast<int>(value));
        } else if (value.type() == typeid(double)) {
            rc = sqlite3_bind_double(stmt, index, std::any_cast<double>(value));
        } else if (value.type() == typeid(std::string)) {
            auto str = std::any_cast<std::string>(value);
            rc = sqlite3_bind_text(stmt, index, str.c_str(), -1, SQLITE_TRANSIENT);
        } else if (value.type() == typeid(bool)) {
            rc = sqlite3_bind_int(stmt, index, std::any_cast<bool>(value) ? 1 : 0);
        } else if (!value.has_value()) {
            rc = sqlite3_bind_null(stmt, index);
        } else {
            throw std::runtime_error("Unsupported type for bind parameter");
        }

        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to bind parameter");
        }
        return std::any();
    }, FunctionSignature{{"any", "int", "any"}, "void"});

    // __sqlite_stmt_exec(handle: any): int
    registerFunction("db", "__sqlite_stmt_exec", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        int rc = sqlite3_step(stmt);
        sqlite3_reset(stmt);

        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
            throw std::runtime_error("Failed to execute statement");
        }

        sqlite3* db = sqlite3_db_handle(stmt);
        return std::any(sqlite3_changes(db));
    }, FunctionSignature{{"any"}, "int"});

    // __sqlite_step(handle: any): bool
    registerFunction("db", "__sqlite_step", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        int rc = sqlite3_step(stmt);
        return std::any(rc == SQLITE_ROW);
    }, FunctionSignature{{"any"}, "bool"});

    // __sqlite_column_int(handle: any, index: int): int
    registerFunction("db", "__sqlite_column_int", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        return std::any(sqlite3_column_int(stmt, index));
    }, FunctionSignature{{"any", "int"}, "int"});

    // __sqlite_column_double(handle: any, index: int): double
    registerFunction("db", "__sqlite_column_double", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        return std::any(sqlite3_column_double(stmt, index));
    }, FunctionSignature{{"any", "int"}, "double"});

    // __sqlite_column_string(handle: any, index: int): string
    registerFunction("db", "__sqlite_column_string", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        const unsigned char* text = sqlite3_column_text(stmt, index);
        if (text) {
            return std::any(std::string(reinterpret_cast<const char*>(text)));
        }
        return std::any(std::string(""));
    }, FunctionSignature{{"any", "int"}, "string"});

    // __sqlite_column_index(handle: any, name: string): int
    registerFunction("db", "__sqlite_column_index", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto name = std::any_cast<std::string>(args[1]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        int columnCount = sqlite3_column_count(stmt);
        for (int i = 0; i < columnCount; i++) {
            const char* columnName = sqlite3_column_name(stmt, i);
            if (columnName && name == columnName) {
                return std::any(i);
            }
        }

        throw std::runtime_error("Column not found: " + name);
    }, FunctionSignature{{"any", "string"}, "int"});

    // __sqlite_column_is_null(handle: any, index: int): bool
    registerFunction("db", "__sqlite_column_is_null", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        return std::any(sqlite3_column_type(stmt, index) == SQLITE_NULL);
    }, FunctionSignature{{"any", "int"}, "bool"});

    // __sqlite_column_count(handle: any): int
    registerFunction("db", "__sqlite_column_count", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        return std::any(sqlite3_column_count(stmt));
    }, FunctionSignature{{"any"}, "int"});

    // __sqlite_column_name(handle: any, index: int): string
    registerFunction("db", "__sqlite_column_name", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);

        const char* name = sqlite3_column_name(stmt, index);
        if (name) {
            return std::any(std::string(name));
        }
        return std::any(std::string(""));
    }, FunctionSignature{{"any", "int"}, "string"});

    // __sqlite_stmt_reset(handle: any): void
    registerFunction("db", "__sqlite_stmt_reset", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);
        sqlite3_reset(stmt);
        return std::any();
    }, FunctionSignature{{"any"}, "void"});

    // __sqlite_stmt_close(handle: any): void
    registerFunction("db", "__sqlite_stmt_close", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);
        sqlite3_finalize(stmt);
        return std::any();
    }, FunctionSignature{{"any"}, "void"});

    // __sqlite_rows_close(handle: any): void
    registerFunction("db", "__sqlite_rows_close", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(handle);
        sqlite3_finalize(stmt);
        return std::any();
    }, FunctionSignature{{"any"}, "void"});

    // __sqlite_last_insert_id(handle: any): int
    registerFunction("db", "__sqlite_last_insert_id", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        sqlite3* db = reinterpret_cast<sqlite3*>(handle);

        return std::any(static_cast<int>(sqlite3_last_insert_rowid(db)));
    }, FunctionSignature{{"any"}, "int"});

    // __sqlite_changes(handle: any): int
    registerFunction("db", "__sqlite_changes", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        sqlite3* db = reinterpret_cast<sqlite3*>(handle);

        return std::any(sqlite3_changes(db));
    }, FunctionSignature{{"any"}, "int"});
}
}  // namespace stratos
