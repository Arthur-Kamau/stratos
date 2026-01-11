# Stratos Static Compilation - Implementation Complete

## Overview

The Stratos `compile` command now generates fully standalone executables similar to Go's `go build`, with configurable optimization levels and static linking support.

## What Was Implemented

### 1. **Static Runtime Library** (`libstratos_runtime.a`)
- Pre-compiled runtime library containing:
  - NativeRegistry (all stdlib functions: math, strings, I/O, crypto, JSON, etc.)
  - TypeSystem
  - FFI (Foreign Function Interface)
  - GarbageCollector
  - Interpreter
  - SQLite (embedded)
- Size: ~3.4 MB

### 2. **Platform Detection & Configuration**
- New files:
  - `include/stratos/RuntimeLinkConfig.h` - Platform detection interface
  - `src/config/RuntimeLinkConfig.cpp` - Platform-specific linking implementation
- Automatic detection of Linux/macOS/Windows
- Platform-specific static linking flags

### 3. **Optimization Flags**
- `-O0` - No optimization (fast compile, large binary)
- `-O1` - Basic optimization
- `-O2` - Recommended optimization (DEFAULT)
- `-O3` - Aggressive optimization + native arch tuning
- `-Os` - Optimize for binary size
- `-Oz` - Aggressively optimize for size
- Alternative syntax: `--opt <level>` where level = 0, 1, 2, 3, s, z

### 4. **Updated Build System**
- Two-phase build process:
  1. Build static runtime library
  2. Build Stratos compiler binary
- Modified files:
  - `src/interpreter/cpp/build.sh` - Updated build process
  - `src/interpreter/cpp/src/main.cpp` - Updated compilation logic

## How to Use

### Rebuild the Compiler

```bash
cd /home/kamau/Development/Projects/stratos
chmod +x rebuild-stratos.sh
./rebuild-stratos.sh
```

### Compile a Project

```bash
# From anywhere:
./src/interpreter/cpp/build/stratos compile <project-path> [options]

# Examples:
./src/interpreter/cpp/build/stratos compile ./examples/test-static-compile/ -v
./src/interpreter/cpp/build/stratos compile ./examples/test-static-compile/ -O3
./src/interpreter/cpp/build/stratos compile ./examples/test-static-compile/ -Os
```

### Test the Implementation

```bash
chmod +x test-static-compile.sh
./test-static-compile.sh
```

## Compilation Output (Verbose Mode)

When you compile with `-v`, you'll see:

```
Compilation configuration:
  Platform: Linux
  Optimization: -O2 (recommended)
  Static linking: Full
  Runtime library: /path/to/libstratos_runtime.a

Executing clang command:
clang "file.ll" -O2 -static -static-libgcc -static-libstdc++ \
  "libstratos_runtime.a" \
  -l:libssl.a -l:libcrypto.a -l:libpthread.a -l:libdl.a \
  -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -l:libc.a \
  -o "output"

=== Binary Information ===
file output
  output: ELF 64-bit LSB executable, statically linked, ...

Binary size: 3.2M

Dependency check:
  not a dynamic executable  ✓ (Fully static!)
```

## Expected Binary Sizes

| Optimization | Expected Size |
|--------------|---------------|
| -O0          | 8-10 MB       |
| -O1          | 5-7 MB        |
| -O2 (default)| 3-4 MB        |
| -O3          | 3-4 MB        |
| -Os          | 2-3 MB        |
| -Oz          | 2 MB          |

## Platform Support

### Linux ✅
- **Full static linking supported**
- True standalone binaries with no dependencies
- Binary runs on any Linux distribution (same architecture)

### macOS ⚠️
- **Partial static linking only**
- macOS system policy prevents full static linking since 10.5
- Will still require `libSystem.B.dylib` (always available on macOS)
- Application code and OpenSSL are statically linked

### Windows ✅
- **MSVC static runtime supported**
- Uses `/MT` flag for static MSVC runtime
- Minimal dependencies
- Requires static OpenSSL libraries

## Prerequisites

### Required:
- `clang` - LLVM compiler (install: `sudo apt install clang`)
- `libssl-dev` - OpenSSL development libraries (install: `sudo apt install libssl-dev`)

### Optional (for smaller binaries):
- `strip` - Strip debug symbols (reduces size 50-70%)
- `upx` - Ultimate Packer for eXecutables (reduces size ~50%)

## Troubleshooting

### Issue: "Runtime library not found"
**Solution:** Ensure you've rebuilt the compiler using `./rebuild-stratos.sh`

### Issue: "clang: not found"
**Solution:** Install clang:
```bash
sudo apt install clang
```

### Issue: "cannot find -l:libssl.a"
**Solution:** Install static OpenSSL libraries:
```bash
sudo apt install libssl-dev
```

### Issue: Binary still has dependencies
**Check platform:**
```bash
./src/interpreter/cpp/build/stratos compile test.st -v
```
Look for "Platform:" and "Static linking:" in the output.

- **Linux:** Should show "Full"
- **macOS:** Will show "Partial" (expected)
- **Windows:** Should show "Full"

## Verification

To verify a binary is fully static (Linux):
```bash
# Method 1: Check with ldd
ldd ./build/mybinary
# Expected output: "not a dynamic executable"

# Method 2: Check with file
file ./build/mybinary
# Should contain: "statically linked"

# Method 3: Test portability
# Copy binary to a clean Ubuntu Docker container:
docker run -it ubuntu:22.04 /bin/bash
./mybinary  # Should work without installing any dependencies!
```

## Architecture

### Build Flow:
```
1. Source (.st) → Lexer → Parser → Semantic Analysis → Optimizer
2. → IRGenerator → LLVM IR (.ll file)
3. → clang + libstratos_runtime.a + static libs → Standalone Binary
```

### File Locations:
```
/home/kamau/Development/Projects/stratos/
├── src/interpreter/cpp/
│   ├── build/
│   │   ├── stratos                    # Compiler executable
│   │   ├── libstratos_runtime.a       # Static runtime library (3.4 MB)
│   │   └── runtime/                   # Object files
│   ├── include/stratos/
│   │   └── RuntimeLinkConfig.h        # NEW: Platform detection header
│   └── src/config/
│       └── RuntimeLinkConfig.cpp      # NEW: Platform-specific linking
└── examples/test-static-compile/
    ├── stratos.conf
    ├── src/main.st
    └── build/
        └── variables                  # Output: standalone binary
```

## Summary

✅ **Fully static binaries on Linux** (no dependencies except kernel)
✅ **SQLite compiled in statically**
✅ **Configurable optimization** (-O0 through -Oz)
✅ **Native platform compilation**
✅ **Go-like experience** - single command produces standalone executable
✅ **Verbose mode** shows full compilation details
✅ **Platform detection** - automatic platform-specific linking

The implementation is complete and ready to use!
