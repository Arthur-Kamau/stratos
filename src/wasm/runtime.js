/**
 * Stratos WASM Runtime
 *
 * Provides the JS glue code for running Stratos-compiled WASM modules.
 * Handles memory management, string passing, and bridging to browser/Node APIs.
 */

class StratosRuntime {
    constructor() {
        this.memory = null;
        this.instance = null;
        this.decoder = new TextDecoder('utf-8');
        this.encoder = new TextEncoder();
        this.outputBuffer = '';
    }

    /**
     * Read a null-terminated string from WASM memory
     */
    readString(ptr) {
        const mem = new Uint8Array(this.memory.buffer);
        let end = ptr;
        while (mem[end] !== 0 && end < mem.length) end++;
        return this.decoder.decode(mem.slice(ptr, end));
    }

    /**
     * Read a string with known length from WASM memory
     */
    readStringN(ptr, len) {
        const mem = new Uint8Array(this.memory.buffer);
        return this.decoder.decode(mem.slice(ptr, ptr + len));
    }

    /**
     * Write a string to WASM memory, returns pointer
     */
    writeString(str) {
        const encoded = this.encoder.encode(str + '\0');
        const ptr = this.instance.exports.malloc(encoded.length);
        const mem = new Uint8Array(this.memory.buffer);
        mem.set(encoded, ptr);
        return ptr;
    }

    /**
     * Get the import object for WebAssembly.instantiate
     */
    getImports() {
        const runtime = this;
        return {
            env: {
                // Memory (will be overridden if module exports its own)
                memory: new WebAssembly.Memory({ initial: 256, maximum: 65536 }),

                // Print functions
                __stratos_print_str(ptr, len) {
                    const str = runtime.readStringN(ptr, len);
                    runtime.print(str);
                },
                __stratos_print_int(value) {
                    runtime.print(String(value));
                },
                __stratos_print_float(value) {
                    runtime.print(String(value));
                },

                // printf shim — simplified, handles %d, %f, %s with \n
                printf(fmtPtr, ...args) {
                    const fmt = runtime.readString(fmtPtr);
                    let result = fmt;
                    let argIdx = 0;
                    result = result.replace(/%[dif]/g, (match) => {
                        if (argIdx < args.length) {
                            return String(args[argIdx++]);
                        }
                        return match;
                    });
                    result = result.replace(/%s/g, () => {
                        if (argIdx < args.length) {
                            return runtime.readString(args[argIdx++]);
                        }
                        return '%s';
                    });
                    // Handle \n (0x0A in C)
                    result = result.replace(/\n/g, '\n');
                    runtime.print(result);
                    return result.length;
                },

                // Memory allocation
                malloc(size) {
                    // Simple bump allocator for basic usage
                    // Production would use a proper allocator
                    if (!runtime._heapPtr) {
                        runtime._heapPtr = 65536; // Start heap after stack
                    }
                    const ptr = runtime._heapPtr;
                    runtime._heapPtr += size;
                    // Align to 8 bytes
                    runtime._heapPtr = (runtime._heapPtr + 7) & ~7;
                    return ptr;
                },

                // Math imports
                sin: Math.sin,
                cos: Math.cos,
                tan: Math.tan,
                sqrt: Math.sqrt,
                pow: Math.pow,
                log: Math.log,
                floor: Math.floor,
                ceil: Math.ceil,
                abs: Math.abs,
                fmod(a, b) { return a % b; },
            },

            wasi_snapshot_preview1: {
                // WASI fd_write — write to file descriptor
                fd_write(fd, iovs, iovsLen, nwritten) {
                    const mem = new DataView(runtime.memory.buffer);
                    const mem8 = new Uint8Array(runtime.memory.buffer);
                    let totalWritten = 0;
                    for (let i = 0; i < iovsLen; i++) {
                        const ptr = mem.getUint32(iovs + i * 8, true);
                        const len = mem.getUint32(iovs + i * 8 + 4, true);
                        const str = runtime.readStringN(ptr, len);
                        if (fd === 1) {
                            runtime.print(str);
                        } else if (fd === 2) {
                            console.error(str);
                        }
                        totalWritten += len;
                    }
                    mem.setUint32(nwritten, totalWritten, true);
                    return 0; // success
                },
                // Stubs for other WASI functions
                fd_close() { return 0; },
                fd_seek() { return 0; },
                fd_read() { return 0; },
                environ_sizes_get(countPtr, sizePtr) {
                    const mem = new DataView(runtime.memory.buffer);
                    mem.setUint32(countPtr, 0, true);
                    mem.setUint32(sizePtr, 0, true);
                    return 0;
                },
                environ_get() { return 0; },
                proc_exit(code) {
                    if (code !== 0) {
                        console.error(`Process exited with code ${code}`);
                    }
                },
                args_sizes_get(countPtr, sizePtr) {
                    const mem = new DataView(runtime.memory.buffer);
                    mem.setUint32(countPtr, 0, true);
                    mem.setUint32(sizePtr, 0, true);
                    return 0;
                },
                args_get() { return 0; },
                clock_time_get(id, precision, timePtr) {
                    const mem = new DataView(runtime.memory.buffer);
                    const now = BigInt(Date.now()) * 1000000n; // ms -> ns
                    mem.setBigUint64(timePtr, now, true);
                    return 0;
                },
            }
        };
    }

    /**
     * Print output — routes to console or DOM
     */
    print(str) {
        this.outputBuffer += str;
        // Flush on newline
        if (str.includes('\n')) {
            const lines = this.outputBuffer.split('\n');
            for (let i = 0; i < lines.length - 1; i++) {
                console.log(lines[i]);
                if (this.onOutput) this.onOutput(lines[i]);
            }
            this.outputBuffer = lines[lines.length - 1];
        }
    }

    /**
     * Flush remaining output
     */
    flush() {
        if (this.outputBuffer) {
            console.log(this.outputBuffer);
            if (this.onOutput) this.onOutput(this.outputBuffer);
            this.outputBuffer = '';
        }
    }

    /**
     * Load and run a .wasm file
     */
    async run(wasmPath) {
        const imports = this.getImports();

        let wasmModule;
        if (typeof fetch !== 'undefined') {
            // Browser
            const response = await fetch(wasmPath);
            const buffer = await response.arrayBuffer();
            wasmModule = await WebAssembly.instantiate(buffer, imports);
        } else {
            // Node.js
            const fs = await import('fs');
            const buffer = fs.readFileSync(wasmPath);
            wasmModule = await WebAssembly.instantiate(buffer, imports);
        }

        this.instance = wasmModule.instance;
        this.memory = this.instance.exports.memory || imports.env.memory;

        // Call _start (WASI) or main
        if (this.instance.exports._start) {
            this.instance.exports._start();
        } else if (this.instance.exports.main) {
            this.instance.exports.main();
        }

        this.flush();
        return this;
    }
}

// Export for both browser and Node.js
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { StratosRuntime };
}
if (typeof window !== 'undefined') {
    window.StratosRuntime = StratosRuntime;
}
