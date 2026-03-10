#include "stratos/WasmCompiler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <array>

namespace stratos {
namespace fs = std::filesystem;

WasmCompiler::WasmCompiler() {}

// ============================================================================
// Tool Detection
// ============================================================================

std::string WasmCompiler::findTool(const std::string& name) {
    // Try versioned names first (e.g., clang-18, clang-17)
    std::vector<std::string> candidates;
    if (name == "clang") {
        for (int v = 20; v >= 14; v--) {
            candidates.push_back("clang-" + std::to_string(v));
        }
    } else if (name == "wasm-ld") {
        for (int v = 20; v >= 14; v--) {
            candidates.push_back("wasm-ld-" + std::to_string(v));
        }
    } else if (name == "llc") {
        for (int v = 20; v >= 14; v--) {
            candidates.push_back("llc-" + std::to_string(v));
        }
    }
    candidates.push_back(name);

    for (const auto& candidate : candidates) {
        std::string cmd = "which " + candidate + " 2>/dev/null";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            std::array<char, 256> buffer;
            std::string result;
            while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
                result += buffer.data();
            }
            int status = pclose(pipe);
            if (status == 0 && !result.empty()) {
                // Remove trailing newline
                while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
                    result.pop_back();
                }
                return result;
            }
        }
    }
    return "";
}

bool WasmCompiler::hasClangWasm() {
    return !findTool("clang").empty();
}

bool WasmCompiler::hasEmscripten() {
    return !findTool("emcc").empty();
}

bool WasmCompiler::hasWasmLd() {
    return !findTool("wasm-ld").empty();
}

// ============================================================================
// Main Compile Entry Point
// ============================================================================

WasmCompileResult WasmCompiler::compile(const std::string& irPath,
                                         const std::string& outputDir,
                                         const std::string& title,
                                         bool useEmscripten) {
    WasmCompileResult result;
    auto start = std::chrono::high_resolution_clock::now();

    // Create output directory
    fs::create_directories(outputDir);

    if (useEmscripten) {
        if (!hasEmscripten()) {
            result.success = false;
            result.errorMessage = "Emscripten (emcc) not found. Install with: git clone https://github.com/emscripten-core/emsdk.git && cd emsdk && ./emsdk install latest && ./emsdk activate latest";
            return result;
        }

        if (!compileWithEmscripten(irPath, outputDir)) {
            result.success = false;
            result.errorMessage = "Emscripten compilation failed";
            return result;
        }

        result.wasmPath = outputDir + "/app.wasm";
        result.jsPath = outputDir + "/app.js";
        result.htmlPath = outputDir + "/index.html";
    } else {
        // Use clang + wasm-ld pipeline
        std::string clang = findTool("clang");
        std::string wasmLd = findTool("wasm-ld");

        if (clang.empty()) {
            result.success = false;
            result.errorMessage = "clang not found. Install with: sudo apt install clang lld";
            return result;
        }

        // Stage 1: IR → object (.o)
        std::string objPath = outputDir + "/app.o";
        std::cout << "  [WASM] Compiling IR to object..." << std::endl;

        std::string compileCmd = clang + " -target wasm32-unknown-wasi"
            " -nostdlib -O2 -c " + irPath + " -o " + objPath;
        if (runCommand(compileCmd) != 0) {
            result.success = false;
            result.errorMessage = "Failed to compile IR to WASM object. Run with -v for details.";
            return result;
        }

        // Stage 2: link → .wasm
        std::string wasmPath = outputDir + "/app.wasm";
        std::cout << "  [WASM] Linking to WASM..." << std::endl;

        if (!wasmLd.empty()) {
            std::string linkCmd = wasmLd + " --no-entry --export-all --allow-undefined"
                " -o " + wasmPath + " " + objPath;
            if (runCommand(linkCmd) != 0) {
                result.success = false;
                result.errorMessage = "WASM linking failed";
                return result;
            }
        } else {
            // Try using clang as linker
            std::string linkCmd = clang + " -target wasm32-unknown-wasi"
                " -nostdlib -Wl,--no-entry -Wl,--export-all -Wl,--allow-undefined"
                " -o " + wasmPath + " " + objPath;
            if (runCommand(linkCmd) != 0) {
                result.success = false;
                result.errorMessage = "WASM linking failed. Install wasm-ld: sudo apt install lld";
                return result;
            }
        }

        result.wasmPath = wasmPath;

        // Stage 3: Copy runtime.js
        std::cout << "  [WASM] Generating runtime..." << std::endl;
        copyRuntimeJS(outputDir);
        result.jsPath = outputDir + "/runtime.js";

        // Stage 4: Generate HTML shell
        generateHTMLShell(outputDir, "app.wasm", title);
        result.htmlPath = outputDir + "/index.html";
    }

    result.success = true;

    auto end = std::chrono::high_resolution_clock::now();
    result.compilationTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return result;
}

// ============================================================================
// Pipeline Stages
// ============================================================================

bool WasmCompiler::compileIRToObject(const std::string& irPath, const std::string& objPath) {
    std::string llc = findTool("llc");
    if (llc.empty()) return false;

    std::string cmd = llc + " -march=wasm32 -filetype=obj -o " + objPath + " " + irPath;
    return runCommand(cmd) == 0;
}

bool WasmCompiler::linkWasm(const std::string& objPath, const std::string& wasmPath) {
    std::string wasmLd = findTool("wasm-ld");
    if (wasmLd.empty()) return false;

    std::string cmd = wasmLd + " --no-entry --export-all --allow-undefined"
        " -o " + wasmPath + " " + objPath;
    return runCommand(cmd) == 0;
}

bool WasmCompiler::compileWithEmscripten(const std::string& irPath, const std::string& outputDir) {
    std::string emcc = findTool("emcc");
    if (emcc.empty()) return false;

    std::string htmlPath = outputDir + "/index.html";
    std::string cmd = emcc + " " + irPath
        + " -o " + htmlPath
        + " -s WASM=1"
        + " -s EXPORTED_FUNCTIONS=\"['_main']\""
        + " -s EXPORTED_RUNTIME_METHODS=\"['ccall','cwrap']\""
        + " -O2";
    return runCommand(cmd) == 0;
}

// ============================================================================
// Support File Generation
// ============================================================================

void WasmCompiler::copyRuntimeJS(const std::string& outputDir) {
    // Find runtime.js relative to the executable or in known locations
    std::vector<std::string> searchPaths = {
        getStratosRoot() + "/src/wasm/runtime.js",
        getStratosRoot() + "/wasm/runtime.js",
        "./src/wasm/runtime.js",
        "./wasm/runtime.js",
    };

    for (const auto& path : searchPaths) {
        if (fs::exists(path)) {
            fs::copy_file(path, outputDir + "/runtime.js",
                         fs::copy_options::overwrite_existing);
            return;
        }
    }

    // Fallback: write a minimal inline runtime
    std::ofstream out(outputDir + "/runtime.js");
    out << R"(// Stratos WASM Runtime (inline fallback)
class StratosRuntime {
    constructor() { this.memory = null; this.instance = null; this.decoder = new TextDecoder(); this.outputBuffer = ''; }
    readString(ptr) { const m = new Uint8Array(this.memory.buffer); let e = ptr; while(m[e]) e++; return this.decoder.decode(m.slice(ptr, e)); }
    readStringN(ptr, len) { return this.decoder.decode(new Uint8Array(this.memory.buffer, ptr, len)); }
    print(s) { this.outputBuffer += s; if (s.includes('\n')) { const l = this.outputBuffer.split('\n'); for (let i = 0; i < l.length-1; i++) { console.log(l[i]); if (this.onOutput) this.onOutput(l[i]); } this.outputBuffer = l[l.length-1]; } }
    flush() { if (this.outputBuffer) { console.log(this.outputBuffer); if (this.onOutput) this.onOutput(this.outputBuffer); this.outputBuffer = ''; } }
    getImports() { const rt = this; return {
        env: { memory: new WebAssembly.Memory({initial:256}),
            __stratos_print_str(p,l) { rt.print(rt.readStringN(p,l)); },
            __stratos_print_int(v) { rt.print(String(v)); },
            __stratos_print_float(v) { rt.print(String(v)); },
            printf(f,...a) { rt.print(rt.readString(f)); return 0; },
            malloc(s) { if(!rt._hp) rt._hp=65536; const p=rt._hp; rt._hp=(rt._hp+s+7)&~7; return p; },
        },
        wasi_snapshot_preview1: { fd_write(fd,iovs,iovsLen,nw) { const m=new DataView(rt.memory.buffer); let tw=0; for(let i=0;i<iovsLen;i++){const p=m.getUint32(iovs+i*8,true),l=m.getUint32(iovs+i*8+4,true);rt.print(rt.readStringN(p,l));tw+=l;} m.setUint32(nw,tw,true);return 0; },
            fd_close(){return 0},fd_seek(){return 0},fd_read(){return 0},environ_sizes_get(c,s){const m=new DataView(rt.memory.buffer);m.setUint32(c,0,true);m.setUint32(s,0,true);return 0},environ_get(){return 0},proc_exit(){},args_sizes_get(c,s){const m=new DataView(rt.memory.buffer);m.setUint32(c,0,true);m.setUint32(s,0,true);return 0},args_get(){return 0},clock_time_get(i,p,t){new DataView(rt.memory.buffer).setBigUint64(t,BigInt(Date.now())*1000000n,true);return 0}
        }
    }; }
    async run(wasmPath) { const imports = this.getImports(); let m; if(typeof fetch!=='undefined'){m=await WebAssembly.instantiate(await(await fetch(wasmPath)).arrayBuffer(),imports);}else{const fs=await import('fs');m=await WebAssembly.instantiate(fs.readFileSync(wasmPath),imports);} this.instance=m.instance; this.memory=this.instance.exports.memory||imports.env.memory; if(this.instance.exports._start)this.instance.exports._start();else if(this.instance.exports.main)this.instance.exports.main(); this.flush(); return this; }
}
if(typeof module!=='undefined'&&module.exports)module.exports={StratosRuntime};
if(typeof window!=='undefined')window.StratosRuntime=StratosRuntime;
)";
    out.close();
}

void WasmCompiler::generateHTMLShell(const std::string& outputDir,
                                      const std::string& wasmFile,
                                      const std::string& title) {
    // Find shell template
    std::string shellContent;
    std::vector<std::string> searchPaths = {
        getStratosRoot() + "/src/wasm/shell.html",
        getStratosRoot() + "/wasm/shell.html",
        "./src/wasm/shell.html",
        "./wasm/shell.html",
    };

    for (const auto& path : searchPaths) {
        if (fs::exists(path)) {
            std::ifstream in(path);
            std::stringstream buf;
            buf << in.rdbuf();
            shellContent = buf.str();
            break;
        }
    }

    if (shellContent.empty()) {
        // Minimal fallback HTML
        shellContent = R"(<!DOCTYPE html>
<html><head><meta charset="UTF-8"><title>{{TITLE}}</title></head>
<body style="background:#1a1a2e;color:#e0e0e0;font-family:monospace;padding:20px">
<h2>{{TITLE}}</h2><pre id="output">Loading...</pre>
<script src="runtime.js"></script>
<script>
(async()=>{const o=document.getElementById('output');o.textContent='';
const rt=new StratosRuntime();rt.onOutput=(l)=>{o.textContent+=l+'\n';};
try{await rt.run('{{WASM_FILE}}');}catch(e){o.textContent+='Error: '+e.message;}})();
</script></body></html>)";
    }

    // Replace placeholders
    auto replaceAll = [](std::string& str, const std::string& from, const std::string& to) {
        size_t pos = 0;
        while ((pos = str.find(from, pos)) != std::string::npos) {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
    };

    replaceAll(shellContent, "{{TITLE}}", title);
    replaceAll(shellContent, "{{WASM_FILE}}", wasmFile);

    std::ofstream out(outputDir + "/index.html");
    out << shellContent;
    out.close();
}

// ============================================================================
// Helpers
// ============================================================================

int WasmCompiler::runCommand(const std::string& cmd) {
    return std::system(cmd.c_str());
}

std::string WasmCompiler::getStratosRoot() {
    // Try to find the project root by looking for stratos.conf or src/wasm/
    fs::path current = fs::current_path();
    for (int i = 0; i < 5; i++) {
        if (fs::exists(current / "src" / "wasm" / "runtime.js")) {
            return current.string();
        }
        if (fs::exists(current / "wasm" / "runtime.js")) {
            return current.string();
        }
        current = current.parent_path();
    }
    return fs::current_path().string();
}

} // namespace stratos
