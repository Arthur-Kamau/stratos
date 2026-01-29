#include "stratos/FFI.h"
#include "stratos/Logger.h"
#include <stdexcept>
#include <sstream>
#include <cstring>

namespace stratos {

FFIManager& FFIManager::instance() {
    static FFIManager instance;
    return instance;
}

int FFIManager::loadLibrary(const std::string& path) {
    // Load the shared library
#ifdef _WIN32
    void* handle = (void*)LoadLibraryA(path.c_str());

    if (!handle) {
        std::string error = "Failed to load library: ";
        error += path;
        error += " - Error code: ";
        error += std::to_string(GetLastError());
        throw std::runtime_error(error);
    }
#else
    void* handle = dlopen(path.c_str(), RTLD_LAZY);

    if (!handle) {
        std::string error = "Failed to load library: ";
        error += path;
        error += " - ";
        error += dlerror();
        throw std::runtime_error(error);
    }
#endif

    // Create library object
    auto library = std::make_shared<FFILibrary>(handle, path);
    int libraryId = nextLibraryId++;
    libraries[libraryId] = library;

    STRATOS_LOG_INFO("Loaded FFI library: " + path + " (ID: " + std::to_string(libraryId) + ")");

    return libraryId;
}

void FFIManager::unloadLibrary(int libraryId) {
    auto it = libraries.find(libraryId);
    if (it != libraries.end()) {
        STRATOS_LOG_INFO("Unloading FFI library ID: " + std::to_string(libraryId));
        libraries.erase(it);
    }
}

void* FFIManager::getFunction(int libraryId, const std::string& functionName) {
    auto it = libraries.find(libraryId);
    if (it == libraries.end()) {
        throw std::runtime_error("Invalid library ID: " + std::to_string(libraryId));
    }

#ifdef _WIN32
    void* func = (void*)GetProcAddress((HMODULE)it->second->handle, functionName.c_str());

    if (!func) {
        std::string msg = "Failed to find function '";
        msg += functionName;
        msg += "' in library - Error code: ";
        msg += std::to_string(GetLastError());
        throw std::runtime_error(msg);
    }
#else
    // Clear any existing error
    dlerror();

    // Look up the symbol
    void* func = dlsym(it->second->handle, functionName.c_str());

    // Check for errors
    char* error = dlerror();
    if (error != nullptr) {
        std::string msg = "Failed to find function '";
        msg += functionName;
        msg += "' in library: ";
        msg += error;
        throw std::runtime_error(msg);
    }

    if (!func) {
        throw std::runtime_error("Function '" + functionName + "' not found in library");
    }
#endif

    return func;
}

std::any FFIManager::callFunction(int libraryId,
                                  const std::string& functionName,
                                  const FFISignature& signature,
                                  const std::vector<std::any>& args) {
    // Get the function pointer
    void* func = getFunction(libraryId, functionName);

    // Validate argument count
    if (args.size() != signature.paramTypes.size()) {
        throw std::runtime_error("Argument count mismatch: expected " +
                               std::to_string(signature.paramTypes.size()) +
                               ", got " + std::to_string(args.size()));
    }

    // Call based on return type
    switch (signature.returnType) {
        case FFIType::VOID:
            callVoidFunction(func, signature, args);
            return std::any();

        case FFIType::INT:
            return std::any(callIntFunction(func, signature, args));

        case FFIType::DOUBLE:
            return std::any(callDoubleFunction(func, signature, args));

        case FFIType::STRING:
            return std::any(callStringFunction(func, signature, args));

        case FFIType::POINTER:
            return std::any(callPointerFunction(func, signature, args));

        default:
            throw std::runtime_error("Unsupported return type");
    }
}

FFIType FFIManager::parseType(const std::string& typeStr) {
    if (typeStr == "void") return FFIType::VOID;
    if (typeStr == "int") return FFIType::INT;
    if (typeStr == "double") return FFIType::DOUBLE;
    if (typeStr == "string") return FFIType::STRING;
    if (typeStr == "pointer" || typeStr == "ptr") return FFIType::POINTER;

    throw std::runtime_error("Unknown FFI type: " + typeStr);
}

std::string FFIManager::typeToString(FFIType type) {
    switch (type) {
        case FFIType::VOID: return "void";
        case FFIType::INT: return "int";
        case FFIType::DOUBLE: return "double";
        case FFIType::STRING: return "string";
        case FFIType::POINTER: return "pointer";
        default: return "unknown";
    }
}

// Helper to extract typed arguments
template<typename T>
T FFIManager::extractArg(const std::any& arg) {
    try {
        return std::any_cast<T>(arg);
    } catch (const std::bad_any_cast&) {
        throw std::runtime_error("FFI argument type mismatch");
    }
}

// Call functions with different return types
// Note: We support up to 6 parameters for now
int FFIManager::callIntFunction(void* func, const FFISignature& sig, const std::vector<std::any>& args) {
    if (sig.paramTypes.size() > 6) {
        throw std::runtime_error("FFI: Maximum 6 parameters supported");
    }

    // Extract arguments based on signature
    if (sig.paramTypes.empty()) {
        return ((int(*)())func)();
    } else if (sig.paramTypes.size() == 1) {
        if (sig.paramTypes[0] == FFIType::INT) {
            return ((int(*)(int))func)(extractArg<int>(args[0]));
        } else if (sig.paramTypes[0] == FFIType::DOUBLE) {
            return ((int(*)(double))func)(extractArg<double>(args[0]));
        }
    } else if (sig.paramTypes.size() == 2) {
        if (sig.paramTypes[0] == FFIType::INT && sig.paramTypes[1] == FFIType::INT) {
            return ((int(*)(int, int))func)(extractArg<int>(args[0]), extractArg<int>(args[1]));
        } else if (sig.paramTypes[0] == FFIType::DOUBLE && sig.paramTypes[1] == FFIType::DOUBLE) {
            return ((int(*)(double, double))func)(extractArg<double>(args[0]), extractArg<double>(args[1]));
        } else if (sig.paramTypes[0] == FFIType::INT && sig.paramTypes[1] == FFIType::DOUBLE) {
            return ((int(*)(int, double))func)(extractArg<int>(args[0]), extractArg<double>(args[1]));
        }
    }

    throw std::runtime_error("FFI: Unsupported parameter combination for int return type");
}

double FFIManager::callDoubleFunction(void* func, const FFISignature& sig, const std::vector<std::any>& args) {
    if (sig.paramTypes.size() > 6) {
        throw std::runtime_error("FFI: Maximum 6 parameters supported");
    }

    if (sig.paramTypes.empty()) {
        return ((double(*)())func)();
    } else if (sig.paramTypes.size() == 1) {
        if (sig.paramTypes[0] == FFIType::INT) {
            return ((double(*)(int))func)(extractArg<int>(args[0]));
        } else if (sig.paramTypes[0] == FFIType::DOUBLE) {
            return ((double(*)(double))func)(extractArg<double>(args[0]));
        }
    } else if (sig.paramTypes.size() == 2) {
        if (sig.paramTypes[0] == FFIType::DOUBLE && sig.paramTypes[1] == FFIType::DOUBLE) {
            return ((double(*)(double, double))func)(extractArg<double>(args[0]), extractArg<double>(args[1]));
        } else if (sig.paramTypes[0] == FFIType::INT && sig.paramTypes[1] == FFIType::INT) {
            return ((double(*)(int, int))func)(extractArg<int>(args[0]), extractArg<int>(args[1]));
        }
    }

    throw std::runtime_error("FFI: Unsupported parameter combination for double return type");
}

std::string FFIManager::callStringFunction(void* func, const FFISignature& sig, const std::vector<std::any>& args) {
    if (sig.paramTypes.size() > 6) {
        throw std::runtime_error("FFI: Maximum 6 parameters supported");
    }

    const char* result = nullptr;

    if (sig.paramTypes.empty()) {
        result = ((const char*(*)())func)();
    } else if (sig.paramTypes.size() == 1) {
        if (sig.paramTypes[0] == FFIType::STRING) {
            const char* arg0 = extractArg<std::string>(args[0]).c_str();
            result = ((const char*(*)(const char*))func)(arg0);
        }
    }

    if (result == nullptr) {
        throw std::runtime_error("FFI: Unsupported parameter combination for string return type");
    }

    return std::string(result);
}

void* FFIManager::callPointerFunction(void* func, const FFISignature& sig, const std::vector<std::any>& args) {
    if (sig.paramTypes.empty()) {
        return ((void*(*)())func)();
    }

    throw std::runtime_error("FFI: Pointer return type not fully implemented");
}

void FFIManager::callVoidFunction(void* func, const FFISignature& sig, const std::vector<std::any>& args) {
    if (sig.paramTypes.size() > 6) {
        throw std::runtime_error("FFI: Maximum 6 parameters supported");
    }

    if (sig.paramTypes.empty()) {
        ((void(*)())func)();
    } else if (sig.paramTypes.size() == 1) {
        if (sig.paramTypes[0] == FFIType::INT) {
            ((void(*)(int))func)(extractArg<int>(args[0]));
        } else if (sig.paramTypes[0] == FFIType::DOUBLE) {
            ((void(*)(double))func)(extractArg<double>(args[0]));
        } else if (sig.paramTypes[0] == FFIType::STRING) {
            const char* arg0 = extractArg<std::string>(args[0]).c_str();
            ((void(*)(const char*))func)(arg0);
        }
    } else if (sig.paramTypes.size() == 2) {
        if (sig.paramTypes[0] == FFIType::INT && sig.paramTypes[1] == FFIType::INT) {
            ((void(*)(int, int))func)(extractArg<int>(args[0]), extractArg<int>(args[1]));
        } else if (sig.paramTypes[0] == FFIType::STRING && sig.paramTypes[1] == FFIType::INT) {
            const char* arg0 = extractArg<std::string>(args[0]).c_str();
            ((void(*)(const char*, int))func)(arg0, extractArg<int>(args[1]));
        }
    } else {
        throw std::runtime_error("FFI: Unsupported parameter combination for void return type");
    }
}

} // namespace stratos
