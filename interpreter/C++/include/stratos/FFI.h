#ifndef STRATOS_FFI_H
#define STRATOS_FFI_H

#include <string>
#include <vector>
#include <unordered_map>
#include <any>
#include <memory>
#include <dlfcn.h>

namespace stratos {

// FFI Library Handle
struct FFILibrary {
    void* handle;
    std::string path;

    FFILibrary(void* h, const std::string& p) : handle(h), path(p) {}

    ~FFILibrary() {
        if (handle) {
            dlclose(handle);
        }
    }
};

// FFI Type mapping
enum class FFIType {
    VOID,
    INT,
    DOUBLE,
    STRING,
    POINTER
};

// FFI Function Signature
struct FFISignature {
    std::vector<FFIType> paramTypes;
    FFIType returnType;
};

// FFI Manager - handles loading and calling foreign functions
class FFIManager {
public:
    static FFIManager& instance();

    // Load a shared library (.so, .dylib, .dll)
    int loadLibrary(const std::string& path);

    // Unload a library
    void unloadLibrary(int libraryId);

    // Get function pointer from loaded library
    void* getFunction(int libraryId, const std::string& functionName);

    // Call a foreign function with proper type conversion
    std::any callFunction(int libraryId,
                         const std::string& functionName,
                         const FFISignature& signature,
                         const std::vector<std::any>& args);

    // Helper to parse FFI type from string
    static FFIType parseType(const std::string& typeStr);

    // Helper to convert type to string
    static std::string typeToString(FFIType type);

private:
    FFIManager() = default;

    std::unordered_map<int, std::shared_ptr<FFILibrary>> libraries;
    int nextLibraryId = 1;

    // Type conversion helpers
    template<typename T>
    T extractArg(const std::any& arg);

    // Function calling helpers for different return types
    int callIntFunction(void* func, const FFISignature& sig, const std::vector<std::any>& args);
    double callDoubleFunction(void* func, const FFISignature& sig, const std::vector<std::any>& args);
    std::string callStringFunction(void* func, const FFISignature& sig, const std::vector<std::any>& args);
    void* callPointerFunction(void* func, const FFISignature& sig, const std::vector<std::any>& args);
    void callVoidFunction(void* func, const FFISignature& sig, const std::vector<std::any>& args);
};

} // namespace stratos

#endif // STRATOS_FFI_H
