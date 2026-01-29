# Building the Stratos Interpreter

This document describes how to build the Stratos interpreter with native standard library support.

## Prerequisites

- CMake 3.20 or higher
- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- Make or Ninja build system

## Build Instructions

### Using CMake (Recommended)

```bash
cd interpreter/C++/build
cmake ..
make -j$(nproc)
```

Or with Ninja:

```bash
cd interpreter/C++/build
cmake -G Ninja ..
ninja
```

### Manual Build (for Development)

If CMake is not available, you can build manually:

```bash
cd interpreter/C++

# Create build directory
mkdir -p build
cd build

# Compile all sources
g++ -std=c++20 -I../include \
    ../src/main.cpp \
    ../src/lexer/*.cpp \
    ../src/parser/*.cpp \
    ../src/sema/*.cpp \
    ../src/codegen/*.cpp \
    ../src/optimizer/*.cpp \
    ../src/config/*.cpp \
    ../src/runtime/*.cpp \
    -o stratos \
    -lpthread
```

## Project Structure

```
interpreter/C++/
├── include/stratos/          # Header files
│   ├── NativeRegistry.h      # Native function registry
│   ├── TypeSystem.h          # Generic type system
│   ├── AsyncRuntime.h        # Async/await and concurrency runtime
│   ├── IRGenerator.h         # LLVM IR generation
│   └── ...
├── src/
│   ├── runtime/              # Runtime implementations
│   │   ├── NativeRegistry.cpp    # Standard library native bindings
│   │   ├── TypeSystem.cpp        # Type system implementation
│   │   └── AsyncRuntime.cpp      # Async runtime implementation
│   ├── lexer/                # Lexical analysis
│   ├── parser/               # Syntax analysis
│   ├── sema/                 # Semantic analysis
│   ├── codegen/              # Code generation
│   ├── optimizer/            # IR optimization
│   ├── config/               # Project configuration
│   └── main.cpp              # Entry point
└── CMakeLists.txt            # Build configuration
```

## Native Standard Library

The interpreter includes C++ implementations for the following standard library modules:

### Core Modules
- **math**: Mathematical functions (trigonometry, logarithms, statistics)
- **strings**: String manipulation (case conversion, searching, splitting)
- **io**: File I/O operations (reading, writing, directory operations)
- **log**: Structured logging (debug, info, warn, error, fatal)
- **time**: Time and duration operations

### Advanced Modules (Partial Implementation)
- **collections**: Generic data structures (List, Map, Set, Queue, Stack)
- **async**: Promise-based async/await runtime
- **concurrent**: Goroutines, channels, mutexes, wait groups

## Features

### Native Function Bindings

The interpreter uses a `NativeRegistry` system to bind C++ implementations to Stratos functions:

```cpp
// Example: Register a native function
NativeRegistry::getInstance().registerFunction("math", "sqrt",
    [](const std::vector<std::any>& args) -> std::any {
        double x = std::any_cast<double>(args[0]);
        return std::sqrt(x);
    }
);
```

### Generic Type System

The interpreter includes a comprehensive type system with support for:
- Generic type parameters (T, U, V)
- Parameterized types (List<T>, Map<K,V>)
- Type inference and unification
- Monomorphization for generic functions

```cpp
// Create a generic List<int> type
auto listType = TypeSystem::getInstance().parameterizedType(
    "List",
    {TypeSystem::getInstance().intType()}
);
```

### Async/Await Runtime

Full runtime support for asynchronous programming:
- Promise<T> implementation
- Event loop for async task scheduling
- Promise.all, Promise.race combinators
- Channel<T> for concurrent communication
- Goroutine spawning and management

```cpp
// Create and resolve a promise
auto promise = AsyncRuntime::getInstance().resolve<int>(42);

// Spawn an async task
auto future = AsyncRuntime::getInstance().spawn<int>([]() {
    return compute();
});
```

## Testing

### Run All Tests

```bash
cd build
./stratos test
```

### Run Specific Test

```bash
./stratos compile ../cases/01_basics.st
```

### Test with Standard Library Examples

```bash
./stratos ../../../samples/stdlib_examples/01_basic_math.st
```

## Standard Library Modules

### Implemented Modules

| Module | Native Bindings | Status |
|--------|----------------|--------|
| math | ✅ Complete | Trigonometry, logarithms, random |
| strings | ✅ Complete | All string operations |
| io | ✅ Complete | File I/O, directories |
| log | ✅ Complete | Structured logging |
| time | ✅ Partial | Basic time operations |
| collections | ⚠️ Partial | Generic runtime needed |
| async | ⚠️ Partial | Event loop implemented |
| concurrent | ⚠️ Partial | Goroutines, channels |

### Pending Modules

- json (JSON parsing/serialization)
- base64 (Base64 encoding/decoding)
- csv (CSV parsing)
- net (TCP/UDP networking)
- http (HTTP server/client)
- os (Operating system interface)
- crypto (Cryptographic functions)
- testing (Unit testing framework)

## Development Workflow

### Adding a New Native Function

1. **Declare in Stratos** (std/module/init.st):
```stratos
fn myFunction(x: int) int;
```

2. **Implement in C++** (src/runtime/NativeRegistry.cpp):
```cpp
void NativeRegistry::initModule() {
    registerFunction("module", "myFunction",
        [](const std::vector<std::any>& args) -> std::any {
            int x = std::any_cast<int>(args[0]);
            return x * 2;
        }
    );
}
```

3. **Register Module** (src/runtime/NativeRegistry.cpp):
```cpp
void NativeRegistry::initializeStdlib() {
    initMath();
    initStrings();
    initModule();  // Add your module
}
```

### Debugging

Enable verbose output:

```bash
./stratos compile myfile.st --verbose
```

Generate IR without optimization:

```bash
./stratos compile myfile.st -o output.ll --no-optimize
```

## Architecture

### Compilation Pipeline

```
Source Code (.st)
    ↓
Lexer → Tokens
    ↓
Parser → AST
    ↓
Semantic Analyzer → Annotated AST
    ↓
IR Generator → LLVM IR
    ↓
Optimizer → Optimized IR
    ↓
Output (.ll file)
```

### Runtime Systems

1. **NativeRegistry**: Maps qualified function names to C++ implementations
2. **TypeSystem**: Handles generic types and type inference
3. **AsyncRuntime**: Manages promises, event loop, and async tasks
4. **GoroutineRuntime**: Spawns and manages lightweight threads

## Performance Considerations

- Generic types use type erasure (pointer-based) for now
- Native functions are called via std::any for type flexibility
- Future optimization: JIT compilation for hot paths
- Channel operations use condition variables for efficiency

## Troubleshooting

### Build Errors

**Error**: `undefined reference to NativeRegistry`
- **Solution**: Ensure `src/runtime/*.cpp` is included in CMakeLists.txt

**Error**: `std::filesystem not found`
- **Solution**: Use GCC 9+ or add `-lstdc++fs` linker flag

**Error**: `std::any_cast fails`
- **Solution**: Check argument types match function signature

### Runtime Errors

**Error**: `Native function not found`
- **Solution**: Ensure `initializeStdlib()` is called in main()

**Error**: `Promise never resolves`
- **Solution**: EventLoop may not be running; call `EventLoop::getInstance().tick()` manually for testing

## Next Steps

1. **Complete Native Bindings**: Implement remaining stdlib modules
2. **IR Integration**: Update IRGenerator to emit native function calls
3. **Generic Instantiation**: Implement full monomorphization
4. **LLVM Backend**: Generate actual machine code instead of IR text
5. **JIT Execution**: Add LLVM JIT for immediate execution
6. **Debugger Support**: Add DWARF debug info generation

## Resources

- [Stratos Language Spec](../../design/language_spec.md)
- [Standard Library Design](../../design/standard_library.md)
- [LLVM Documentation](https://llvm.org/docs/)
- [C++20 Features](https://en.cppreference.com/w/cpp/20)

## License

Part of the Stratos programming language project.
