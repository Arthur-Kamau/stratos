#!/bin/bash
# Simple build script for Stratos interpreter
# This script compiles the Stratos interpreter without requiring cmake/ninja

echo "Building Stratos interpreter..."

# Compile SQLite as C first
echo "Compiling SQLite..."
gcc -c libs/sqlite/sqlite3.c -o build/sqlite3.o

if [ $? -ne 0 ]; then
    echo "[FAILED] SQLite compilation failed"
    exit 1
fi

# Compile Stratos C++ code and link with SQLite
echo "Compiling Stratos..."
g++ -std=c++20 -I include -I libs/sqlite \
  src/main.cpp \
  src/lexer/Lexer.cpp \
  src/parser/Parser.cpp \
  src/sema/SemanticAnalyzer.cpp \
  src/codegen/IRGenerator.cpp \
  src/optimizer/Optimizer.cpp \
  src/runtime/Interpreter.cpp \
  src/runtime/GarbageCollector.cpp \
  src/runtime/NativeRegistry.cpp \
  src/runtime/FFI.cpp \
  src/runtime/WebSocket.cpp \
  src/config/ProjectConfig.cpp \
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
  build/sqlite3.o \
  -o build/stratos -lpthread -ldl -lssl -lcrypto

if [ $? -eq 0 ]; then
    echo "[SUCCESS] Build successful! Binary: build/stratos"
    ls -lh build/stratos
else
    echo "[FAILED] Build failed"
    exit 1
fi
