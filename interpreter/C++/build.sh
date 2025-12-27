#!/bin/bash
# Simple build script for Stratos interpreter
# This script compiles the Stratos interpreter without requiring cmake/ninja

echo "Building Stratos interpreter..."

g++ -std=c++20 -I include \
  src/main.cpp \
  src/lexer/Lexer.cpp \
  src/parser/Parser.cpp \
  src/sema/SemanticAnalyzer.cpp \
  src/codegen/IRGenerator.cpp \
  src/optimizer/Optimizer.cpp \
  src/runtime/Interpreter.cpp \
  src/runtime/NativeRegistry.cpp \
  src/config/ProjectConfig.cpp \
  src/config/DependencyManager.cpp \
  src/config/LockFile.cpp \
  -o build/stratos -lpthread

if [ $? -eq 0 ]; then
    echo "✓ Build successful! Binary: build/stratos"
    ls -lh build/stratos
else
    echo "✗ Build failed"
    exit 1
fi
