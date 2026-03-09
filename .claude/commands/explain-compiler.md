# Explain Stratos Compiler Internals

Explain how a specific part of the Stratos compiler/interpreter works.

## Arguments
- $ARGUMENTS: Component or concept to explain (e.g., "lexer", "parser", "gc", "type system", "async runtime")

## Instructions

Read the relevant source files and provide a clear explanation of how the component works.

### Component Map

| Topic | Header File | Implementation |
|-------|------------|----------------|
| lexer, tokenizer | `src/include/stratos/Lexer.h`, `Token.h` | `src/src/lexer/Lexer.cpp` |
| parser, ast | `src/include/stratos/Parser.h`, `AST.h` | `src/src/parser/Parser.cpp` |
| semantic analysis, type checking | `src/include/stratos/SemanticAnalyzer.h` | `src/src/sema/SemanticAnalyzer.cpp` |
| interpreter, runtime | `src/include/stratos/Interpreter.h` | `src/src/runtime/Interpreter.cpp` |
| garbage collector, gc, memory | `src/include/stratos/GarbageCollector.h` | `src/src/runtime/GarbageCollector.cpp` |
| type system, generics | `src/include/stratos/TypeSystem.h` | `src/src/runtime/TypeSystem.cpp` |
| native registry, stdlib bindings | `src/include/stratos/NativeRegistry.h` | `src/src/runtime/NativeRegistry.cpp` |
| ffi, foreign functions | `src/include/stratos/FFI.h` | `src/src/runtime/FFI.cpp` |
| async, promises, event loop | `src/include/stratos/AsyncRuntime.h` | `src/src/runtime/AsyncRuntime.cpp` |
| http server | `src/include/stratos/HttpServer.h` | `src/src/runtime/HttpServer.cpp` |
| websocket | `src/include/stratos/WebSocket.h` | `src/src/runtime/WebSocket.cpp` |
| ir, codegen, llvm | `src/include/stratos/IRGenerator.h` | `src/src/codegen/IRGenerator.cpp` |
| optimizer | `src/include/stratos/Optimizer.h` | `src/src/optimizer/Optimizer.cpp` |
| formatter, fmt | `src/include/stratos/Formatter.h` | `src/src/formatter/Formatter.cpp` |
| project config, stratos.conf | `src/include/stratos/ProjectConfig.h` | `src/src/config/ProjectConfig.cpp` |
| dependencies, package manager | `src/include/stratos/DependencyManager.h` | `src/src/config/DependencyManager.cpp` |
| lock file | `src/include/stratos/LockFile.h` | `src/src/config/LockFile.cpp` |
| cache | `src/include/stratos/CacheManager.h` | `src/src/config/CacheManager.cpp` |
| devtools server | `src/include/stratos/DevToolsServer.h` | `src/src/devtools/DevToolsServer.cpp` |
| logger | `src/include/stratos/Logger.h` | `src/src/devtools/Logger.cpp` |
| memory profiler | `src/include/stratos/MemoryProfiler.h` | `src/src/devtools/MemoryProfiler.cpp` |
| debug engine | `src/include/stratos/DebugEngine.h` | `src/src/devtools/DebugEngine.cpp` |
| doc generation | `src/include/stratos/DocExtractor.h` | `src/src/doc/DocExtractor.cpp` |
| symbols, symbol table | `src/include/stratos/Symbol.h`, `SymbolTable.h` | (used by SemanticAnalyzer) |
| pipeline, compilation | `src/src/main.cpp` | Entry point, all stages |

### Response Format

1. Read the relevant header and implementation files
2. Explain the high-level architecture and design decisions
3. Walk through the key data structures and algorithms
4. Show how this component connects to the rest of the pipeline
5. Highlight any interesting patterns or trade-offs
