#include "stratos/NativeRegistry.h"
#include "stratos/Logger.h"
#include "stratos/FFI.h"
#include "stratos/WebSocket.h"
#include "stratos/HttpServer.h"
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
#include <libpq-fe.h>
#include <mysql/mysql.h>
#include <hiredis/hiredis.h>
#include "stratos/Interpreter.h"
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <future>

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
    initPostgreSQL(); // PostgreSQL database support
    initMySQL();     // MySQL/MariaDB database support
    initRedis();     // Redis support
    initXML();       // XML parsing support
    initTemplate();  // HTML templating support
    initCollections(); // Collections module
    initConcurrent(); // Concurrency primitives
    initHTTP();      // HTTP server and client
    initAsync();     // Async/await primitives
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
    }, FunctionSignature{{}, "map<string, any>"});

    // mapOf<K, V>(entries) - Create map from entries
    registerFunction("collections", "mapOf", [](const std::vector<std::any>& args) -> std::any {
        std::unordered_map<std::string, std::any> map;
        // Logic to populate map from entries would go here
        // For now return empty or simple implementation if args provided
        // This is complex as entries is Array<Pair<K,V>> which is List<Object>...
        return map;
    }, FunctionSignature{{"Array<Pair<K, V>>"}, "map<K, V>"});

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

    // Alias for charAt - used when string[index] or string.get(index) is called
    registerFunction("strings", "get", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 2) {
            return std::string("");
        }
        try {
            std::string s = std::any_cast<std::string>(args[0]);
            int index;
            // Handle both int and double index types
            try {
                index = std::any_cast<int>(args[1]);
            } catch (const std::bad_any_cast&) {
                try {
                    index = static_cast<int>(std::any_cast<double>(args[1]));
                } catch (const std::bad_any_cast&) {
                    return std::string("");
                }
            }
            if (index >= 0 && index < static_cast<int>(s.length())) {
                return std::string(1, s[index]);
            }
        } catch (const std::bad_any_cast&) {
            // Return empty string on type mismatch
        }
        return std::string("");
    }, FunctionSignature{{"string", "int"}, "string"});

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
// Helper Functions for Result Types
// ============================================================================

// Helper to create io::Result<T, Error> class instances
template<typename T>
std::shared_ptr<ClassInstance> createOkResult(const T& value, const std::string& typeName) {
    auto result = std::make_shared<ClassInstance>();
    result->className = "io::Result";

    // Set the value field
    if constexpr (std::is_same_v<T, std::string>) {
        result->fields["value"] = RuntimeValue(value, "string");
    } else if constexpr (std::is_same_v<T, int>) {
        result->fields["value"] = RuntimeValue(value, "int");
    } else if constexpr (std::is_same_v<T, bool>) {
        result->fields["value"] = RuntimeValue(value, "bool");
    } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
        result->fields["value"] = RuntimeValue(std::any(value), "array<byte>");
    } else if constexpr (std::is_same_v<T, std::shared_ptr<ClassInstance>>) {
        result->fields["value"] = RuntimeValue(value, "object");
    } else {
        result->fields["value"] = RuntimeValue(std::any(value), typeName);
    }

    // Set the error field (null/empty for Ok)
    auto errorObj = std::make_shared<ClassInstance>();
    errorObj->className = "io::Error";
    errorObj->fields["message"] = RuntimeValue(std::string(""), "string");
    result->fields["error"] = RuntimeValue(errorObj, "object");

    // Set isOk flag
    result->fields["isOk"] = RuntimeValue(true, "bool");

    return result;
}

std::shared_ptr<ClassInstance> createErrResult(const std::string& errorMessage) {
    auto result = std::make_shared<ClassInstance>();
    result->className = "io::Result";

    // Set the value field (empty for Err)
    result->fields["value"] = RuntimeValue(std::any(), "any");

    // Set the error field
    auto errorObj = std::make_shared<ClassInstance>();
    errorObj->className = "io::Error";
    errorObj->fields["message"] = RuntimeValue(errorMessage, "string");
    result->fields["error"] = RuntimeValue(errorObj, "object");

    // Set isOk flag
    result->fields["isOk"] = RuntimeValue(false, "bool");

    return result;
}

// ============================================================================
// Optional Type Helpers
// ============================================================================

template<typename T>
std::shared_ptr<ClassInstance> createSome(const T& value, const std::string& typeName = "any") {
    auto optional = std::make_shared<ClassInstance>();
    optional->className = "Optional<" + typeName + ">";

    optional->fields["hasValue"] = RuntimeValue(true, "bool");

    if constexpr (std::is_same_v<T, std::string>) {
        optional->fields["value"] = RuntimeValue(value, "string");
    } else if constexpr (std::is_same_v<T, int>) {
        optional->fields["value"] = RuntimeValue(value, "int");
    } else if constexpr (std::is_same_v<T, bool>) {
        optional->fields["value"] = RuntimeValue(value, "bool");
    } else if constexpr (std::is_same_v<T, double>) {
        optional->fields["value"] = RuntimeValue(value, "double");
    } else if constexpr (std::is_same_v<T, std::shared_ptr<ClassInstance>>) {
        optional->fields["value"] = RuntimeValue(value, "object");
    } else {
        optional->fields["value"] = RuntimeValue(std::any(value), typeName);
    }

    return optional;
}

std::shared_ptr<ClassInstance> createNone(const std::string& typeName = "any") {
    auto optional = std::make_shared<ClassInstance>();
    optional->className = "Optional<" + typeName + ">";

    optional->fields["hasValue"] = RuntimeValue(false, "bool");
    optional->fields["value"] = RuntimeValue(std::any(), "any");

    return optional;
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
            return createErrResult("Failed to open file: " + path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return createOkResult(buffer.str(), "string");
    }, FunctionSignature{{"string"}, "Result<string, Error>"});

    registerFunction("io", "readBytes", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return createErrResult("Failed to open file: " + path);
        }
        std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
        return createOkResult(bytes, "array<byte>");
    }, FunctionSignature{{"string"}, "Result<Array<byte>, Error>"});

    registerFunction("io", "readLines", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        std::ifstream file(path);
        if (!file.is_open()) {
            return std::vector<std::string>();
        }
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        return lines;
    }, FunctionSignature{{"string"}, "array<string>"});

    // File writing
    registerFunction("io", "writeFile", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        std::string content = std::any_cast<std::string>(args[1]);
        std::ofstream file(path);
        if (!file.is_open()) {
            return createErrResult("Failed to write file: " + path);
        }
        file << content;
        return createOkResult(true, "bool");
    }, FunctionSignature{{"string", "string"}, "Result<bool, Error>"});

    registerFunction("io", "appendFile", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        std::string content = std::any_cast<std::string>(args[1]);
        std::ofstream file(path, std::ios::app);
        if (!file.is_open()) {
            return createErrResult("Failed to append to file: " + path);
        }
        file << content;
        return createOkResult(true, "bool");
    }, FunctionSignature{{"string", "string"}, "Result<bool, Error>"});

    registerFunction("io", "appendLine", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        std::string content = std::any_cast<std::string>(args[1]);
        std::ofstream file(path, std::ios::app);
        if (!file.is_open()) {
            return createErrResult("Failed to append to file: " + path);
        }
        file << content << "\n";
        return createOkResult(true, "bool");
    }, FunctionSignature{{"string", "string"}, "Result<bool, Error>"});

    registerFunction("io", "writeBytes", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        auto bytes = std::any_cast<std::vector<uint8_t>>(args[1]);
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return createErrResult("Failed to write bytes to file: " + path);
        }
        file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        return createOkResult(true, "bool");
    }, FunctionSignature{{"string", "Array<byte>"}, "Result<bool, Error>"});

    // File operations
    registerFunction("io", "remove", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        try {
            bool success = fs::remove(path);
            if (!success) {
                return createErrResult("File not found: " + path);
            }
            return createOkResult(true, "bool");
        } catch (const std::exception& e) {
            return createErrResult(std::string("Failed to remove file: ") + e.what());
        }
    }, FunctionSignature{{"string"}, "Result<bool, Error>"});

    registerFunction("io", "rename", [](const std::vector<std::any>& args) -> std::any {
        std::string oldPath = std::any_cast<std::string>(args[0]);
        std::string newPath = std::any_cast<std::string>(args[1]);
        try {
            fs::rename(oldPath, newPath);
            return createOkResult(true, "bool");
        } catch (const std::exception& e) {
            return createErrResult(std::string("Failed to rename file: ") + e.what());
        }
    }, FunctionSignature{{"string", "string"}, "Result<bool, Error>"});

    registerFunction("io", "copy", [](const std::vector<std::any>& args) -> std::any {
        std::string src = std::any_cast<std::string>(args[0]);
        std::string dst = std::any_cast<std::string>(args[1]);
        try {
            fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
            return createOkResult(true, "bool");
        } catch (const std::exception& e) {
            return createErrResult(std::string("Failed to copy file: ") + e.what());
        }
    }, FunctionSignature{{"string", "string"}, "Result<bool, Error>"});

    // Directory operations
    registerFunction("io", "mkdir", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        try {
            bool success = fs::create_directory(path);
            if (!success && !fs::exists(path)) {
                return createErrResult("Failed to create directory: " + path);
            }
            return createOkResult(true, "bool");
        } catch (const std::exception& e) {
            return createErrResult(std::string("Failed to create directory: ") + e.what());
        }
    }, FunctionSignature{{"string"}, "Result<bool, Error>"});

    registerFunction("io", "mkdirAll", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        try {
            bool success = fs::create_directories(path);
            if (!success && !fs::exists(path)) {
                return createErrResult("Failed to create directories: " + path);
            }
            return createOkResult(true, "bool");
        } catch (const std::exception& e) {
            return createErrResult(std::string("Failed to create directories: ") + e.what());
        }
    }, FunctionSignature{{"string"}, "Result<bool, Error>"});

    registerFunction("io", "removeDir", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        try {
            bool success = fs::remove(path);
            if (!success) {
                return createErrResult("Directory not found or not empty: " + path);
            }
            return createOkResult(true, "bool");
        } catch (const std::exception& e) {
            return createErrResult(std::string("Failed to remove directory: ") + e.what());
        }
    }, FunctionSignature{{"string"}, "Result<bool, Error>"});

    registerFunction("io", "removeDirAll", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        try {
            uintmax_t removed = fs::remove_all(path);
            return createOkResult(true, "bool");
        } catch (const std::exception& e) {
            return createErrResult(std::string("Failed to remove directory tree: ") + e.what());
        }
    }, FunctionSignature{{"string"}, "Result<bool, Error>"});

    // File info - exists is simple bool, no Result needed
    registerFunction("io", "exists", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        try {
            return fs::exists(path);
        } catch (...) {
            return false;
        }
    }, FunctionSignature{{"string"}, "bool"});

    registerFunction("io", "isFile", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        try {
            return fs::is_regular_file(path);
        } catch (...) {
            return false;
        }
    }, FunctionSignature{{"string"}, "bool"});

    registerFunction("io", "isDirectory", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        try {
            return fs::is_directory(path);
        } catch (...) {
            return false;
        }
    }, FunctionSignature{{"string"}, "bool"});

    registerFunction("io", "fileSize", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        try {
            int size = static_cast<int>(fs::file_size(path));
            return createOkResult(size, "int");
        } catch (const std::exception& e) {
            return createErrResult(std::string("Failed to get file size: ") + e.what());
        }
    }, FunctionSignature{{"string"}, "Result<int, Error>"});

    // Directory listing
    registerFunction("io", "readDir", [](const std::vector<std::any>& args) -> std::any {
        std::string path = std::any_cast<std::string>(args[0]);
        try {
            std::vector<std::shared_ptr<ClassInstance>> fileInfoList;

            for (const auto& entry : fs::directory_iterator(path)) {
                auto fileInfo = std::make_shared<ClassInstance>();
                fileInfo->className = "io::FileInfo";

                fileInfo->fields["fileName"] = RuntimeValue(entry.path().filename().string(), "string");
                fileInfo->fields["isDir"] = RuntimeValue(entry.is_directory(), "bool");

                // Get file size (0 for directories)
                int size = 0;
                if (entry.is_regular_file()) {
                    try {
                        size = static_cast<int>(fs::file_size(entry.path()));
                    } catch (...) {
                        size = 0;
                    }
                }
                fileInfo->fields["size"] = RuntimeValue(size, "int");

                // Get modification time (Unix timestamp)
                try {
                    auto ftime = fs::last_write_time(entry.path());
                    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                        ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
                    );
                    auto timestamp = std::chrono::system_clock::to_time_t(sctp);
                    fileInfo->fields["modTime"] = RuntimeValue(static_cast<int>(timestamp), "int");
                } catch (...) {
                    fileInfo->fields["modTime"] = RuntimeValue(0, "int");
                }

                fileInfoList.push_back(fileInfo);
            }

            return createOkResult(std::any(fileInfoList), "array<FileInfo>");
        } catch (const std::exception& e) {
            return createErrResult(std::string("Failed to read directory: ") + e.what());
        }
    }, FunctionSignature{{"string"}, "Result<Array<FileInfo>, Error>"});

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

    // log.infoWith(message, data) - Log info message with structured data
    registerFunction("log", "infoWith", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 2) return std::any();

        try {
            std::string message = std::any_cast<std::string>(args[0]);

            // Format the data as key=value pairs
            std::string dataStr;
            try {
                auto data = std::any_cast<std::unordered_map<std::string, std::any>>(args[1]);
                for (const auto& [key, value] : data) {
                    if (!dataStr.empty()) dataStr += ", ";
                    dataStr += key + "=";

                    // Try different types
                    if (auto* s = std::any_cast<std::string>(&value)) {
                        dataStr += *s;
                    } else if (auto* i = std::any_cast<int>(&value)) {
                        dataStr += std::to_string(*i);
                    } else if (auto* b = std::any_cast<bool>(&value)) {
                        dataStr += (*b ? "true" : "false");
                    } else if (auto* d = std::any_cast<double>(&value)) {
                        dataStr += std::to_string(*d);
                    } else {
                        dataStr += "<unknown>";
                    }
                }
            } catch (...) {
                dataStr = "<data>";
            }

            Logger::instance().info(message + " {" + dataStr + "}");
        } catch (...) {
            Logger::instance().error("Failed to log info message: invalid type");
        }

        return std::any();
    }, FunctionSignature{{"string", "map"}, "void"});

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
        auto secs = duration_cast<seconds>(now.time_since_epoch()).count();
        // Return a Time object with timestamp field (in seconds to avoid overflow)
        auto timeObj = std::make_shared<ClassInstance>();
        timeObj->className = "Time";
        timeObj->fields["timestamp"] = RuntimeValue(static_cast<int>(secs), "int");
        return timeObj;
    }, FunctionSignature{{}, "Time"});

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

// Forward declarations
std::string serializeAny(const std::any& val);
std::string serializeRuntimeValue(const RuntimeValue& val);

// Serialize a JsonValue object according to its type field
std::string serializeJsonValue(const std::shared_ptr<ClassInstance>& jsonValue) {
    if (!jsonValue || jsonValue->className != "JsonValue") {
        return "null";
    }

    // Get the type field (enum index)
    int typeIndex = 0;
    if (jsonValue->fields.count("type")) {
        typeIndex = jsonValue->fields.at("type").asInt();
    }

    // JsonType: NULL=0, BOOLEAN=1, NUMBER=2, STRING=3, ARRAY=4, OBJECT=5
    switch (typeIndex) {
        case 0: // NULL
            return "null";
        case 1: // BOOLEAN
            if (jsonValue->fields.count("boolValue")) {
                return jsonValue->fields.at("boolValue").asBool() ? "true" : "false";
            }
            return "false";
        case 2: // NUMBER
            if (jsonValue->fields.count("numberValue")) {
                double num = jsonValue->fields.at("numberValue").asDouble();
                // Output integers without decimal point
                if (num == static_cast<int>(num)) {
                    return std::to_string(static_cast<int>(num));
                }
                return std::to_string(num);
            }
            return "0";
        case 3: // STRING
            if (jsonValue->fields.count("stringValue")) {
                return serializeRuntimeValue(RuntimeValue(jsonValue->fields.at("stringValue").asString()));
            }
            return "\"\"";
        case 4: // ARRAY
            if (jsonValue->fields.count("arrayValue")) {
                const auto& arrVal = jsonValue->fields.at("arrayValue");
                if (std::holds_alternative<std::any>(arrVal.value)) {
                    const auto& anyVal = std::get<std::any>(arrVal.value);
                    // Handle std::vector<RuntimeValue> (how Stratos stores object arrays)
                    if (anyVal.type() == typeid(std::vector<RuntimeValue>)) {
                        auto vec = std::any_cast<std::vector<RuntimeValue>>(anyVal);
                        std::stringstream ss;
                        ss << "[";
                        for (size_t i = 0; i < vec.size(); ++i) {
                            if (i > 0) ss << ", ";
                            if (vec[i].type == "object") {
                                auto elem = vec[i].asObject();
                                if (elem && elem->className == "JsonValue") {
                                    ss << serializeJsonValue(elem);
                                } else {
                                    ss << serializeRuntimeValue(vec[i]);
                                }
                            } else {
                                ss << serializeRuntimeValue(vec[i]);
                            }
                        }
                        ss << "]";
                        return ss.str();
                    }
                    // Handle std::vector<std::shared_ptr<ClassInstance>> (alternative storage)
                    if (anyVal.type() == typeid(std::vector<std::shared_ptr<ClassInstance>>)) {
                        auto vec = std::any_cast<std::vector<std::shared_ptr<ClassInstance>>>(anyVal);
                        std::stringstream ss;
                        ss << "[";
                        for (size_t i = 0; i < vec.size(); ++i) {
                            if (i > 0) ss << ", ";
                            ss << serializeJsonValue(vec[i]);
                        }
                        ss << "]";
                        return ss.str();
                    }
                }
            }
            return "[]";
        case 5: // OBJECT
            if (jsonValue->fields.count("objectValue")) {
                const auto& objVal = jsonValue->fields.at("objectValue");
                if (objVal.type == "object") {
                    auto obj = objVal.asObject();
                    std::stringstream ss;
                    ss << "{";
                    bool first = true;
                    for (const auto& [key, field] : obj->fields) {
                        if (!first) ss << ", ";
                        ss << "\"" << key << "\": ";
                        // Check if field value is a JsonValue
                        if (field.type == "object") {
                            auto fieldObj = field.asObject();
                            if (fieldObj && fieldObj->className == "JsonValue") {
                                ss << serializeJsonValue(fieldObj);
                            } else {
                                ss << serializeRuntimeValue(field);
                            }
                        } else {
                            ss << serializeRuntimeValue(field);
                        }
                        first = false;
                    }
                    ss << "}";
                    return ss.str();
                }
            }
            return "{}";
        default:
            return "null";
    }
}

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
        // Check if this is a JsonValue - serialize it specially
        if (instance && instance->className == "JsonValue") {
            return serializeJsonValue(instance);
        }
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
    // JSON parsing using SimpleJsonParser
    registerFunction("json", "jsonParse", [](const std::vector<std::any>& args) -> std::any {
        std::string jsonStr = std::any_cast<std::string>(args[0]);

        // Parse the JSON string
        SimpleJsonParser parser(jsonStr);
        RuntimeValue parsed = parser.parse(RuntimeValue());

        // Convert the parsed RuntimeValue to a JsonValue
        auto jsonValue = std::make_shared<ClassInstance>();
        jsonValue->className = "JsonValue";

        if (parsed.type == "object") {
            jsonValue->fields["type"] = RuntimeValue(5, "int");  // OBJECT
            jsonValue->fields["objectValue"] = RuntimeValue(parsed.asObject());
        } else if (parsed.type == "string") {
            jsonValue->fields["type"] = RuntimeValue(3, "int");  // STRING
            jsonValue->fields["stringValue"] = RuntimeValue(parsed.asString());
        } else if (parsed.type == "int") {
            jsonValue->fields["type"] = RuntimeValue(2, "int");  // NUMBER
            jsonValue->fields["numberValue"] = RuntimeValue(static_cast<double>(parsed.asInt()));
        } else if (parsed.type == "double") {
            jsonValue->fields["type"] = RuntimeValue(2, "int");  // NUMBER
            jsonValue->fields["numberValue"] = RuntimeValue(parsed.asDouble());
        } else if (parsed.type == "bool") {
            jsonValue->fields["type"] = RuntimeValue(1, "int");  // BOOLEAN
            jsonValue->fields["boolValue"] = RuntimeValue(parsed.asBool());
        } else {
            jsonValue->fields["type"] = RuntimeValue(0, "int");  // NULL
        }

        // Initialize other fields with defaults
        if (!jsonValue->fields.count("boolValue"))
            jsonValue->fields["boolValue"] = RuntimeValue(false, "bool");
        if (!jsonValue->fields.count("numberValue"))
            jsonValue->fields["numberValue"] = RuntimeValue(0.0, "double");
        if (!jsonValue->fields.count("stringValue"))
            jsonValue->fields["stringValue"] = RuntimeValue(std::string(""), "string");

        return jsonValue;
    }, FunctionSignature{{"string"}, "JsonValue"});

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

    // getStringPath - Get a string value from a JsonValue by key/path
    registerFunction("json", "getStringPath", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 2) return std::string("");

        // First arg is the JsonValue, second is the key/path
        std::string key;
        if (args[1].type() == typeid(std::string)) {
            key = std::any_cast<std::string>(args[1]);
        } else {
            return std::string("");
        }

        // Handle JsonValue (ClassInstance with className "JsonValue")
        if (args[0].type() == typeid(std::shared_ptr<ClassInstance>)) {
            auto jsonValue = std::any_cast<std::shared_ptr<ClassInstance>>(args[0]);

            if (jsonValue && jsonValue->className == "JsonValue") {
                // Check if it's an OBJECT type (type == 5)
                if (jsonValue->fields.count("type") && jsonValue->fields["type"].asInt() == 5) {
                    // Get the objectValue field
                    if (jsonValue->fields.count("objectValue")) {
                        auto& objVal = jsonValue->fields["objectValue"];
                        if (objVal.type == "object") {
                            auto obj = objVal.asObject();
                            if (obj && obj->fields.count(key)) {
                                auto& fieldVal = obj->fields[key];
                                // If it's a string, return it directly
                                if (fieldVal.type == "string") {
                                    return fieldVal.asString();
                                }
                                // If it's a JsonValue with STRING type
                                if (fieldVal.type == "object") {
                                    auto fieldObj = fieldVal.asObject();
                                    if (fieldObj && fieldObj->className == "JsonValue") {
                                        if (fieldObj->fields.count("type") && fieldObj->fields["type"].asInt() == 3) {
                                            if (fieldObj->fields.count("stringValue")) {
                                                return fieldObj->fields["stringValue"].asString();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        return std::string("");
    }, FunctionSignature{{"JsonValue", "string"}, "string"});

    // JsonType enum indices: NULL=0, BOOLEAN=1, NUMBER=2, STRING=3, ARRAY=4, OBJECT=5

    // jsonNull() JsonValue - creates a null JsonValue
    registerFunction("json", "jsonNull", [](const std::vector<std::any>& args) -> std::any {
        auto jsonValue = std::make_shared<ClassInstance>();
        jsonValue->className = "JsonValue";
        jsonValue->fields["type"] = RuntimeValue(0, "int");  // JsonType.NULL
        jsonValue->fields["boolValue"] = RuntimeValue(false, "bool");
        jsonValue->fields["numberValue"] = RuntimeValue(0.0, "double");
        jsonValue->fields["stringValue"] = RuntimeValue(std::string(""), "string");
        return jsonValue;
    }, FunctionSignature{{}, "JsonValue"});

    // jsonBool(value: bool) JsonValue - creates a boolean JsonValue
    registerFunction("json", "jsonBool", [](const std::vector<std::any>& args) -> std::any {
        bool value = std::any_cast<bool>(args[0]);
        auto jsonValue = std::make_shared<ClassInstance>();
        jsonValue->className = "JsonValue";
        jsonValue->fields["type"] = RuntimeValue(1, "int");  // JsonType.BOOLEAN
        jsonValue->fields["boolValue"] = RuntimeValue(value, "bool");
        jsonValue->fields["numberValue"] = RuntimeValue(0.0, "double");
        jsonValue->fields["stringValue"] = RuntimeValue(std::string(""), "string");
        return jsonValue;
    }, FunctionSignature{{"bool"}, "JsonValue"});

    // jsonNumber(value: double) JsonValue - creates a number JsonValue
    registerFunction("json", "jsonNumber", [](const std::vector<std::any>& args) -> std::any {
        double value = 0.0;
        if (args[0].type() == typeid(int)) {
            value = static_cast<double>(std::any_cast<int>(args[0]));
        } else if (args[0].type() == typeid(double)) {
            value = std::any_cast<double>(args[0]);
        }
        auto jsonValue = std::make_shared<ClassInstance>();
        jsonValue->className = "JsonValue";
        jsonValue->fields["type"] = RuntimeValue(2, "int");  // JsonType.NUMBER
        jsonValue->fields["boolValue"] = RuntimeValue(false, "bool");
        jsonValue->fields["numberValue"] = RuntimeValue(value, "double");
        jsonValue->fields["stringValue"] = RuntimeValue(std::string(""), "string");
        return jsonValue;
    }, FunctionSignature{{"double"}, "JsonValue"});

    // jsonNumberInt(value: int) JsonValue - creates a number JsonValue from int
    registerFunction("json", "jsonNumberInt", [](const std::vector<std::any>& args) -> std::any {
        int intValue = std::any_cast<int>(args[0]);
        double value = static_cast<double>(intValue);
        auto jsonValue = std::make_shared<ClassInstance>();
        jsonValue->className = "JsonValue";
        jsonValue->fields["type"] = RuntimeValue(2, "int");  // JsonType.NUMBER
        jsonValue->fields["boolValue"] = RuntimeValue(false, "bool");
        jsonValue->fields["numberValue"] = RuntimeValue(value, "double");
        jsonValue->fields["stringValue"] = RuntimeValue(std::string(""), "string");
        return jsonValue;
    }, FunctionSignature{{"int"}, "JsonValue"});

    // jsonString(value: string) JsonValue - creates a string JsonValue
    registerFunction("json", "jsonString", [](const std::vector<std::any>& args) -> std::any {
        std::string value = std::any_cast<std::string>(args[0]);
        auto jsonValue = std::make_shared<ClassInstance>();
        jsonValue->className = "JsonValue";
        jsonValue->fields["type"] = RuntimeValue(3, "int");  // JsonType.STRING
        jsonValue->fields["boolValue"] = RuntimeValue(false, "bool");
        jsonValue->fields["numberValue"] = RuntimeValue(0.0, "double");
        jsonValue->fields["stringValue"] = RuntimeValue(value, "string");
        return jsonValue;
    }, FunctionSignature{{"string"}, "JsonValue"});

    // jsonArray(values: Array<JsonValue>) JsonValue - creates an array JsonValue
    registerFunction("json", "jsonArray", [](const std::vector<std::any>& args) -> std::any {
        auto jsonValue = std::make_shared<ClassInstance>();
        jsonValue->className = "JsonValue";
        jsonValue->fields["type"] = RuntimeValue(4, "int");  // JsonType.ARRAY
        jsonValue->fields["boolValue"] = RuntimeValue(false, "bool");
        jsonValue->fields["numberValue"] = RuntimeValue(0.0, "double");
        jsonValue->fields["stringValue"] = RuntimeValue(std::string(""), "string");
        // Pass through the array value
        if (!args.empty()) {
            jsonValue->fields["arrayValue"] = RuntimeValue(args[0], "array<JsonValue>");
        }
        return jsonValue;
    }, FunctionSignature{{"array<JsonValue>"}, "JsonValue"});

    // jsonObject(entries: object) JsonValue - creates an object JsonValue from object literal
    registerFunction("json", "jsonObject", [](const std::vector<std::any>& args) -> std::any {
        auto jsonValue = std::make_shared<ClassInstance>();
        jsonValue->className = "JsonValue";
        jsonValue->fields["type"] = RuntimeValue(5, "int");  // JsonType.OBJECT
        jsonValue->fields["boolValue"] = RuntimeValue(false, "bool");
        jsonValue->fields["numberValue"] = RuntimeValue(0.0, "double");
        jsonValue->fields["stringValue"] = RuntimeValue(std::string(""), "string");

        if (!args.empty()) {
            // Handle ClassInstance (object literal)
            if (args[0].type() == typeid(std::shared_ptr<ClassInstance>)) {
                auto inputObj = std::any_cast<std::shared_ptr<ClassInstance>>(args[0]);
                jsonValue->fields["objectValue"] = RuntimeValue(inputObj);
            }
            // Handle unordered_map<string, any> (how object literals are sometimes passed)
            else if (args[0].type() == typeid(std::unordered_map<std::string, std::any>)) {
                auto inputMap = std::any_cast<std::unordered_map<std::string, std::any>>(args[0]);
                // Convert to ClassInstance
                auto objInstance = std::make_shared<ClassInstance>();
                for (const auto& [key, value] : inputMap) {
                    // Convert std::any values to RuntimeValue
                    if (value.type() == typeid(std::shared_ptr<ClassInstance>)) {
                        objInstance->fields[key] = RuntimeValue(std::any_cast<std::shared_ptr<ClassInstance>>(value));
                    } else if (value.type() == typeid(std::string)) {
                        objInstance->fields[key] = RuntimeValue(std::any_cast<std::string>(value));
                    } else if (value.type() == typeid(int)) {
                        objInstance->fields[key] = RuntimeValue(std::any_cast<int>(value));
                    } else if (value.type() == typeid(double)) {
                        objInstance->fields[key] = RuntimeValue(std::any_cast<double>(value));
                    } else if (value.type() == typeid(bool)) {
                        objInstance->fields[key] = RuntimeValue(std::any_cast<bool>(value));
                    } else {
                        objInstance->fields[key] = RuntimeValue(value, "any");
                    }
                }
                jsonValue->fields["objectValue"] = RuntimeValue(objInstance);
            }
        }
        return jsonValue;
    }, FunctionSignature{{"any"}, "JsonValue"});
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

    // maps.put - Put a key-value pair in the map (supports any value type)
    registerFunction("maps", "put", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 3) return args[0];

        // Get the key (convert int to string if needed)
        std::string key;
        if (args[1].type() == typeid(std::string)) {
            key = std::any_cast<std::string>(args[1]);
        } else if (args[1].type() == typeid(int)) {
            key = std::to_string(std::any_cast<int>(args[1]));
        } else {
            return args[0];
        }

        // Try map<string, string>
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
            std::string value = std::any_cast<std::string>(args[2]);
            map[key] = value;
            return map;
        } catch (const std::bad_any_cast&) {}

        // Try map<string, any>
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::any>>(args[0]);
            map[key] = args[2];
            return map;
        } catch (const std::bad_any_cast&) {}

        return args[0];
    }, FunctionSignature{{"any", "any", "any"}, "any"});

    // maps.get - Get a value by key (returns empty/null if not found)
    registerFunction("maps", "get", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 2) return std::any();

        // Get the key as string
        std::string keyStr;
        if (args[1].type() == typeid(std::string)) {
            keyStr = std::any_cast<std::string>(args[1]);
        } else if (args[1].type() == typeid(int)) {
            keyStr = std::to_string(std::any_cast<int>(args[1]));
        }

        // Try map<string, string>
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
            auto it = map.find(keyStr);
            if (it != map.end()) {
                return it->second;
            }
            return std::string("");
        } catch (const std::bad_any_cast&) {}

        // Try map<string, any>
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::any>>(args[0]);
            auto it = map.find(keyStr);
            if (it != map.end()) {
                return it->second;
            }
            return std::any();
        } catch (const std::bad_any_cast&) {}

        return std::any();
    }, FunctionSignature{{"any", "any"}, "any"});

    // maps.has - Check if a key exists in the map
    registerFunction("maps", "has", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        std::string key = std::any_cast<std::string>(args[1]);
        return map.find(key) != map.end();
    }, FunctionSignature{{"map<string,string>", "string"}, "bool"});

    // maps.containsKey - Check if a key exists in the map (works with any map type)
    registerFunction("maps", "containsKey", [](const std::vector<std::any>& args) -> std::any {
        if (args.size() < 2) return false;

        // Get the key as string
        std::string keyStr;
        if (args[1].type() == typeid(std::string)) {
            keyStr = std::any_cast<std::string>(args[1]);
        } else if (args[1].type() == typeid(int)) {
            keyStr = std::to_string(std::any_cast<int>(args[1]));
        }

        // Try different map types
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
            return map.find(keyStr) != map.end();
        } catch (const std::bad_any_cast&) {}
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::any>>(args[0]);
            return map.find(keyStr) != map.end();
        } catch (const std::bad_any_cast&) {}
        try {
            // For Map<string, JsonValue> which is stored as ClassInstance
            auto mapObj = std::any_cast<std::shared_ptr<ClassInstance>>(args[0]);
            if (mapObj && mapObj->fields.count("__data")) {
                auto& dataVal = mapObj->fields["__data"];
                if (std::holds_alternative<std::any>(dataVal.value)) {
                    auto& anyData = std::get<std::any>(dataVal.value);
                    auto data = std::any_cast<std::unordered_map<std::string, std::any>>(anyData);
                    std::string key;
                    if (args[1].type() == typeid(std::string)) {
                        key = std::any_cast<std::string>(args[1]);
                    } else if (args[1].type() == typeid(int)) {
                        key = std::to_string(std::any_cast<int>(args[1]));
                    }
                    return data.find(key) != data.end();
                }
            }
        } catch (const std::bad_any_cast&) {}
        return false;
    }, FunctionSignature{{"any", "any"}, "bool"});

    // maps.remove - Remove a key from the map
    registerFunction("maps", "remove", [](const std::vector<std::any>& args) -> std::any {
        auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
        std::string key = std::any_cast<std::string>(args[1]);
        map.erase(key);
        return map;
    }, FunctionSignature{{"map<string,string>", "string"}, "map<string,string>"});

    // maps.size - Get the number of entries in the map
    registerFunction("maps", "size", [](const std::vector<std::any>& args) -> std::any {
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
            return static_cast<int>(map.size());
        } catch (const std::bad_any_cast&) {}
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::any>>(args[0]);
            return static_cast<int>(map.size());
        } catch (const std::bad_any_cast&) {}
        return 0;
    }, FunctionSignature{{"any"}, "int"});

    // maps.length - Alias for size
    registerFunction("maps", "length", [](const std::vector<std::any>& args) -> std::any {
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
            return static_cast<int>(map.size());
        } catch (const std::bad_any_cast&) {}
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::any>>(args[0]);
            return static_cast<int>(map.size());
        } catch (const std::bad_any_cast&) {}
        return 0;
    }, FunctionSignature{{"any"}, "int"});

    // maps.keys - Get all keys as an array
    registerFunction("maps", "keys", [](const std::vector<std::any>& args) -> std::any {
        std::vector<std::string> keys;
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
            for (const auto& pair : map) {
                keys.push_back(pair.first);
            }
            return keys;
        } catch (const std::bad_any_cast&) {}
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::any>>(args[0]);
            for (const auto& pair : map) {
                keys.push_back(pair.first);
            }
            return keys;
        } catch (const std::bad_any_cast&) {}
        return keys;
    }, FunctionSignature{{"any"}, "array<string>"});

    // maps.values - Get all values as an array
    registerFunction("maps", "values", [](const std::vector<std::any>& args) -> std::any {
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::string>>(args[0]);
            std::vector<std::string> values;
            for (const auto& pair : map) {
                values.push_back(pair.second);
            }
            return values;
        } catch (const std::bad_any_cast&) {}
        try {
            auto map = std::any_cast<std::unordered_map<std::string, std::any>>(args[0]);
            std::vector<std::any> values;
            for (const auto& pair : map) {
                values.push_back(pair.second);
            }
            return values;
        } catch (const std::bad_any_cast&) {}
        return std::vector<std::any>();
    }, FunctionSignature{{"any"}, "array<any>"});

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
// Resource Manager - Safe Handle Management
// ============================================================================

// Resource types for the handle table
enum class ResourceType {
    SQLiteDB,
    SQLiteStmt,
    PostgreSQLConn,
    MySQLConn,
    RedisContext
};

// Thread-safe handle table for native resources
class ResourceManager {
public:
    static ResourceManager& instance() {
        static ResourceManager mgr;
        return mgr;
    }

    // Register a resource and get a unique handle ID
    int registerResource(void* ptr, ResourceType type) {
        std::lock_guard<std::mutex> lock(mutex_);
        int handle = ++handleCounter_;
        resources_[handle] = {ptr, type, true};
        return handle;
    }

    // Get a resource pointer, returns nullptr if invalid/closed
    void* getResource(int handle, ResourceType expectedType) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = resources_.find(handle);
        if (it == resources_.end()) {
            return nullptr;
        }
        if (!it->second.valid || it->second.type != expectedType) {
            return nullptr;
        }
        return it->second.ptr;
    }

    // Mark a resource as closed (invalidates the handle)
    bool closeResource(int handle) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = resources_.find(handle);
        if (it == resources_.end() || !it->second.valid) {
            return false;
        }
        it->second.valid = false;
        it->second.ptr = nullptr;
        return true;
    }

    // Check if a handle is valid
    bool isValid(int handle, ResourceType expectedType) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = resources_.find(handle);
        if (it == resources_.end()) {
            return false;
        }
        return it->second.valid && it->second.type == expectedType;
    }

private:
    ResourceManager() = default;

    struct ResourceEntry {
        void* ptr;
        ResourceType type;
        bool valid;
    };

    std::mutex mutex_;
    std::unordered_map<int, ResourceEntry> resources_;
    int handleCounter_ = 0;
};

// Helper macros for safe resource access
#define GET_SQLITE_DB(handle) \
    static_cast<sqlite3*>(ResourceManager::instance().getResource(handle, ResourceType::SQLiteDB))

#define GET_SQLITE_STMT(handle) \
    static_cast<sqlite3_stmt*>(ResourceManager::instance().getResource(handle, ResourceType::SQLiteStmt))

#define GET_PSQL_CONN(handle) \
    static_cast<PGconn*>(ResourceManager::instance().getResource(handle, ResourceType::PostgreSQLConn))

#define GET_MYSQL_CONN(handle) \
    static_cast<MYSQL*>(ResourceManager::instance().getResource(handle, ResourceType::MySQLConn))

#define GET_REDIS_CTX(handle) \
    static_cast<redisContext*>(ResourceManager::instance().getResource(handle, ResourceType::RedisContext))

// ============================================================================
// SQLite Module - Database Support
// ============================================================================

void NativeRegistry::initSQLite() {
    // __sqlite_open(path: string): int (handle ID)
    registerFunction("db", "__sqlite_open", [](const std::vector<std::any>& args) -> std::any {
        auto path = std::any_cast<std::string>(args[0]);
        sqlite3* db = nullptr;

        int rc = sqlite3_open(path.c_str(), &db);
        if (rc != SQLITE_OK) {
            std::string error = "Failed to open database: " + std::string(sqlite3_errmsg(db));
            sqlite3_close(db);
            throw std::runtime_error(error);
        }

        // Register with ResourceManager and return handle ID
        int handle = ResourceManager::instance().registerResource(db, ResourceType::SQLiteDB);
        return std::any(handle);
    }, FunctionSignature{{"string"}, "int"});

    // __sqlite_close(handle: int): void
    registerFunction("db", "__sqlite_close", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        sqlite3* db = GET_SQLITE_DB(handle);
        if (!db) {
            throw std::runtime_error("Invalid or already closed SQLite database handle");
        }
        sqlite3_close(db);
        ResourceManager::instance().closeResource(handle);
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // __sqlite_exec(handle: int, sql: string): int
    registerFunction("db", "__sqlite_exec", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        sqlite3* db = GET_SQLITE_DB(handle);
        if (!db) {
            throw std::runtime_error("Invalid or closed SQLite database handle");
        }

        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

        if (rc != SQLITE_OK) {
            std::string error = "SQL error: " + std::string(errMsg);
            sqlite3_free(errMsg);
            throw std::runtime_error(error);
        }

        return std::any(sqlite3_changes(db));
    }, FunctionSignature{{"int", "string"}, "int"});

    // __sqlite_prepare(handle: int, sql: string): int (stmt handle)
    registerFunction("db", "__sqlite_prepare", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        sqlite3* db = GET_SQLITE_DB(handle);
        if (!db) {
            throw std::runtime_error("Invalid or closed SQLite database handle");
        }

        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::string error = "Failed to prepare statement: " + std::string(sqlite3_errmsg(db));
            throw std::runtime_error(error);
        }

        int stmtHandle = ResourceManager::instance().registerResource(stmt, ResourceType::SQLiteStmt);
        return std::any(stmtHandle);
    }, FunctionSignature{{"int", "string"}, "int"});

    // __sqlite_query(handle: int, sql: string): int (stmt handle)
    registerFunction("db", "__sqlite_query", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        sqlite3* db = GET_SQLITE_DB(handle);
        if (!db) {
            throw std::runtime_error("Invalid or closed SQLite database handle");
        }

        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::string error = "Failed to prepare query: " + std::string(sqlite3_errmsg(db));
            throw std::runtime_error(error);
        }

        int stmtHandle = ResourceManager::instance().registerResource(stmt, ResourceType::SQLiteStmt);
        return std::any(stmtHandle);
    }, FunctionSignature{{"int", "string"}, "int"});

    // __sqlite_bind_int(handle: int, index: int, value: int): void
    registerFunction("db", "__sqlite_bind_int", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        auto value = std::any_cast<int>(args[2]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        int rc = sqlite3_bind_int(stmt, index, value);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to bind int parameter");
        }
        return std::any();
    }, FunctionSignature{{"int", "int", "int"}, "void"});

    // __sqlite_bind_double(handle: int, index: int, value: double): void
    registerFunction("db", "__sqlite_bind_double", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        auto value = std::any_cast<double>(args[2]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        int rc = sqlite3_bind_double(stmt, index, value);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to bind double parameter");
        }
        return std::any();
    }, FunctionSignature{{"int", "int", "double"}, "void"});

    // __sqlite_bind_string(handle: int, index: int, value: string): void
    registerFunction("db", "__sqlite_bind_string", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        auto value = std::any_cast<std::string>(args[2]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        int rc = sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to bind string parameter");
        }
        return std::any();
    }, FunctionSignature{{"int", "int", "string"}, "void"});

    // __sqlite_bind_null(handle: int, index: int): void
    registerFunction("db", "__sqlite_bind_null", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        int rc = sqlite3_bind_null(stmt, index);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to bind null parameter");
        }
        return std::any();
    }, FunctionSignature{{"int", "int"}, "void"});

    // __sqlite_bind(handle: int, index: int, value: any): void
    registerFunction("db", "__sqlite_bind", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        auto& value = args[2];
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

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
    }, FunctionSignature{{"int", "int", "any"}, "void"});

    // __sqlite_stmt_exec(handle: int): int
    registerFunction("db", "__sqlite_stmt_exec", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        int rc = sqlite3_step(stmt);
        sqlite3_reset(stmt);

        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
            throw std::runtime_error("Failed to execute statement");
        }

        sqlite3* db = sqlite3_db_handle(stmt);
        return std::any(sqlite3_changes(db));
    }, FunctionSignature{{"int"}, "int"});

    // __sqlite_step(handle: int): bool
    registerFunction("db", "__sqlite_step", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        int rc = sqlite3_step(stmt);
        return std::any(rc == SQLITE_ROW);
    }, FunctionSignature{{"int"}, "bool"});

    // __sqlite_column_int(handle: int, index: int): int
    registerFunction("db", "__sqlite_column_int", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        return std::any(sqlite3_column_int(stmt, index));
    }, FunctionSignature{{"int", "int"}, "int"});

    // __sqlite_column_double(handle: int, index: int): double
    registerFunction("db", "__sqlite_column_double", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        return std::any(sqlite3_column_double(stmt, index));
    }, FunctionSignature{{"int", "int"}, "double"});

    // __sqlite_column_string(handle: int, index: int): string
    registerFunction("db", "__sqlite_column_string", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        const unsigned char* text = sqlite3_column_text(stmt, index);
        if (text) {
            return std::any(std::string(reinterpret_cast<const char*>(text)));
        }
        return std::any(std::string(""));
    }, FunctionSignature{{"int", "int"}, "string"});

    // __sqlite_column_index(handle: int, name: string): int
    registerFunction("db", "__sqlite_column_index", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto name = std::any_cast<std::string>(args[1]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        int columnCount = sqlite3_column_count(stmt);
        for (int i = 0; i < columnCount; i++) {
            const char* columnName = sqlite3_column_name(stmt, i);
            if (columnName && name == columnName) {
                return std::any(i);
            }
        }

        throw std::runtime_error("Column not found: " + name);
    }, FunctionSignature{{"int", "string"}, "int"});

    // __sqlite_column_is_null(handle: int, index: int): bool
    registerFunction("db", "__sqlite_column_is_null", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        return std::any(sqlite3_column_type(stmt, index) == SQLITE_NULL);
    }, FunctionSignature{{"int", "int"}, "bool"});

    // __sqlite_column_count(handle: int): int
    registerFunction("db", "__sqlite_column_count", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        return std::any(sqlite3_column_count(stmt));
    }, FunctionSignature{{"int"}, "int"});

    // __sqlite_column_name(handle: int, index: int): string
    registerFunction("db", "__sqlite_column_name", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto index = std::any_cast<int>(args[1]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }

        const char* name = sqlite3_column_name(stmt, index);
        if (name) {
            return std::any(std::string(name));
        }
        return std::any(std::string(""));
    }, FunctionSignature{{"int", "int"}, "string"});

    // __sqlite_stmt_reset(handle: int): void
    registerFunction("db", "__sqlite_stmt_reset", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }
        sqlite3_reset(stmt);
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // __sqlite_stmt_close(handle: int): void
    registerFunction("db", "__sqlite_stmt_close", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }
        sqlite3_finalize(stmt);
        ResourceManager::instance().closeResource(handle);
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // __sqlite_rows_close(handle: int): void
    registerFunction("db", "__sqlite_rows_close", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        sqlite3_stmt* stmt = GET_SQLITE_STMT(handle);
        if (!stmt) {
            throw std::runtime_error("Invalid or closed SQLite statement handle");
        }
        sqlite3_finalize(stmt);
        ResourceManager::instance().closeResource(handle);
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // __sqlite_last_insert_id(handle: int): int
    registerFunction("db", "__sqlite_last_insert_id", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        sqlite3* db = GET_SQLITE_DB(handle);
        if (!db) {
            throw std::runtime_error("Invalid or closed SQLite database handle");
        }

        return std::any(static_cast<int>(sqlite3_last_insert_rowid(db)));
    }, FunctionSignature{{"int"}, "int"});

    // __sqlite_changes(handle: int): int
    registerFunction("db", "__sqlite_changes", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        sqlite3* db = GET_SQLITE_DB(handle);
        if (!db) {
            throw std::runtime_error("Invalid or closed SQLite database handle");
        }

        return std::any(sqlite3_changes(db));
    }, FunctionSignature{{"int"}, "int"});
}

// ============================================================================
// Concurrent Module Native Functions
// ============================================================================

// Thread-safe channel implementation
template<typename T>
class Channel {
public:
    Channel(size_t capacity = 0) : capacity_(capacity), closed_(false) {}

    bool send(const T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (closed_) return false;

        if (capacity_ > 0) {
            cv_not_full_.wait(lock, [this] { return queue_.size() < capacity_ || closed_; });
        }
        if (closed_) return false;

        queue_.push(value);
        cv_not_empty_.notify_one();
        return true;
    }

    std::pair<T, bool> receive() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_not_empty_.wait(lock, [this] { return !queue_.empty() || closed_; });

        if (queue_.empty()) {
            return {T{}, false};
        }

        T value = queue_.front();
        queue_.pop();
        cv_not_full_.notify_one();
        return {value, true};
    }

    void close() {
        std::unique_lock<std::mutex> lock(mutex_);
        closed_ = true;
        cv_not_empty_.notify_all();
        cv_not_full_.notify_all();
    }

    bool isClosed() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return closed_;
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_not_empty_;
    std::condition_variable cv_not_full_;
    std::queue<T> queue_;
    size_t capacity_;
    bool closed_;
};

// Global storage for concurrent objects
static std::mutex g_concurrent_mutex;
static std::vector<std::shared_ptr<Channel<RuntimeValue>>> g_channels;
static std::vector<std::thread> g_threads;

void NativeRegistry::initConcurrent() {
    // concurrent.go(fn) - spawn a goroutine
    registerFunction("concurrent", "go", [](const std::vector<std::any>& args) -> std::any {
        // The function is passed as a closure - we need interpreter support to call it
        // For now, just spawn a thread that does nothing useful
        // Real implementation requires passing the interpreter context
        return std::any();
    }, FunctionSignature{{"function"}, "void"});

    // concurrent.newChannel(capacity) - create a new channel
    registerFunction("concurrent", "newChannel", [](const std::vector<std::any>& args) -> std::any {
        int capacity = args.empty() ? 0 : std::any_cast<int>(args[0]);

        auto channel = std::make_shared<Channel<RuntimeValue>>(capacity);

        std::lock_guard<std::mutex> lock(g_concurrent_mutex);
        size_t id = g_channels.size();
        g_channels.push_back(channel);

        // Return a channel object
        auto channelObj = std::make_shared<ClassInstance>();
        channelObj->className = "Channel";
        channelObj->fields["__id"] = RuntimeValue(static_cast<int>(id));
        channelObj->fields["capacity"] = RuntimeValue(capacity);
        channelObj->fields["closed"] = RuntimeValue(false);

        return channelObj;
    }, FunctionSignature{{"int"}, "Channel"});

    // concurrent.newWaitGroup() - create a new wait group
    registerFunction("concurrent", "newWaitGroup", [](const std::vector<std::any>& args) -> std::any {
        auto wg = std::make_shared<ClassInstance>();
        wg->className = "WaitGroup";
        wg->fields["count"] = RuntimeValue(0);
        return wg;
    }, FunctionSignature{{}, "WaitGroup"});

    // concurrent.newMutex() - create a new mutex
    registerFunction("concurrent", "newMutex", [](const std::vector<std::any>& args) -> std::any {
        auto mutex = std::make_shared<ClassInstance>();
        mutex->className = "Mutex";
        mutex->fields["locked"] = RuntimeValue(false);
        return mutex;
    }, FunctionSignature{{}, "Mutex"});

    // concurrent.sleep(ms) - sleep for milliseconds
    registerFunction("concurrent", "sleep", [](const std::vector<std::any>& args) -> std::any {
        int ms = std::any_cast<int>(args[0]);
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // concurrent.getThreadId() - get current thread ID
    registerFunction("concurrent", "getThreadId", [](const std::vector<std::any>& args) -> std::any {
        auto id = std::this_thread::get_id();
        std::ostringstream ss;
        ss << id;
        return std::any(static_cast<int>(std::hash<std::thread::id>{}(id) % 1000000));
    }, FunctionSignature{{}, "int"});

    // concurrent.newWorkerPool(numWorkers, workerFunc) - create a worker pool
    // Actual implementation is in Interpreter - this is just a signature registration
    registerFunction("concurrent", "newWorkerPool", [](const std::vector<std::any>& args) -> std::any {
        // Implementation handled by Interpreter for closure support
        return std::any();
    }, FunctionSignature{{"int", "function"}, "WorkerPool"});

    // concurrent.pipeline(inputChannel, stages) - create a pipeline
    // Actual implementation is in Interpreter - this is just a signature registration
    registerFunction("concurrent", "pipeline", [](const std::vector<std::any>& args) -> std::any {
        // Implementation handled by Interpreter for closure support
        return std::any();
    }, FunctionSignature{{"Channel", "array"}, "Channel"});
}

// ============================================================================
// HTTP Module - HTTP Server and Client
// ============================================================================

void NativeRegistry::initAsync() {
    // async.delay(ms) - pause execution for ms milliseconds
    registerFunction("async", "delay", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::any();
        int ms = 0;
        try {
            ms = std::any_cast<int>(args[0]);
        } catch (const std::bad_any_cast&) {
            try {
                ms = static_cast<int>(std::any_cast<double>(args[0]));
            } catch (...) {}
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // async.sleep(ms) - alias for delay
    registerFunction("async", "sleep", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) return std::any();
        int ms = 0;
        try {
            ms = std::any_cast<int>(args[0]);
        } catch (const std::bad_any_cast&) {
            try {
                ms = static_cast<int>(std::any_cast<double>(args[0]));
            } catch (...) {}
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // async.spawn(action) - spawn an async task (stub - real implementation needs interpreter)
    registerFunction("async", "spawn", [](const std::vector<std::any>& args) -> std::any {
        // This requires interpreter support for callbacks
        return std::any();
    }, FunctionSignature{{"function"}, "void"});
}

void NativeRegistry::initHTTP() {
    // http.newRouter() - Create a new HTTP router
    registerFunction("http", "newRouter", [](const std::vector<std::any>& args) -> std::any {
        auto& mgr = HttpServerManager::getInstance();
        int routerId = mgr.createRouter();

        auto router = std::make_shared<ClassInstance>();
        router->className = "Router";
        router->fields["__routerId"] = RuntimeValue(routerId);

        return router;
    }, FunctionSignature{{}, "Router"});

    // http.newServer(router) - Create a new HTTP server
    registerFunction("http", "newServer", [](const std::vector<std::any>& args) -> std::any {
        if (args.empty()) {
            throw std::runtime_error("http.newServer requires a router argument");
        }

        auto routerObj = std::any_cast<std::shared_ptr<ClassInstance>>(args[0]);
        if (!routerObj || routerObj->className != "Router") {
            throw std::runtime_error("http.newServer requires a Router argument");
        }

        int routerId = routerObj->fields["__routerId"].asInt();

        auto& mgr = HttpServerManager::getInstance();
        int serverId = mgr.createServer(routerId);

        auto server = std::make_shared<ClassInstance>();
        server->className = "Server";
        server->fields["__serverId"] = RuntimeValue(serverId);
        server->fields["__routerId"] = RuntimeValue(routerId);

        return server;
    }, FunctionSignature{{"Router"}, "Server"});
}

// ============================================================================
// PostgreSQL Module - Database Support
// ============================================================================

// Thread-safe storage for PostgreSQL result sets
static std::mutex g_psql_mutex;
static std::unordered_map<int, PGresult*> g_psql_results;
static std::unordered_map<int, int> g_psql_result_row;  // Current row index per result
static int g_psql_result_counter = 0;

void NativeRegistry::initPostgreSQL() {
    // __psql_connect(connectionString: string): int (handle ID)
    registerFunction("psql", "__psql_connect", [](const std::vector<std::any>& args) -> std::any {
        auto connStr = std::any_cast<std::string>(args[0]);
        PGconn* conn = PQconnectdb(connStr.c_str());

        if (PQstatus(conn) != CONNECTION_OK) {
            std::string error = "Failed to connect to PostgreSQL: " + std::string(PQerrorMessage(conn));
            PQfinish(conn);
            throw std::runtime_error(error);
        }

        int handle = ResourceManager::instance().registerResource(conn, ResourceType::PostgreSQLConn);
        return std::any(handle);
    }, FunctionSignature{{"string"}, "int"});

    // __psql_close(handle: int): void
    registerFunction("psql", "__psql_close", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        PGconn* conn = GET_PSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or already closed PostgreSQL connection handle");
        }
        PQfinish(conn);
        ResourceManager::instance().closeResource(handle);
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // __psql_exec(handle: int, sql: string): int
    // Executes SQL that doesn't return rows, returns affected row count
    registerFunction("psql", "__psql_exec", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        PGconn* conn = GET_PSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed PostgreSQL connection handle");
        }

        PGresult* result = PQexec(conn, sql.c_str());
        ExecStatusType status = PQresultStatus(result);

        if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
            std::string error = "PostgreSQL error: " + std::string(PQerrorMessage(conn));
            PQclear(result);
            throw std::runtime_error(error);
        }

        // Get affected rows count
        const char* affected = PQcmdTuples(result);
        int count = (affected && affected[0]) ? std::stoi(affected) : 0;
        PQclear(result);

        return std::any(count);
    }, FunctionSignature{{"int", "string"}, "int"});

    // __psql_query(handle: int, sql: string): int (result ID)
    // Executes SQL and returns a result handle for iteration
    registerFunction("psql", "__psql_query", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        PGconn* conn = GET_PSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed PostgreSQL connection handle");
        }

        PGresult* result = PQexec(conn, sql.c_str());
        ExecStatusType status = PQresultStatus(result);

        if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
            std::string error = "PostgreSQL query error: " + std::string(PQerrorMessage(conn));
            PQclear(result);
            throw std::runtime_error(error);
        }

        // Store result in global map and return handle ID
        std::lock_guard<std::mutex> lock(g_psql_mutex);
        int resultId = ++g_psql_result_counter;
        g_psql_results[resultId] = result;
        g_psql_result_row[resultId] = -1;  // Before first row

        return std::any(resultId);
    }, FunctionSignature{{"int", "string"}, "int"});

    // __psql_query_params(handle: int, sql: string, params: array<string>): int (result ID)
    // Executes parameterized SQL query
    registerFunction("psql", "__psql_query_params", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        auto params = std::any_cast<std::vector<std::string>>(args[2]);
        PGconn* conn = GET_PSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed PostgreSQL connection handle");
        }

        // Convert params to C-style array
        std::vector<const char*> paramValues;
        for (const auto& p : params) {
            paramValues.push_back(p.c_str());
        }

        PGresult* result = PQexecParams(conn, sql.c_str(),
            static_cast<int>(params.size()),
            nullptr,  // Let PostgreSQL infer types
            paramValues.data(),
            nullptr,  // Text format for all params
            nullptr,
            0);  // Text format for results

        ExecStatusType status = PQresultStatus(result);
        if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
            std::string error = "PostgreSQL query error: " + std::string(PQerrorMessage(conn));
            PQclear(result);
            throw std::runtime_error(error);
        }

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        int resultId = ++g_psql_result_counter;
        g_psql_results[resultId] = result;
        g_psql_result_row[resultId] = -1;

        return std::any(resultId);
    }, FunctionSignature{{"int", "string", "array<string>"}, "int"});

    // __psql_exec_params(handle: int, sql: string, params: array<string>): int
    // Executes parameterized SQL that doesn't return rows
    registerFunction("psql", "__psql_exec_params", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        auto params = std::any_cast<std::vector<std::string>>(args[2]);
        PGconn* conn = GET_PSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed PostgreSQL connection handle");
        }

        std::vector<const char*> paramValues;
        for (const auto& p : params) {
            paramValues.push_back(p.c_str());
        }

        PGresult* result = PQexecParams(conn, sql.c_str(),
            static_cast<int>(params.size()),
            nullptr, paramValues.data(), nullptr, nullptr, 0);

        ExecStatusType status = PQresultStatus(result);
        if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
            std::string error = "PostgreSQL error: " + std::string(PQerrorMessage(conn));
            PQclear(result);
            throw std::runtime_error(error);
        }

        const char* affected = PQcmdTuples(result);
        int count = (affected && affected[0]) ? std::stoi(affected) : 0;
        PQclear(result);

        return std::any(count);
    }, FunctionSignature{{"int", "string", "array<string>"}, "int"});

    // __psql_prepare(handle: int, name: string, sql: string): bool
    // Creates a prepared statement
    registerFunction("psql", "__psql_prepare", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto name = std::any_cast<std::string>(args[1]);
        auto sql = std::any_cast<std::string>(args[2]);
        PGconn* conn = GET_PSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed PostgreSQL connection handle");
        }

        PGresult* result = PQprepare(conn, name.c_str(), sql.c_str(), 0, nullptr);
        ExecStatusType status = PQresultStatus(result);

        if (status != PGRES_COMMAND_OK) {
            std::string error = "PostgreSQL prepare error: " + std::string(PQerrorMessage(conn));
            PQclear(result);
            throw std::runtime_error(error);
        }

        PQclear(result);
        return std::any(true);
    }, FunctionSignature{{"int", "string", "string"}, "bool"});

    // __psql_exec_prepared(handle: int, name: string, params: array<string>): int (result ID)
    // Executes a prepared statement
    registerFunction("psql", "__psql_exec_prepared", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto name = std::any_cast<std::string>(args[1]);
        auto params = std::any_cast<std::vector<std::string>>(args[2]);
        PGconn* conn = GET_PSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed PostgreSQL connection handle");
        }

        std::vector<const char*> paramValues;
        for (const auto& p : params) {
            paramValues.push_back(p.c_str());
        }

        PGresult* result = PQexecPrepared(conn, name.c_str(),
            static_cast<int>(params.size()),
            paramValues.data(), nullptr, nullptr, 0);

        ExecStatusType status = PQresultStatus(result);
        if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
            std::string error = "PostgreSQL exec prepared error: " + std::string(PQerrorMessage(conn));
            PQclear(result);
            throw std::runtime_error(error);
        }

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        int resultId = ++g_psql_result_counter;
        g_psql_results[resultId] = result;
        g_psql_result_row[resultId] = -1;

        return std::any(resultId);
    }, FunctionSignature{{"int", "string", "array<string>"}, "int"});

    // __psql_begin(handle: int): bool
    // Begins a transaction
    registerFunction("psql", "__psql_begin", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        PGconn* conn = GET_PSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed PostgreSQL connection handle");
        }

        PGresult* result = PQexec(conn, "BEGIN");
        ExecStatusType status = PQresultStatus(result);
        PQclear(result);

        if (status != PGRES_COMMAND_OK) {
            throw std::runtime_error("Failed to begin transaction: " + std::string(PQerrorMessage(conn)));
        }

        return std::any(true);
    }, FunctionSignature{{"int"}, "bool"});

    // __psql_commit(handle: int): bool
    registerFunction("psql", "__psql_commit", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        PGconn* conn = GET_PSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed PostgreSQL connection handle");
        }

        PGresult* result = PQexec(conn, "COMMIT");
        ExecStatusType status = PQresultStatus(result);
        PQclear(result);

        if (status != PGRES_COMMAND_OK) {
            throw std::runtime_error("Failed to commit transaction: " + std::string(PQerrorMessage(conn)));
        }

        return std::any(true);
    }, FunctionSignature{{"int"}, "bool"});

    // __psql_rollback(handle: int): bool
    registerFunction("psql", "__psql_rollback", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        PGconn* conn = GET_PSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed PostgreSQL connection handle");
        }

        PGresult* result = PQexec(conn, "ROLLBACK");
        ExecStatusType status = PQresultStatus(result);
        PQclear(result);

        if (status != PGRES_COMMAND_OK) {
            throw std::runtime_error("Failed to rollback transaction: " + std::string(PQerrorMessage(conn)));
        }

        return std::any(true);
    }, FunctionSignature{{"int"}, "bool"});

    // __psql_next(resultId: int): bool
    // Moves to next row in result set
    registerFunction("psql", "__psql_next", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        auto it = g_psql_results.find(resultId);
        if (it == g_psql_results.end()) {
            throw std::runtime_error("Invalid PostgreSQL result handle");
        }

        PGresult* result = it->second;
        int& currentRow = g_psql_result_row[resultId];
        currentRow++;

        return std::any(currentRow < PQntuples(result));
    }, FunctionSignature{{"int"}, "bool"});

    // __psql_get_int(resultId: int, col: int): int
    registerFunction("psql", "__psql_get_int", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        auto it = g_psql_results.find(resultId);
        if (it == g_psql_results.end()) {
            throw std::runtime_error("Invalid PostgreSQL result handle");
        }

        PGresult* result = it->second;
        int row = g_psql_result_row[resultId];

        if (PQgetisnull(result, row, col)) {
            return std::any(0);
        }

        const char* val = PQgetvalue(result, row, col);
        return std::any(std::stoi(val));
    }, FunctionSignature{{"int", "int"}, "int"});

    // __psql_get_double(resultId: int, col: int): double
    registerFunction("psql", "__psql_get_double", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        auto it = g_psql_results.find(resultId);
        if (it == g_psql_results.end()) {
            throw std::runtime_error("Invalid PostgreSQL result handle");
        }

        PGresult* result = it->second;
        int row = g_psql_result_row[resultId];

        if (PQgetisnull(result, row, col)) {
            return std::any(0.0);
        }

        const char* val = PQgetvalue(result, row, col);
        return std::any(std::stod(val));
    }, FunctionSignature{{"int", "int"}, "double"});

    // __psql_get_string(resultId: int, col: int): string
    registerFunction("psql", "__psql_get_string", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        auto it = g_psql_results.find(resultId);
        if (it == g_psql_results.end()) {
            throw std::runtime_error("Invalid PostgreSQL result handle");
        }

        PGresult* result = it->second;
        int row = g_psql_result_row[resultId];

        if (PQgetisnull(result, row, col)) {
            return std::any(std::string(""));
        }

        const char* val = PQgetvalue(result, row, col);
        return std::any(std::string(val));
    }, FunctionSignature{{"int", "int"}, "string"});

    // __psql_get_bool(resultId: int, col: int): bool
    registerFunction("psql", "__psql_get_bool", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        auto it = g_psql_results.find(resultId);
        if (it == g_psql_results.end()) {
            throw std::runtime_error("Invalid PostgreSQL result handle");
        }

        PGresult* result = it->second;
        int row = g_psql_result_row[resultId];

        if (PQgetisnull(result, row, col)) {
            return std::any(false);
        }

        const char* val = PQgetvalue(result, row, col);
        // PostgreSQL returns 't' for true, 'f' for false
        return std::any(val[0] == 't' || val[0] == 'T' || val[0] == '1');
    }, FunctionSignature{{"int", "int"}, "bool"});

    // __psql_is_null(resultId: int, col: int): bool
    registerFunction("psql", "__psql_is_null", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        auto it = g_psql_results.find(resultId);
        if (it == g_psql_results.end()) {
            throw std::runtime_error("Invalid PostgreSQL result handle");
        }

        PGresult* result = it->second;
        int row = g_psql_result_row[resultId];

        return std::any(PQgetisnull(result, row, col) == 1);
    }, FunctionSignature{{"int", "int"}, "bool"});

    // __psql_column_count(resultId: int): int
    registerFunction("psql", "__psql_column_count", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        auto it = g_psql_results.find(resultId);
        if (it == g_psql_results.end()) {
            throw std::runtime_error("Invalid PostgreSQL result handle");
        }

        return std::any(PQnfields(it->second));
    }, FunctionSignature{{"int"}, "int"});

    // __psql_column_name(resultId: int, col: int): string
    registerFunction("psql", "__psql_column_name", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        auto it = g_psql_results.find(resultId);
        if (it == g_psql_results.end()) {
            throw std::runtime_error("Invalid PostgreSQL result handle");
        }

        const char* name = PQfname(it->second, col);
        return std::any(std::string(name ? name : ""));
    }, FunctionSignature{{"int", "int"}, "string"});

    // __psql_column_index(resultId: int, name: string): int
    registerFunction("psql", "__psql_column_index", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto name = std::any_cast<std::string>(args[1]);

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        auto it = g_psql_results.find(resultId);
        if (it == g_psql_results.end()) {
            throw std::runtime_error("Invalid PostgreSQL result handle");
        }

        int index = PQfnumber(it->second, name.c_str());
        if (index < 0) {
            throw std::runtime_error("Column not found: " + name);
        }

        return std::any(index);
    }, FunctionSignature{{"int", "string"}, "int"});

    // __psql_row_count(resultId: int): int
    registerFunction("psql", "__psql_row_count", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        auto it = g_psql_results.find(resultId);
        if (it == g_psql_results.end()) {
            throw std::runtime_error("Invalid PostgreSQL result handle");
        }

        return std::any(PQntuples(it->second));
    }, FunctionSignature{{"int"}, "int"});

    // __psql_rows_close(resultId: int): void
    registerFunction("psql", "__psql_rows_close", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_psql_mutex);
        auto it = g_psql_results.find(resultId);
        if (it != g_psql_results.end()) {
            PQclear(it->second);
            g_psql_results.erase(it);
            g_psql_result_row.erase(resultId);
        }

        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // __psql_escape_string(handle: any, str: string): string
    registerFunction("psql", "__psql_escape_string", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto str = std::any_cast<std::string>(args[1]);
        PGconn* conn = reinterpret_cast<PGconn*>(handle);

        char* escaped = PQescapeLiteral(conn, str.c_str(), str.length());
        if (!escaped) {
            throw std::runtime_error("Failed to escape string: " + std::string(PQerrorMessage(conn)));
        }

        std::string result(escaped);
        PQfreemem(escaped);
        return std::any(result);
    }, FunctionSignature{{"any", "string"}, "string"});

    // __psql_escape_identifier(handle: any, str: string): string
    registerFunction("psql", "__psql_escape_identifier", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto str = std::any_cast<std::string>(args[1]);
        PGconn* conn = reinterpret_cast<PGconn*>(handle);

        char* escaped = PQescapeIdentifier(conn, str.c_str(), str.length());
        if (!escaped) {
            throw std::runtime_error("Failed to escape identifier: " + std::string(PQerrorMessage(conn)));
        }

        std::string result(escaped);
        PQfreemem(escaped);
        return std::any(result);
    }, FunctionSignature{{"any", "string"}, "string"});

    // __psql_status(handle: any): string
    registerFunction("psql", "__psql_status", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        PGconn* conn = reinterpret_cast<PGconn*>(handle);

        ConnStatusType status = PQstatus(conn);
        switch (status) {
            case CONNECTION_OK: return std::any(std::string("connected"));
            case CONNECTION_BAD: return std::any(std::string("disconnected"));
            default: return std::any(std::string("unknown"));
        }
    }, FunctionSignature{{"any"}, "string"});
}

// ============================================================================
// MySQL/MariaDB Module - Database Support
// ============================================================================

// Thread-safe storage for MySQL result sets
static std::mutex g_mysql_mutex;
static std::unordered_map<int, MYSQL_RES*> g_mysql_results;
static std::unordered_map<int, MYSQL_ROW> g_mysql_current_row;
static int g_mysql_result_counter = 0;

void NativeRegistry::initMySQL() {
    // __mysql_connect(host: string, user: string, password: string, database: string, port: int): int (handle ID)
    registerFunction("mysql", "__mysql_connect", [](const std::vector<std::any>& args) -> std::any {
        auto host = std::any_cast<std::string>(args[0]);
        auto user = std::any_cast<std::string>(args[1]);
        auto password = std::any_cast<std::string>(args[2]);
        auto database = std::any_cast<std::string>(args[3]);
        auto port = std::any_cast<int>(args[4]);

        MYSQL* conn = mysql_init(nullptr);
        if (!conn) {
            throw std::runtime_error("Failed to initialize MySQL connection");
        }

        if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(),
                               database.c_str(), static_cast<unsigned int>(port), nullptr, 0)) {
            std::string error = "Failed to connect to MySQL: " + std::string(mysql_error(conn));
            mysql_close(conn);
            throw std::runtime_error(error);
        }

        // Set UTF-8 charset
        mysql_set_character_set(conn, "utf8mb4");

        int handle = ResourceManager::instance().registerResource(conn, ResourceType::MySQLConn);
        return std::any(handle);
    }, FunctionSignature{{"string", "string", "string", "string", "int"}, "int"});

    // __mysql_close(handle: int): void
    registerFunction("mysql", "__mysql_close", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        MYSQL* conn = GET_MYSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or already closed MySQL connection handle");
        }
        mysql_close(conn);
        ResourceManager::instance().closeResource(handle);
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // __mysql_exec(handle: int, sql: string): int
    registerFunction("mysql", "__mysql_exec", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        MYSQL* conn = GET_MYSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed MySQL connection handle");
        }

        if (mysql_query(conn, sql.c_str()) != 0) {
            throw std::runtime_error("MySQL error: " + std::string(mysql_error(conn)));
        }

        return std::any(static_cast<int>(mysql_affected_rows(conn)));
    }, FunctionSignature{{"int", "string"}, "int"});

    // __mysql_query(handle: int, sql: string): int (result ID)
    registerFunction("mysql", "__mysql_query", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        MYSQL* conn = GET_MYSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed MySQL connection handle");
        }

        if (mysql_query(conn, sql.c_str()) != 0) {
            throw std::runtime_error("MySQL query error: " + std::string(mysql_error(conn)));
        }

        MYSQL_RES* result = mysql_store_result(conn);
        if (!result && mysql_field_count(conn) > 0) {
            throw std::runtime_error("MySQL error retrieving result: " + std::string(mysql_error(conn)));
        }

        std::lock_guard<std::mutex> lock(g_mysql_mutex);
        int resultId = ++g_mysql_result_counter;
        g_mysql_results[resultId] = result;
        g_mysql_current_row[resultId] = nullptr;

        return std::any(resultId);
    }, FunctionSignature{{"int", "string"}, "int"});

    // __mysql_prepare(handle: int, sql: string): int - NOTE: MySQL prepared statements require different handling
    registerFunction("mysql", "__mysql_prepare", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto sql = std::any_cast<std::string>(args[1]);
        MYSQL* conn = GET_MYSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed MySQL connection handle");
        }

        MYSQL_STMT* stmt = mysql_stmt_init(conn);
        if (!stmt) {
            throw std::runtime_error("Failed to initialize MySQL statement");
        }

        if (mysql_stmt_prepare(stmt, sql.c_str(), static_cast<unsigned long>(sql.length())) != 0) {
            std::string error = "MySQL prepare error: " + std::string(mysql_stmt_error(stmt));
            mysql_stmt_close(stmt);
            throw std::runtime_error(error);
        }

        // Store stmt pointer - MySQL statements don't fit cleanly into ResourceManager
        // because they require the connection to remain open
        return std::any(reinterpret_cast<void*>(stmt));
    }, FunctionSignature{{"int", "string"}, "any"});

    // __mysql_stmt_close(handle: any): void - stmt handles remain as void*
    registerFunction("mysql", "__mysql_stmt_close", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        if (!handle) {
            throw std::runtime_error("Invalid MySQL statement handle");
        }
        MYSQL_STMT* stmt = reinterpret_cast<MYSQL_STMT*>(handle);
        mysql_stmt_close(stmt);
        return std::any();
    }, FunctionSignature{{"any"}, "void"});

    // __mysql_begin(handle: int): bool
    registerFunction("mysql", "__mysql_begin", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        MYSQL* conn = GET_MYSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed MySQL connection handle");
        }

        if (mysql_query(conn, "START TRANSACTION") != 0) {
            throw std::runtime_error("Failed to begin transaction: " + std::string(mysql_error(conn)));
        }

        return std::any(true);
    }, FunctionSignature{{"int"}, "bool"});

    // __mysql_commit(handle: int): bool
    registerFunction("mysql", "__mysql_commit", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        MYSQL* conn = GET_MYSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed MySQL connection handle");
        }

        if (mysql_commit(conn) != 0) {
            throw std::runtime_error("Failed to commit transaction: " + std::string(mysql_error(conn)));
        }

        return std::any(true);
    }, FunctionSignature{{"int"}, "bool"});

    // __mysql_rollback(handle: int): bool
    registerFunction("mysql", "__mysql_rollback", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        MYSQL* conn = GET_MYSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed MySQL connection handle");
        }

        if (mysql_rollback(conn) != 0) {
            throw std::runtime_error("Failed to rollback transaction: " + std::string(mysql_error(conn)));
        }

        return std::any(true);
    }, FunctionSignature{{"int"}, "bool"});

    // __mysql_next(resultId: int): bool
    registerFunction("mysql", "__mysql_next", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_mysql_mutex);
        auto it = g_mysql_results.find(resultId);
        if (it == g_mysql_results.end() || !it->second) {
            return std::any(false);
        }

        MYSQL_ROW row = mysql_fetch_row(it->second);
        g_mysql_current_row[resultId] = row;

        return std::any(row != nullptr);
    }, FunctionSignature{{"int"}, "bool"});

    // __mysql_get_int(resultId: int, col: int): int
    registerFunction("mysql", "__mysql_get_int", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_mysql_mutex);
        auto it = g_mysql_results.find(resultId);
        if (it == g_mysql_results.end() || !it->second) {
            throw std::runtime_error("Invalid MySQL result handle");
        }
        if (col < 0 || col >= static_cast<int>(mysql_num_fields(it->second))) {
            throw std::runtime_error("Column index out of bounds");
        }
        auto row = g_mysql_current_row[resultId];
        if (!row || !row[col]) {
            return std::any(0);
        }

        return std::any(std::stoi(row[col]));
    }, FunctionSignature{{"int", "int"}, "int"});

    // __mysql_get_double(resultId: int, col: int): double
    registerFunction("mysql", "__mysql_get_double", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_mysql_mutex);
        auto it = g_mysql_results.find(resultId);
        if (it == g_mysql_results.end() || !it->second) {
            throw std::runtime_error("Invalid MySQL result handle");
        }
        if (col < 0 || col >= static_cast<int>(mysql_num_fields(it->second))) {
            throw std::runtime_error("Column index out of bounds");
        }
        auto row = g_mysql_current_row[resultId];
        if (!row || !row[col]) {
            return std::any(0.0);
        }

        return std::any(std::stod(row[col]));
    }, FunctionSignature{{"int", "int"}, "double"});

    // __mysql_get_string(resultId: int, col: int): string
    registerFunction("mysql", "__mysql_get_string", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_mysql_mutex);
        auto it = g_mysql_results.find(resultId);
        if (it == g_mysql_results.end() || !it->second) {
            throw std::runtime_error("Invalid MySQL result handle");
        }
        if (col < 0 || col >= static_cast<int>(mysql_num_fields(it->second))) {
            throw std::runtime_error("Column index out of bounds");
        }
        auto row = g_mysql_current_row[resultId];
        if (!row || !row[col]) {
            return std::any(std::string(""));
        }

        return std::any(std::string(row[col]));
    }, FunctionSignature{{"int", "int"}, "string"});

    // __mysql_get_bool(resultId: int, col: int): bool
    registerFunction("mysql", "__mysql_get_bool", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_mysql_mutex);
        auto it = g_mysql_results.find(resultId);
        if (it == g_mysql_results.end() || !it->second) {
            throw std::runtime_error("Invalid MySQL result handle");
        }
        if (col < 0 || col >= static_cast<int>(mysql_num_fields(it->second))) {
            throw std::runtime_error("Column index out of bounds");
        }
        auto row = g_mysql_current_row[resultId];
        if (!row || !row[col]) {
            return std::any(false);
        }

        return std::any(row[col][0] == '1' || row[col][0] == 't' || row[col][0] == 'T');
    }, FunctionSignature{{"int", "int"}, "bool"});

    // __mysql_is_null(resultId: int, col: int): bool
    registerFunction("mysql", "__mysql_is_null", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_mysql_mutex);
        auto it = g_mysql_results.find(resultId);
        if (it == g_mysql_results.end() || !it->second) {
            throw std::runtime_error("Invalid MySQL result handle");
        }
        if (col < 0 || col >= static_cast<int>(mysql_num_fields(it->second))) {
            throw std::runtime_error("Column index out of bounds");
        }
        auto row = g_mysql_current_row[resultId];
        return std::any(!row || !row[col]);
    }, FunctionSignature{{"int", "int"}, "bool"});

    // __mysql_column_count(resultId: int): int
    registerFunction("mysql", "__mysql_column_count", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_mysql_mutex);
        auto it = g_mysql_results.find(resultId);
        if (it == g_mysql_results.end() || !it->second) {
            return std::any(0);
        }

        return std::any(static_cast<int>(mysql_num_fields(it->second)));
    }, FunctionSignature{{"int"}, "int"});

    // __mysql_column_name(resultId: int, col: int): string
    registerFunction("mysql", "__mysql_column_name", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);
        auto col = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_mysql_mutex);
        auto it = g_mysql_results.find(resultId);
        if (it == g_mysql_results.end() || !it->second) {
            return std::any(std::string(""));
        }

        MYSQL_FIELD* fields = mysql_fetch_fields(it->second);
        unsigned int numFields = mysql_num_fields(it->second);

        if (col < 0 || col >= static_cast<int>(numFields)) {
            return std::any(std::string(""));
        }

        return std::any(std::string(fields[col].name));
    }, FunctionSignature{{"int", "int"}, "string"});

    // __mysql_rows_close(resultId: int): void
    registerFunction("mysql", "__mysql_rows_close", [](const std::vector<std::any>& args) -> std::any {
        auto resultId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_mysql_mutex);
        auto it = g_mysql_results.find(resultId);
        if (it != g_mysql_results.end()) {
            if (it->second) {
                mysql_free_result(it->second);
            }
            g_mysql_results.erase(it);
            g_mysql_current_row.erase(resultId);
        }

        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // __mysql_last_insert_id(handle: int): int
    registerFunction("mysql", "__mysql_last_insert_id", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        MYSQL* conn = GET_MYSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed MySQL connection handle");
        }

        return std::any(static_cast<int>(mysql_insert_id(conn)));
    }, FunctionSignature{{"int"}, "int"});

    // __mysql_escape_string(handle: int, str: string): string
    registerFunction("mysql", "__mysql_escape_string", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto str = std::any_cast<std::string>(args[1]);
        MYSQL* conn = GET_MYSQL_CONN(handle);
        if (!conn) {
            throw std::runtime_error("Invalid or closed MySQL connection handle");
        }

        std::vector<char> escaped(str.length() * 2 + 1);
        unsigned long len = mysql_real_escape_string(conn, escaped.data(), str.c_str(),
                                                      static_cast<unsigned long>(str.length()));

        return std::any(std::string(escaped.data(), len));
    }, FunctionSignature{{"int", "string"}, "string"});
}

// ============================================================================
// Redis Module - Key-Value Store Support
// ============================================================================

void NativeRegistry::initRedis() {
    // __redis_connect(host: string, port: int): int (handle ID)
    registerFunction("redis", "__redis_connect", [](const std::vector<std::any>& args) -> std::any {
        auto host = std::any_cast<std::string>(args[0]);
        auto port = std::any_cast<int>(args[1]);

        redisContext* ctx = redisConnect(host.c_str(), port);
        if (!ctx) {
            throw std::runtime_error("Failed to allocate Redis context");
        }

        if (ctx->err) {
            std::string error = "Redis connection error: " + std::string(ctx->errstr);
            redisFree(ctx);
            throw std::runtime_error(error);
        }

        int handle = ResourceManager::instance().registerResource(ctx, ResourceType::RedisContext);
        return std::any(handle);
    }, FunctionSignature{{"string", "int"}, "int"});

    // __redis_connect_auth(host: string, port: int, password: string): int (handle ID)
    registerFunction("redis", "__redis_connect_auth", [](const std::vector<std::any>& args) -> std::any {
        auto host = std::any_cast<std::string>(args[0]);
        auto port = std::any_cast<int>(args[1]);
        auto password = std::any_cast<std::string>(args[2]);

        redisContext* ctx = redisConnect(host.c_str(), port);
        if (!ctx) {
            throw std::runtime_error("Failed to allocate Redis context");
        }

        if (ctx->err) {
            std::string error = "Redis connection error: " + std::string(ctx->errstr);
            redisFree(ctx);
            throw std::runtime_error(error);
        }

        // Authenticate
        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "AUTH %s", password.c_str()));
        if (!reply) {
            std::string error = "Redis auth error: " + std::string(ctx->errstr);
            redisFree(ctx);
            throw std::runtime_error(error);
        }

        if (reply->type == REDIS_REPLY_ERROR) {
            std::string error = "Redis auth failed: " + std::string(reply->str);
            freeReplyObject(reply);
            redisFree(ctx);
            throw std::runtime_error(error);
        }

        freeReplyObject(reply);
        int handle = ResourceManager::instance().registerResource(ctx, ResourceType::RedisContext);
        return std::any(handle);
    }, FunctionSignature{{"string", "int", "string"}, "int"});

    // __redis_close(handle: int): void
    registerFunction("redis", "__redis_close", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        redisContext* ctx = GET_REDIS_CTX(handle);
        if (!ctx) {
            throw std::runtime_error("Invalid or already closed Redis connection handle");
        }
        redisFree(ctx);
        ResourceManager::instance().closeResource(handle);
        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // __redis_get(handle: int, key: string): string
    registerFunction("redis", "__redis_get", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = GET_REDIS_CTX(handle);
        if (!ctx) {
            throw std::runtime_error("Invalid or closed Redis connection handle");
        }

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "GET %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis GET error: " + std::string(ctx->errstr));
        }

        std::string result;
        if (reply->type == REDIS_REPLY_STRING) {
            result = std::string(reply->str, reply->len);
        }

        freeReplyObject(reply);
        return std::any(result);
    }, FunctionSignature{{"int", "string"}, "string"});

    // __redis_set(handle: int, key: string, value: string): bool
    registerFunction("redis", "__redis_set", [](const std::vector<std::any>& args) -> std::any {
        int handle = std::any_cast<int>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto value = std::any_cast<std::string>(args[2]);
        redisContext* ctx = GET_REDIS_CTX(handle);
        if (!ctx) {
            throw std::runtime_error("Invalid or closed Redis connection handle");
        }

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "SET %s %s", key.c_str(), value.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis SET error: " + std::string(ctx->errstr));
        }

        bool success = (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0);
        freeReplyObject(reply);
        return std::any(success);
    }, FunctionSignature{{"any", "string", "string"}, "bool"});

    // __redis_setex(handle: any, key: string, value: string, seconds: int): bool
    registerFunction("redis", "__redis_setex", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto value = std::any_cast<std::string>(args[2]);
        auto seconds = std::any_cast<int>(args[3]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "SETEX %s %d %s",
            key.c_str(), seconds, value.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis SETEX error: " + std::string(ctx->errstr));
        }

        bool success = (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0);
        freeReplyObject(reply);
        return std::any(success);
    }, FunctionSignature{{"any", "string", "string", "int"}, "bool"});

    // __redis_del(handle: any, key: string): int
    registerFunction("redis", "__redis_del", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "DEL %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis DEL error: " + std::string(ctx->errstr));
        }

        int deleted = (reply->type == REDIS_REPLY_INTEGER) ? static_cast<int>(reply->integer) : 0;
        freeReplyObject(reply);
        return std::any(deleted);
    }, FunctionSignature{{"any", "string"}, "int"});

    // __redis_exists(handle: any, key: string): bool
    registerFunction("redis", "__redis_exists", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "EXISTS %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis EXISTS error: " + std::string(ctx->errstr));
        }

        bool exists = (reply->type == REDIS_REPLY_INTEGER && reply->integer > 0);
        freeReplyObject(reply);
        return std::any(exists);
    }, FunctionSignature{{"any", "string"}, "bool"});

    // __redis_expire(handle: any, key: string, seconds: int): bool
    registerFunction("redis", "__redis_expire", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto seconds = std::any_cast<int>(args[2]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "EXPIRE %s %d", key.c_str(), seconds));
        if (!reply) {
            throw std::runtime_error("Redis EXPIRE error: " + std::string(ctx->errstr));
        }

        bool success = (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1);
        freeReplyObject(reply);
        return std::any(success);
    }, FunctionSignature{{"any", "string", "int"}, "bool"});

    // __redis_ttl(handle: any, key: string): int
    registerFunction("redis", "__redis_ttl", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "TTL %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis TTL error: " + std::string(ctx->errstr));
        }

        int ttl = (reply->type == REDIS_REPLY_INTEGER) ? static_cast<int>(reply->integer) : -2;
        freeReplyObject(reply);
        return std::any(ttl);
    }, FunctionSignature{{"any", "string"}, "int"});

    // __redis_incr(handle: any, key: string): int
    registerFunction("redis", "__redis_incr", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "INCR %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis INCR error: " + std::string(ctx->errstr));
        }

        if (reply->type == REDIS_REPLY_ERROR) {
            std::string error = "Redis INCR error: " + std::string(reply->str);
            freeReplyObject(reply);
            throw std::runtime_error(error);
        }

        int value = (reply->type == REDIS_REPLY_INTEGER) ? static_cast<int>(reply->integer) : 0;
        freeReplyObject(reply);
        return std::any(value);
    }, FunctionSignature{{"any", "string"}, "int"});

    // __redis_decr(handle: any, key: string): int
    registerFunction("redis", "__redis_decr", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "DECR %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis DECR error: " + std::string(ctx->errstr));
        }

        if (reply->type == REDIS_REPLY_ERROR) {
            std::string error = "Redis DECR error: " + std::string(reply->str);
            freeReplyObject(reply);
            throw std::runtime_error(error);
        }

        int value = (reply->type == REDIS_REPLY_INTEGER) ? static_cast<int>(reply->integer) : 0;
        freeReplyObject(reply);
        return std::any(value);
    }, FunctionSignature{{"any", "string"}, "int"});

    // Hash operations

    // __redis_hget(handle: any, key: string, field: string): string
    registerFunction("redis", "__redis_hget", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto field = std::any_cast<std::string>(args[2]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "HGET %s %s", key.c_str(), field.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis HGET error: " + std::string(ctx->errstr));
        }

        std::string result;
        if (reply->type == REDIS_REPLY_STRING) {
            result = std::string(reply->str, reply->len);
        }

        freeReplyObject(reply);
        return std::any(result);
    }, FunctionSignature{{"any", "string", "string"}, "string"});

    // __redis_hset(handle: any, key: string, field: string, value: string): bool
    registerFunction("redis", "__redis_hset", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto field = std::any_cast<std::string>(args[2]);
        auto value = std::any_cast<std::string>(args[3]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "HSET %s %s %s",
            key.c_str(), field.c_str(), value.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis HSET error: " + std::string(ctx->errstr));
        }

        freeReplyObject(reply);
        return std::any(true);
    }, FunctionSignature{{"any", "string", "string", "string"}, "bool"});

    // __redis_hdel(handle: any, key: string, field: string): int
    registerFunction("redis", "__redis_hdel", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto field = std::any_cast<std::string>(args[2]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "HDEL %s %s", key.c_str(), field.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis HDEL error: " + std::string(ctx->errstr));
        }

        int deleted = (reply->type == REDIS_REPLY_INTEGER) ? static_cast<int>(reply->integer) : 0;
        freeReplyObject(reply);
        return std::any(deleted);
    }, FunctionSignature{{"any", "string", "string"}, "int"});

    // __redis_hexists(handle: any, key: string, field: string): bool
    registerFunction("redis", "__redis_hexists", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto field = std::any_cast<std::string>(args[2]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "HEXISTS %s %s", key.c_str(), field.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis HEXISTS error: " + std::string(ctx->errstr));
        }

        bool exists = (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1);
        freeReplyObject(reply);
        return std::any(exists);
    }, FunctionSignature{{"any", "string", "string"}, "bool"});

    // __redis_hgetall(handle: any, key: string): array<string>
    // Returns alternating field/value pairs: [field1, value1, field2, value2, ...]
    registerFunction("redis", "__redis_hgetall", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "HGETALL %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis HGETALL error: " + std::string(ctx->errstr));
        }

        std::vector<std::string> result;
        if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < reply->elements; i++) {
                if (reply->element[i]->type == REDIS_REPLY_STRING) {
                    result.push_back(std::string(reply->element[i]->str, reply->element[i]->len));
                }
            }
        }

        freeReplyObject(reply);
        return std::any(result);
    }, FunctionSignature{{"any", "string"}, "array<string>"});

    // __redis_hkeys(handle: any, key: string): array<string>
    registerFunction("redis", "__redis_hkeys", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "HKEYS %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis HKEYS error: " + std::string(ctx->errstr));
        }

        std::vector<std::string> result;
        if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < reply->elements; i++) {
                if (reply->element[i]->type == REDIS_REPLY_STRING) {
                    result.push_back(std::string(reply->element[i]->str, reply->element[i]->len));
                }
            }
        }

        freeReplyObject(reply);
        return std::any(result);
    }, FunctionSignature{{"any", "string"}, "array<string>"});

    // List operations

    // __redis_lpush(handle: any, key: string, value: string): int
    registerFunction("redis", "__redis_lpush", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto value = std::any_cast<std::string>(args[2]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "LPUSH %s %s", key.c_str(), value.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis LPUSH error: " + std::string(ctx->errstr));
        }

        int len = (reply->type == REDIS_REPLY_INTEGER) ? static_cast<int>(reply->integer) : 0;
        freeReplyObject(reply);
        return std::any(len);
    }, FunctionSignature{{"any", "string", "string"}, "int"});

    // __redis_rpush(handle: any, key: string, value: string): int
    registerFunction("redis", "__redis_rpush", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto value = std::any_cast<std::string>(args[2]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "RPUSH %s %s", key.c_str(), value.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis RPUSH error: " + std::string(ctx->errstr));
        }

        int len = (reply->type == REDIS_REPLY_INTEGER) ? static_cast<int>(reply->integer) : 0;
        freeReplyObject(reply);
        return std::any(len);
    }, FunctionSignature{{"any", "string", "string"}, "int"});

    // __redis_lpop(handle: any, key: string): string
    registerFunction("redis", "__redis_lpop", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "LPOP %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis LPOP error: " + std::string(ctx->errstr));
        }

        std::string result;
        if (reply->type == REDIS_REPLY_STRING) {
            result = std::string(reply->str, reply->len);
        }

        freeReplyObject(reply);
        return std::any(result);
    }, FunctionSignature{{"any", "string"}, "string"});

    // __redis_rpop(handle: any, key: string): string
    registerFunction("redis", "__redis_rpop", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "RPOP %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis RPOP error: " + std::string(ctx->errstr));
        }

        std::string result;
        if (reply->type == REDIS_REPLY_STRING) {
            result = std::string(reply->str, reply->len);
        }

        freeReplyObject(reply);
        return std::any(result);
    }, FunctionSignature{{"any", "string"}, "string"});

    // __redis_lrange(handle: any, key: string, start: int, stop: int): array<string>
    registerFunction("redis", "__redis_lrange", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto start = std::any_cast<int>(args[2]);
        auto stop = std::any_cast<int>(args[3]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "LRANGE %s %d %d",
            key.c_str(), start, stop));
        if (!reply) {
            throw std::runtime_error("Redis LRANGE error: " + std::string(ctx->errstr));
        }

        std::vector<std::string> result;
        if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < reply->elements; i++) {
                if (reply->element[i]->type == REDIS_REPLY_STRING) {
                    result.push_back(std::string(reply->element[i]->str, reply->element[i]->len));
                }
            }
        }

        freeReplyObject(reply);
        return std::any(result);
    }, FunctionSignature{{"any", "string", "int", "int"}, "array<string>"});

    // __redis_llen(handle: any, key: string): int
    registerFunction("redis", "__redis_llen", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "LLEN %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis LLEN error: " + std::string(ctx->errstr));
        }

        int len = (reply->type == REDIS_REPLY_INTEGER) ? static_cast<int>(reply->integer) : 0;
        freeReplyObject(reply);
        return std::any(len);
    }, FunctionSignature{{"any", "string"}, "int"});

    // Set operations

    // __redis_sadd(handle: any, key: string, member: string): int
    registerFunction("redis", "__redis_sadd", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto member = std::any_cast<std::string>(args[2]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "SADD %s %s", key.c_str(), member.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis SADD error: " + std::string(ctx->errstr));
        }

        int added = (reply->type == REDIS_REPLY_INTEGER) ? static_cast<int>(reply->integer) : 0;
        freeReplyObject(reply);
        return std::any(added);
    }, FunctionSignature{{"any", "string", "string"}, "int"});

    // __redis_srem(handle: any, key: string, member: string): int
    registerFunction("redis", "__redis_srem", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto member = std::any_cast<std::string>(args[2]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "SREM %s %s", key.c_str(), member.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis SREM error: " + std::string(ctx->errstr));
        }

        int removed = (reply->type == REDIS_REPLY_INTEGER) ? static_cast<int>(reply->integer) : 0;
        freeReplyObject(reply);
        return std::any(removed);
    }, FunctionSignature{{"any", "string", "string"}, "int"});

    // __redis_sismember(handle: any, key: string, member: string): bool
    registerFunction("redis", "__redis_sismember", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto member = std::any_cast<std::string>(args[2]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "SISMEMBER %s %s", key.c_str(), member.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis SISMEMBER error: " + std::string(ctx->errstr));
        }

        bool isMember = (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1);
        freeReplyObject(reply);
        return std::any(isMember);
    }, FunctionSignature{{"any", "string", "string"}, "bool"});

    // __redis_smembers(handle: any, key: string): array<string>
    registerFunction("redis", "__redis_smembers", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "SMEMBERS %s", key.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis SMEMBERS error: " + std::string(ctx->errstr));
        }

        std::vector<std::string> result;
        if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < reply->elements; i++) {
                if (reply->element[i]->type == REDIS_REPLY_STRING) {
                    result.push_back(std::string(reply->element[i]->str, reply->element[i]->len));
                }
            }
        }

        freeReplyObject(reply);
        return std::any(result);
    }, FunctionSignature{{"any", "string"}, "array<string>"});

    // Pub/Sub operations

    // __redis_publish(handle: any, channel: string, message: string): int
    registerFunction("redis", "__redis_publish", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto channel = std::any_cast<std::string>(args[1]);
        auto message = std::any_cast<std::string>(args[2]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "PUBLISH %s %s",
            channel.c_str(), message.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis PUBLISH error: " + std::string(ctx->errstr));
        }

        int subscribers = (reply->type == REDIS_REPLY_INTEGER) ? static_cast<int>(reply->integer) : 0;
        freeReplyObject(reply);
        return std::any(subscribers);
    }, FunctionSignature{{"any", "string", "string"}, "int"});

    // __redis_ping(handle: any): bool
    registerFunction("redis", "__redis_ping", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "PING"));
        if (!reply) {
            return std::any(false);
        }

        bool success = (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "PONG") == 0);
        freeReplyObject(reply);
        return std::any(success);
    }, FunctionSignature{{"any"}, "bool"});

    // __redis_select(handle: any, db: int): bool
    registerFunction("redis", "__redis_select", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto db = std::any_cast<int>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "SELECT %d", db));
        if (!reply) {
            throw std::runtime_error("Redis SELECT error: " + std::string(ctx->errstr));
        }

        bool success = (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0);
        freeReplyObject(reply);
        return std::any(success);
    }, FunctionSignature{{"any", "int"}, "bool"});

    // __redis_flushdb(handle: any): bool
    registerFunction("redis", "__redis_flushdb", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "FLUSHDB"));
        if (!reply) {
            throw std::runtime_error("Redis FLUSHDB error: " + std::string(ctx->errstr));
        }

        bool success = (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0);
        freeReplyObject(reply);
        return std::any(success);
    }, FunctionSignature{{"any"}, "bool"});

    // __redis_keys(handle: any, pattern: string): array<string>
    registerFunction("redis", "__redis_keys", [](const std::vector<std::any>& args) -> std::any {
        auto handle = std::any_cast<void*>(args[0]);
        auto pattern = std::any_cast<std::string>(args[1]);
        redisContext* ctx = reinterpret_cast<redisContext*>(handle);

        redisReply* reply = static_cast<redisReply*>(redisCommand(ctx, "KEYS %s", pattern.c_str()));
        if (!reply) {
            throw std::runtime_error("Redis KEYS error: " + std::string(ctx->errstr));
        }

        std::vector<std::string> result;
        if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < reply->elements; i++) {
                if (reply->element[i]->type == REDIS_REPLY_STRING) {
                    result.push_back(std::string(reply->element[i]->str, reply->element[i]->len));
                }
            }
        }

        freeReplyObject(reply);
        return std::any(result);
    }, FunctionSignature{{"any", "string"}, "array<string>"});
}

// ============================================================================
// XML Module - XML Parsing Support
// ============================================================================

// Simple recursive descent XML parser (no external dependencies)
struct XmlNode {
    std::string tag;
    std::string text;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::shared_ptr<XmlNode>> children;
};

static std::mutex g_xml_mutex;
static std::unordered_map<int, std::shared_ptr<XmlNode>> g_xml_nodes;
static int g_xml_node_counter = 0;

static std::string xmlTrim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

static std::shared_ptr<XmlNode> parseXmlElement(const std::string& xml, size_t& pos);

static void skipWhitespace(const std::string& xml, size_t& pos) {
    while (pos < xml.length() && std::isspace(xml[pos])) {
        pos++;
    }
}

static std::string parseXmlName(const std::string& xml, size_t& pos) {
    std::string name;
    while (pos < xml.length() && (std::isalnum(xml[pos]) || xml[pos] == '_' || xml[pos] == '-' || xml[pos] == ':')) {
        name += xml[pos++];
    }
    return name;
}

static std::string parseXmlAttributeValue(const std::string& xml, size_t& pos) {
    if (pos >= xml.length() || (xml[pos] != '"' && xml[pos] != '\'')) {
        return "";
    }
    char quote = xml[pos++];
    std::string value;
    while (pos < xml.length() && xml[pos] != quote) {
        if (xml[pos] == '&') {
            // Handle basic XML entities
            if (xml.substr(pos, 4) == "&lt;") { value += '<'; pos += 4; }
            else if (xml.substr(pos, 4) == "&gt;") { value += '>'; pos += 4; }
            else if (xml.substr(pos, 5) == "&amp;") { value += '&'; pos += 5; }
            else if (xml.substr(pos, 6) == "&quot;") { value += '"'; pos += 6; }
            else if (xml.substr(pos, 6) == "&apos;") { value += '\''; pos += 6; }
            else { value += xml[pos++]; }
        } else {
            value += xml[pos++];
        }
    }
    if (pos < xml.length()) pos++;  // Skip closing quote
    return value;
}

static std::shared_ptr<XmlNode> parseXmlElement(const std::string& xml, size_t& pos) {
    skipWhitespace(xml, pos);

    // Skip XML declaration and comments
    while (pos < xml.length() && xml[pos] == '<') {
        if (xml.substr(pos, 4) == "<!--") {
            size_t end = xml.find("-->", pos);
            if (end != std::string::npos) { pos = end + 3; skipWhitespace(xml, pos); continue; }
        }
        if (xml.substr(pos, 5) == "<?xml") {
            size_t end = xml.find("?>", pos);
            if (end != std::string::npos) { pos = end + 2; skipWhitespace(xml, pos); continue; }
        }
        break;
    }

    if (pos >= xml.length() || xml[pos] != '<') {
        return nullptr;
    }
    pos++;  // Skip <

    auto node = std::make_shared<XmlNode>();
    node->tag = parseXmlName(xml, pos);
    if (node->tag.empty()) return nullptr;

    // Parse attributes
    while (pos < xml.length()) {
        skipWhitespace(xml, pos);
        if (xml[pos] == '/' || xml[pos] == '>') break;

        std::string attrName = parseXmlName(xml, pos);
        if (attrName.empty()) break;

        skipWhitespace(xml, pos);
        if (pos < xml.length() && xml[pos] == '=') {
            pos++;
            skipWhitespace(xml, pos);
            node->attributes[attrName] = parseXmlAttributeValue(xml, pos);
        } else {
            node->attributes[attrName] = "";
        }
    }

    skipWhitespace(xml, pos);

    // Self-closing tag
    if (pos < xml.length() && xml[pos] == '/') {
        pos++;
        if (pos < xml.length() && xml[pos] == '>') pos++;
        return node;
    }

    if (pos >= xml.length() || xml[pos] != '>') return nullptr;
    pos++;  // Skip >

    // Parse content
    std::string textContent;
    while (pos < xml.length()) {
        if (xml[pos] == '<') {
            if (xml.substr(pos, 2) == "</") {
                // Closing tag
                pos += 2;
                std::string closeTag = parseXmlName(xml, pos);
                skipWhitespace(xml, pos);
                if (pos < xml.length() && xml[pos] == '>') pos++;
                break;
            } else if (xml.substr(pos, 4) == "<!--") {
                // Skip comment
                size_t end = xml.find("-->", pos);
                if (end != std::string::npos) { pos = end + 3; continue; }
            } else {
                // Child element
                if (!textContent.empty()) {
                    node->text = xmlTrim(textContent);
                    textContent.clear();
                }
                auto child = parseXmlElement(xml, pos);
                if (child) {
                    node->children.push_back(child);
                }
            }
        } else {
            // Handle entities in text content
            if (xml[pos] == '&') {
                if (xml.substr(pos, 4) == "&lt;") { textContent += '<'; pos += 4; }
                else if (xml.substr(pos, 4) == "&gt;") { textContent += '>'; pos += 4; }
                else if (xml.substr(pos, 5) == "&amp;") { textContent += '&'; pos += 5; }
                else if (xml.substr(pos, 6) == "&quot;") { textContent += '"'; pos += 6; }
                else if (xml.substr(pos, 6) == "&apos;") { textContent += '\''; pos += 6; }
                else { textContent += xml[pos++]; }
            } else {
                textContent += xml[pos++];
            }
        }
    }

    if (!textContent.empty()) {
        node->text = xmlTrim(textContent);
    }

    return node;
}

static std::string xmlNodeToString(const std::shared_ptr<XmlNode>& node, int indent = 0) {
    if (!node) return "";

    std::string result;
    std::string indentStr(indent * 2, ' ');

    result += indentStr + "<" + node->tag;

    for (const auto& [key, value] : node->attributes) {
        result += " " + key + "=\"" + value + "\"";
    }

    if (node->children.empty() && node->text.empty()) {
        result += "/>\n";
    } else {
        result += ">";
        if (!node->text.empty()) {
            result += node->text;
        }
        if (!node->children.empty()) {
            result += "\n";
            for (const auto& child : node->children) {
                result += xmlNodeToString(child, indent + 1);
            }
            result += indentStr;
        }
        result += "</" + node->tag + ">\n";
    }

    return result;
}

void NativeRegistry::initXML() {
    // __xml_parse(xmlString: string): int
    // Parses XML and returns node handle
    registerFunction("xml", "__xml_parse", [](const std::vector<std::any>& args) -> std::any {
        auto xmlStr = std::any_cast<std::string>(args[0]);

        size_t pos = 0;
        auto node = parseXmlElement(xmlStr, pos);

        if (!node) {
            throw std::runtime_error("Failed to parse XML");
        }

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        int nodeId = ++g_xml_node_counter;
        g_xml_nodes[nodeId] = node;

        return std::any(nodeId);
    }, FunctionSignature{{"string"}, "int"});

    // __xml_stringify(nodeId: int): string
    registerFunction("xml", "__xml_stringify", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        return std::any(xmlNodeToString(it->second));
    }, FunctionSignature{{"int"}, "string"});

    // __xml_get_tag(nodeId: int): string
    registerFunction("xml", "__xml_get_tag", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        return std::any(it->second->tag);
    }, FunctionSignature{{"int"}, "string"});

    // __xml_get_text(nodeId: int): string
    registerFunction("xml", "__xml_get_text", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        return std::any(it->second->text);
    }, FunctionSignature{{"int"}, "string"});

    // __xml_get_attr(nodeId: int, name: string): string
    registerFunction("xml", "__xml_get_attr", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);
        auto name = std::any_cast<std::string>(args[1]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        auto attrIt = it->second->attributes.find(name);
        if (attrIt != it->second->attributes.end()) {
            return std::any(attrIt->second);
        }
        return std::any(std::string(""));
    }, FunctionSignature{{"int", "string"}, "string"});

    // __xml_has_attr(nodeId: int, name: string): bool
    registerFunction("xml", "__xml_has_attr", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);
        auto name = std::any_cast<std::string>(args[1]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        return std::any(it->second->attributes.find(name) != it->second->attributes.end());
    }, FunctionSignature{{"int", "string"}, "bool"});

    // __xml_get_attr_names(nodeId: int): array<string>
    registerFunction("xml", "__xml_get_attr_names", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        std::vector<std::string> names;
        for (const auto& [key, _] : it->second->attributes) {
            names.push_back(key);
        }
        return std::any(names);
    }, FunctionSignature{{"int"}, "array<string>"});

    // __xml_child_count(nodeId: int): int
    registerFunction("xml", "__xml_child_count", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        return std::any(static_cast<int>(it->second->children.size()));
    }, FunctionSignature{{"int"}, "int"});

    // __xml_get_child(nodeId: int, index: int): int
    registerFunction("xml", "__xml_get_child", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);
        auto index = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        if (index < 0 || index >= static_cast<int>(it->second->children.size())) {
            throw std::runtime_error("XML child index out of bounds");
        }

        auto child = it->second->children[index];
        int childId = ++g_xml_node_counter;
        g_xml_nodes[childId] = child;

        return std::any(childId);
    }, FunctionSignature{{"int", "int"}, "int"});

    // __xml_find_by_tag(nodeId: int, tag: string): array<int>
    // Finds all child elements with matching tag (non-recursive)
    registerFunction("xml", "__xml_find_by_tag", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);
        auto tag = std::any_cast<std::string>(args[1]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        std::vector<int> result;
        for (const auto& child : it->second->children) {
            if (child->tag == tag) {
                int childId = ++g_xml_node_counter;
                g_xml_nodes[childId] = child;
                result.push_back(childId);
            }
        }

        return std::any(result);
    }, FunctionSignature{{"int", "string"}, "array<int>"});

    // __xml_find_all_by_tag(nodeId: int, tag: string): array<int>
    // Finds all descendant elements with matching tag (recursive)
    registerFunction("xml", "__xml_find_all_by_tag", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);
        auto tag = std::any_cast<std::string>(args[1]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        std::vector<int> result;
        std::function<void(const std::shared_ptr<XmlNode>&)> findRecursive =
            [&](const std::shared_ptr<XmlNode>& node) {
                for (const auto& child : node->children) {
                    if (child->tag == tag) {
                        int childId = ++g_xml_node_counter;
                        g_xml_nodes[childId] = child;
                        result.push_back(childId);
                    }
                    findRecursive(child);
                }
            };

        findRecursive(it->second);
        return std::any(result);
    }, FunctionSignature{{"int", "string"}, "array<int>"});

    // __xml_free(nodeId: int): void
    registerFunction("xml", "__xml_free", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        g_xml_nodes.erase(nodeId);

        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // __xml_create(tag: string): int
    registerFunction("xml", "__xml_create", [](const std::vector<std::any>& args) -> std::any {
        auto tag = std::any_cast<std::string>(args[0]);

        auto node = std::make_shared<XmlNode>();
        node->tag = tag;

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        int nodeId = ++g_xml_node_counter;
        g_xml_nodes[nodeId] = node;

        return std::any(nodeId);
    }, FunctionSignature{{"string"}, "int"});

    // __xml_set_text(nodeId: int, text: string): void
    registerFunction("xml", "__xml_set_text", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);
        auto text = std::any_cast<std::string>(args[1]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        it->second->text = text;
        return std::any();
    }, FunctionSignature{{"int", "string"}, "void"});

    // __xml_set_attr(nodeId: int, name: string, value: string): void
    registerFunction("xml", "__xml_set_attr", [](const std::vector<std::any>& args) -> std::any {
        auto nodeId = std::any_cast<int>(args[0]);
        auto name = std::any_cast<std::string>(args[1]);
        auto value = std::any_cast<std::string>(args[2]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto it = g_xml_nodes.find(nodeId);
        if (it == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        it->second->attributes[name] = value;
        return std::any();
    }, FunctionSignature{{"int", "string", "string"}, "void"});

    // __xml_add_child(parentId: int, childId: int): void
    registerFunction("xml", "__xml_add_child", [](const std::vector<std::any>& args) -> std::any {
        auto parentId = std::any_cast<int>(args[0]);
        auto childId = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_xml_mutex);
        auto parentIt = g_xml_nodes.find(parentId);
        auto childIt = g_xml_nodes.find(childId);

        if (parentIt == g_xml_nodes.end() || childIt == g_xml_nodes.end()) {
            throw std::runtime_error("Invalid XML node handle");
        }

        parentIt->second->children.push_back(childIt->second);
        return std::any();
    }, FunctionSignature{{"int", "int"}, "void"});
}

// ============================================================================
// Template Module - HTML Templating Support
// ============================================================================

static std::string templateEscapeHtml(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 1.1);
    for (char c : str) {
        switch (c) {
            case '&': result += "&amp;"; break;
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c;
        }
    }
    return result;
}

// Simple template data context using nested maps
struct TemplateContext {
    std::unordered_map<std::string, std::string> strings;
    std::unordered_map<std::string, int> integers;
    std::unordered_map<std::string, double> doubles;
    std::unordered_map<std::string, bool> booleans;
    std::unordered_map<std::string, std::vector<std::string>> arrays;
};

static std::mutex g_template_mutex;
static std::unordered_map<int, TemplateContext> g_template_contexts;
static int g_template_context_counter = 0;

static std::string templateGetValue(const TemplateContext& ctx, const std::string& key) {
    auto strIt = ctx.strings.find(key);
    if (strIt != ctx.strings.end()) return strIt->second;

    auto intIt = ctx.integers.find(key);
    if (intIt != ctx.integers.end()) return std::to_string(intIt->second);

    auto dblIt = ctx.doubles.find(key);
    if (dblIt != ctx.doubles.end()) return std::to_string(dblIt->second);

    auto boolIt = ctx.booleans.find(key);
    if (boolIt != ctx.booleans.end()) return boolIt->second ? "true" : "false";

    return "";
}

static bool templateGetBool(const TemplateContext& ctx, const std::string& key) {
    auto boolIt = ctx.booleans.find(key);
    if (boolIt != ctx.booleans.end()) return boolIt->second;

    auto strIt = ctx.strings.find(key);
    if (strIt != ctx.strings.end()) return !strIt->second.empty();

    auto intIt = ctx.integers.find(key);
    if (intIt != ctx.integers.end()) return intIt->second != 0;

    auto arrIt = ctx.arrays.find(key);
    if (arrIt != ctx.arrays.end()) return !arrIt->second.empty();

    return false;
}

static std::string templateRender(const std::string& tmpl, const TemplateContext& ctx);

static std::string templateRender(const std::string& tmpl, const TemplateContext& ctx) {
    std::string result;
    size_t pos = 0;

    while (pos < tmpl.length()) {
        // Check for {{ variable }}
        if (tmpl.substr(pos, 2) == "{{") {
            size_t endPos = tmpl.find("}}", pos + 2);
            if (endPos != std::string::npos) {
                std::string expr = xmlTrim(tmpl.substr(pos + 2, endPos - pos - 2));

                // Check for raw output (triple braces or |raw filter)
                bool escapeHtml = true;
                if (expr.length() > 0 && expr[0] == '{') {
                    // Triple brace - raw output
                    size_t tripleEnd = tmpl.find("}}}", pos + 2);
                    if (tripleEnd != std::string::npos) {
                        expr = xmlTrim(tmpl.substr(pos + 3, tripleEnd - pos - 3));
                        escapeHtml = false;
                        endPos = tripleEnd + 1;
                    }
                } else if (expr.find("|raw") != std::string::npos) {
                    expr = xmlTrim(expr.substr(0, expr.find("|raw")));
                    escapeHtml = false;
                }

                std::string value = templateGetValue(ctx, expr);
                result += escapeHtml ? templateEscapeHtml(value) : value;
                pos = endPos + 2;
                continue;
            }
        }

        // Check for {% if condition %}
        if (tmpl.substr(pos, 2) == "{%") {
            size_t endPos = tmpl.find("%}", pos + 2);
            if (endPos != std::string::npos) {
                std::string tag = xmlTrim(tmpl.substr(pos + 2, endPos - pos - 2));

                if (tag.substr(0, 3) == "if ") {
                    std::string condition = xmlTrim(tag.substr(3));
                    bool negate = false;
                    if (condition.substr(0, 4) == "not ") {
                        negate = true;
                        condition = xmlTrim(condition.substr(4));
                    }

                    // Find matching endif
                    size_t ifDepth = 1;
                    size_t searchPos = endPos + 2;
                    size_t elsePos = std::string::npos;
                    size_t endifPos = std::string::npos;

                    while (searchPos < tmpl.length() && ifDepth > 0) {
                        size_t nextTag = tmpl.find("{%", searchPos);
                        if (nextTag == std::string::npos) break;

                        size_t tagEnd = tmpl.find("%}", nextTag + 2);
                        if (tagEnd == std::string::npos) break;

                        std::string innerTag = xmlTrim(tmpl.substr(nextTag + 2, tagEnd - nextTag - 2));

                        if (innerTag.substr(0, 3) == "if ") {
                            ifDepth++;
                        } else if (innerTag == "endif") {
                            ifDepth--;
                            if (ifDepth == 0) {
                                endifPos = nextTag;
                            }
                        } else if (innerTag == "else" && ifDepth == 1) {
                            elsePos = nextTag;
                        }

                        searchPos = tagEnd + 2;
                    }

                    if (endifPos != std::string::npos) {
                        bool conditionTrue = templateGetBool(ctx, condition);
                        if (negate) conditionTrue = !conditionTrue;

                        std::string block;
                        if (conditionTrue) {
                            if (elsePos != std::string::npos) {
                                block = tmpl.substr(endPos + 2, elsePos - endPos - 2);
                            } else {
                                block = tmpl.substr(endPos + 2, endifPos - endPos - 2);
                            }
                        } else if (elsePos != std::string::npos) {
                            size_t elseEnd = tmpl.find("%}", elsePos + 2);
                            block = tmpl.substr(elseEnd + 2, endifPos - elseEnd - 2);
                        }

                        result += templateRender(block, ctx);

                        // Skip to after endif
                        size_t endifEnd = tmpl.find("%}", endifPos + 2);
                        pos = endifEnd + 2;
                        continue;
                    }
                }

                // Check for {% for item in list %}
                if (tag.substr(0, 4) == "for ") {
                    std::string forExpr = tag.substr(4);
                    size_t inPos = forExpr.find(" in ");
                    if (inPos != std::string::npos) {
                        std::string itemVar = xmlTrim(forExpr.substr(0, inPos));
                        std::string listVar = xmlTrim(forExpr.substr(inPos + 4));

                        // Find matching endfor
                        size_t forDepth = 1;
                        size_t searchPos = endPos + 2;
                        size_t endforPos = std::string::npos;

                        while (searchPos < tmpl.length() && forDepth > 0) {
                            size_t nextTag = tmpl.find("{%", searchPos);
                            if (nextTag == std::string::npos) break;

                            size_t tagEnd = tmpl.find("%}", nextTag + 2);
                            if (tagEnd == std::string::npos) break;

                            std::string innerTag = xmlTrim(tmpl.substr(nextTag + 2, tagEnd - nextTag - 2));

                            if (innerTag.substr(0, 4) == "for ") {
                                forDepth++;
                            } else if (innerTag == "endfor") {
                                forDepth--;
                                if (forDepth == 0) {
                                    endforPos = nextTag;
                                }
                            }

                            searchPos = tagEnd + 2;
                        }

                        if (endforPos != std::string::npos) {
                            std::string loopBody = tmpl.substr(endPos + 2, endforPos - endPos - 2);

                            auto arrIt = ctx.arrays.find(listVar);
                            if (arrIt != ctx.arrays.end()) {
                                for (size_t i = 0; i < arrIt->second.size(); i++) {
                                    TemplateContext loopCtx = ctx;
                                    loopCtx.strings[itemVar] = arrIt->second[i];
                                    loopCtx.integers["loop.index"] = static_cast<int>(i);
                                    loopCtx.integers["loop.index1"] = static_cast<int>(i + 1);
                                    loopCtx.booleans["loop.first"] = (i == 0);
                                    loopCtx.booleans["loop.last"] = (i == arrIt->second.size() - 1);
                                    result += templateRender(loopBody, loopCtx);
                                }
                            }

                            size_t endforEnd = tmpl.find("%}", endforPos + 2);
                            pos = endforEnd + 2;
                            continue;
                        }
                    }
                }

                pos = endPos + 2;
                continue;
            }
        }

        result += tmpl[pos++];
    }

    return result;
}

void NativeRegistry::initTemplate() {
    // __template_create_context(): int
    registerFunction("template", "__template_create_context", [](const std::vector<std::any>& args) -> std::any {
        std::lock_guard<std::mutex> lock(g_template_mutex);
        int ctxId = ++g_template_context_counter;
        g_template_contexts[ctxId] = TemplateContext();
        return std::any(ctxId);
    }, FunctionSignature{{}, "int"});

    // __template_set_string(ctxId: int, key: string, value: string): void
    registerFunction("template", "__template_set_string", [](const std::vector<std::any>& args) -> std::any {
        auto ctxId = std::any_cast<int>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto value = std::any_cast<std::string>(args[2]);

        std::lock_guard<std::mutex> lock(g_template_mutex);
        auto it = g_template_contexts.find(ctxId);
        if (it == g_template_contexts.end()) {
            throw std::runtime_error("Invalid template context");
        }

        it->second.strings[key] = value;
        return std::any();
    }, FunctionSignature{{"int", "string", "string"}, "void"});

    // __template_set_int(ctxId: int, key: string, value: int): void
    registerFunction("template", "__template_set_int", [](const std::vector<std::any>& args) -> std::any {
        auto ctxId = std::any_cast<int>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto value = std::any_cast<int>(args[2]);

        std::lock_guard<std::mutex> lock(g_template_mutex);
        auto it = g_template_contexts.find(ctxId);
        if (it == g_template_contexts.end()) {
            throw std::runtime_error("Invalid template context");
        }

        it->second.integers[key] = value;
        return std::any();
    }, FunctionSignature{{"int", "string", "int"}, "void"});

    // __template_set_double(ctxId: int, key: string, value: double): void
    registerFunction("template", "__template_set_double", [](const std::vector<std::any>& args) -> std::any {
        auto ctxId = std::any_cast<int>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto value = std::any_cast<double>(args[2]);

        std::lock_guard<std::mutex> lock(g_template_mutex);
        auto it = g_template_contexts.find(ctxId);
        if (it == g_template_contexts.end()) {
            throw std::runtime_error("Invalid template context");
        }

        it->second.doubles[key] = value;
        return std::any();
    }, FunctionSignature{{"int", "string", "double"}, "void"});

    // __template_set_bool(ctxId: int, key: string, value: bool): void
    registerFunction("template", "__template_set_bool", [](const std::vector<std::any>& args) -> std::any {
        auto ctxId = std::any_cast<int>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto value = std::any_cast<bool>(args[2]);

        std::lock_guard<std::mutex> lock(g_template_mutex);
        auto it = g_template_contexts.find(ctxId);
        if (it == g_template_contexts.end()) {
            throw std::runtime_error("Invalid template context");
        }

        it->second.booleans[key] = value;
        return std::any();
    }, FunctionSignature{{"int", "string", "bool"}, "void"});

    // __template_set_array(ctxId: int, key: string, values: array<string>): void
    registerFunction("template", "__template_set_array", [](const std::vector<std::any>& args) -> std::any {
        auto ctxId = std::any_cast<int>(args[0]);
        auto key = std::any_cast<std::string>(args[1]);
        auto values = std::any_cast<std::vector<std::string>>(args[2]);

        std::lock_guard<std::mutex> lock(g_template_mutex);
        auto it = g_template_contexts.find(ctxId);
        if (it == g_template_contexts.end()) {
            throw std::runtime_error("Invalid template context");
        }

        it->second.arrays[key] = values;
        return std::any();
    }, FunctionSignature{{"int", "string", "array<string>"}, "void"});

    // __template_render(templateString: string, ctxId: int): string
    registerFunction("template", "__template_render", [](const std::vector<std::any>& args) -> std::any {
        auto tmpl = std::any_cast<std::string>(args[0]);
        auto ctxId = std::any_cast<int>(args[1]);

        std::lock_guard<std::mutex> lock(g_template_mutex);
        auto it = g_template_contexts.find(ctxId);
        if (it == g_template_contexts.end()) {
            throw std::runtime_error("Invalid template context");
        }

        return std::any(templateRender(tmpl, it->second));
    }, FunctionSignature{{"string", "int"}, "string"});

    // __template_render_file(filePath: string, ctxId: int): string
    registerFunction("template", "__template_render_file", [](const std::vector<std::any>& args) -> std::any {
        auto filePath = std::any_cast<std::string>(args[0]);
        auto ctxId = std::any_cast<int>(args[1]);

        // Read file
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open template file: " + filePath);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string tmpl = buffer.str();

        std::lock_guard<std::mutex> lock(g_template_mutex);
        auto it = g_template_contexts.find(ctxId);
        if (it == g_template_contexts.end()) {
            throw std::runtime_error("Invalid template context");
        }

        return std::any(templateRender(tmpl, it->second));
    }, FunctionSignature{{"string", "int"}, "string"});

    // __template_free_context(ctxId: int): void
    registerFunction("template", "__template_free_context", [](const std::vector<std::any>& args) -> std::any {
        auto ctxId = std::any_cast<int>(args[0]);

        std::lock_guard<std::mutex> lock(g_template_mutex);
        g_template_contexts.erase(ctxId);

        return std::any();
    }, FunctionSignature{{"int"}, "void"});

    // __template_escape_html(str: string): string
    registerFunction("template", "__template_escape_html", [](const std::vector<std::any>& args) -> std::any {
        auto str = std::any_cast<std::string>(args[0]);
        return std::any(templateEscapeHtml(str));
    }, FunctionSignature{{"string"}, "string"});
}

}  // namespace stratos
