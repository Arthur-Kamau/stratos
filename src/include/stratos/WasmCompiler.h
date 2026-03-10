#ifndef STRATOS_WASM_COMPILER_H
#define STRATOS_WASM_COMPILER_H

#include "IRGenerator.h"
#include <string>
#include <vector>
#include <filesystem>

namespace stratos {

struct WasmCompileResult {
    bool success;
    std::string errorMessage;
    std::string wasmPath;    // Path to generated .wasm
    std::string jsPath;      // Path to generated runtime.js
    std::string htmlPath;    // Path to generated index.html
    double compilationTime;  // in ms
};

class WasmCompiler {
public:
    WasmCompiler();

    // Compile LLVM IR (.ll) to WASM
    // outputDir: directory for output files (wasm, js, html)
    // title: HTML page title
    // useEmscripten: use emscripten pipeline instead of clang/wasm-ld
    WasmCompileResult compile(const std::string& irPath,
                               const std::string& outputDir,
                               const std::string& title = "Stratos App",
                               bool useEmscripten = false);

    // Check if required tools are available
    static bool hasClangWasm();
    static bool hasEmscripten();
    static bool hasWasmLd();
    static std::string findTool(const std::string& name);

private:
    // Pipeline stages
    bool compileIRToObject(const std::string& irPath, const std::string& objPath);
    bool linkWasm(const std::string& objPath, const std::string& wasmPath);
    bool compileWithEmscripten(const std::string& irPath, const std::string& outputDir);

    // Generate support files
    void copyRuntimeJS(const std::string& outputDir);
    void generateHTMLShell(const std::string& outputDir, const std::string& wasmFile,
                           const std::string& title);

    // Helper
    int runCommand(const std::string& cmd);
    std::string getStratosRoot();
};

} // namespace stratos

#endif // STRATOS_WASM_COMPILER_H
