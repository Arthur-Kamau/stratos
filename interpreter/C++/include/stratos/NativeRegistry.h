#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <any>
#include <memory>

namespace stratos {

// Forward declarations
class Value;

// Native function signature - takes vector of values, returns a value
using NativeFunction = std::function<std::any(const std::vector<std::any>&)>;

/**
 * Type signature for a native function
 */
struct FunctionSignature {
    std::vector<std::string> paramTypes;  // Parameter types (e.g., "int", "double", "string")
    std::string returnType;               // Return type
};

/**
 * Registry for native (C++) functions that can be called from Stratos code
 */
class NativeRegistry {
public:
    static NativeRegistry& getInstance();

    // Register a native function with its implementation
    void registerFunction(const std::string& moduleName, const std::string& functionName, NativeFunction func);

    // Register a native function with type signature
    void registerFunction(const std::string& moduleName, const std::string& functionName,
                         NativeFunction func, const FunctionSignature& signature);

    // Check if a function is native
    bool isNative(const std::string& moduleName, const std::string& functionName) const;

    // Get a native function
    NativeFunction getFunction(const std::string& moduleName, const std::string& functionName) const;

    // Get function type signature
    FunctionSignature getSignature(const std::string& moduleName, const std::string& functionName) const;

    // Check if signature exists
    bool hasSignature(const std::string& moduleName, const std::string& functionName) const;

    // Get fully qualified name
    std::string getQualifiedName(const std::string& moduleName, const std::string& functionName) const;

    // Initialize all standard library native functions
    void initializeStdlib();

private:
    NativeRegistry() = default;
    std::unordered_map<std::string, NativeFunction> functions_;
    std::unordered_map<std::string, FunctionSignature> signatures_;

    // Module initialization functions
    void initMath();
    void initStrings();
    void initIO();
    void initLog();
    void initTime();
    void initCollections();
    void initJSON();
    void initBase64();
    void initCSV();
    void initConcurrent();
    void initAsync();
    void initNet();
    void initHTTP();
    void initOS();
    void initCrypto();
    void initZip();
    void initTesting();
};

// Helper macros for registering native functions
#define REGISTER_NATIVE(module, name, func) \
    registerFunction(#module, #name, func)

#define NATIVE_FUNCTION(name) \
    std::any name(const std::vector<std::any>& args)

} // namespace stratos
