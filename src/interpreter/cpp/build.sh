#!/bin/bash
# Build script for Stratos interpreter with static runtime library
# This script creates a standalone static runtime library, then builds the compiler

echo "Building Stratos interpreter with static runtime..."

# === PHASE 1: Build Static Runtime Library ===
echo ""
echo "Step 1: Building Stratos runtime static library..."
echo "=============================================="

# Create runtime build directory
mkdir -p build/runtime

# Runtime sources to compile
# Note: AsyncRuntime.cpp temporarily excluded due to template compilation issues
RUNTIME_SOURCES=(
    "src/runtime/NativeRegistry.cpp"
    "src/runtime/GarbageCollector.cpp"
    "src/runtime/TypeSystem.cpp"
    "src/runtime/FFI.cpp"
    "src/runtime/WebSocket.cpp"
    "src/runtime/Interpreter.cpp"
    "src/runtime/HttpServer.cpp"
)

# Compile each runtime source to object file with -fPIC
RUNTIME_OBJS=""
for src in "${RUNTIME_SOURCES[@]}"; do
    obj_name="build/runtime/$(basename $src .cpp).o"
    echo "  Compiling $src..."
    g++ -std=c++20 -O2 -fPIC -I include -I libs/sqlite -c $src -o $obj_name

    if [ $? -ne 0 ]; then
        echo "[FAILED] Runtime compilation failed: $src"
        exit 1
    fi

    RUNTIME_OBJS="$RUNTIME_OBJS $obj_name"
done

# Compile SQLite separately with -fPIC
echo "  Compiling SQLite..."
gcc -O2 -fPIC -c libs/sqlite/sqlite3.c -o build/runtime/sqlite3.o

if [ $? -ne 0 ]; then
    echo "[FAILED] SQLite compilation failed"
    exit 1
fi

# Create static library archive
echo "  Creating libstratos_runtime.a..."
ar rcs build/libstratos_runtime.a $RUNTIME_OBJS build/runtime/sqlite3.o

if [ $? -ne 0 ]; then
    echo "[FAILED] Failed to create static library"
    exit 1
fi

# Index the archive for faster linking
ranlib build/libstratos_runtime.a

echo "[SUCCESS] Runtime library created: build/libstratos_runtime.a"
ls -lh build/libstratos_runtime.a

# === PHASE 2: Build Stratos Compiler Binary ===
echo ""
echo "Step 2: Compiling Stratos compiler..."
echo "=============================================="

# Compile the main Stratos compiler, linking against the runtime library
# Note: AsyncRuntime.cpp excluded due to template compilation issues
g++ -std=c++20 -I include -I libs/sqlite \
  src/main.cpp \
  src/lexer/Lexer.cpp \
  src/parser/Parser.cpp \
  src/sema/SemanticAnalyzer.cpp \
  src/codegen/IRGenerator.cpp \
  src/optimizer/Optimizer.cpp \
  src/config/ProjectConfig.cpp \
  src/config/RuntimeLinkConfig.cpp \
  src/config/DependencyManager.cpp \
  src/config/LockFile.cpp \
  src/config/CacheManager.cpp \
  src/formatter/Formatter.cpp \
  src/devtools/Logger.cpp \
  src/devtools/DevToolsServer.cpp \
  src/devtools/MemoryProfiler.cpp \
  src/doc/DocExtractor.cpp \
  src/doc/HTMLDocGenerator.cpp \
  src/doc/MarkdownDocGenerator.cpp \
  src/doc/JSONDocGenerator.cpp \
  build/libstratos_runtime.a \
  -o build/stratos -lpthread -ldl -lssl -lcrypto

if [ $? -eq 0 ]; then
    echo ""
    echo "=============================================="
    echo "[SUCCESS] Build successful!"
    echo "=============================================="
    echo ""
    echo "Build artifacts:"
    echo "  Compiler:        build/stratos"
    echo "  Runtime library: build/libstratos_runtime.a"
    echo ""
    ls -lh build/stratos
    ls -lh build/libstratos_runtime.a
else
    echo "[FAILED] Build failed"
    exit 1
fi
