#!/usr/bin/env pwsh
# Simple build script for Stratos interpreter
# This script compiles the Stratos interpreter without requiring cmake/ninja

Write-Host "Building Stratos interpreter..." -ForegroundColor Cyan

# Compile SQLite as C first
Write-Host "Compiling SQLite..." -ForegroundColor Yellow
gcc -c libs/sqlite/sqlite3.c -o build/sqlite3.o

if ($LASTEXITCODE -ne 0) {
    Write-Host "[FAILED] SQLite compilation failed" -ForegroundColor Red
    exit 1
}

# Compile Stratos C++ code and link with SQLite
Write-Host "Compiling Stratos..." -ForegroundColor Yellow
g++ -std=c++20 -I include -I libs/sqlite `
  src/main.cpp `
  src/lexer/Lexer.cpp `
  src/parser/Parser.cpp `
  src/sema/SemanticAnalyzer.cpp `
  src/codegen/IRGenerator.cpp `
  src/optimizer/Optimizer.cpp `
  src/runtime/Interpreter.cpp `
  src/runtime/GarbageCollector.cpp `
  src/runtime/NativeRegistry.cpp `
  src/runtime/FFI.cpp `
  src/runtime/WebSocket.cpp `
  src/config/ProjectConfig.cpp `
  src/config/DependencyManager.cpp `
  src/config/LockFile.cpp `
  src/config/CacheManager.cpp `
  src/config/RuntimeLinkConfig.cpp `
  src/formatter/Formatter.cpp `
  src/devtools/Logger.cpp `
  src/devtools/DevToolsServer.cpp `
  src/devtools/MemoryProfiler.cpp `
  src/doc/DocExtractor.cpp `
  src/doc/HTMLDocGenerator.cpp `
  src/doc/MarkdownDocGenerator.cpp `
  src/doc/JSONDocGenerator.cpp `
  build/sqlite3.o `
  -o build/stratos.exe -lws2_32 -lssl -lcrypto

if ($LASTEXITCODE -eq 0) {
    Write-Host "[SUCCESS] Build successful! Binary: build/stratos.exe" -ForegroundColor Green
    Get-Item build/stratos.exe | Format-Table Name, Length, LastWriteTime -AutoSize
} else {
    Write-Host "[FAILED] Build failed" -ForegroundColor Red
    exit 1
}
