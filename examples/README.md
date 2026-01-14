# Stratos Examples

This directory contains example Stratos projects demonstrating various features of the language.

## Running Examples

### Run All Examples

You can run all examples at once using the provided scripts:

**PowerShell (Windows):**
```powershell
cd examples
.\run-all-examples.ps1
```

**Bash (Linux/Mac/Git Bash):**
```bash
cd examples
./run-all-examples.sh
```

### Run Individual Examples

To run a specific example:

```bash
# From project root
./src/interpreter/cpp/build/stratos.exe run examples/<example-name>/src/main.st

# Examples:
./src/interpreter/cpp/build/stratos.exe run examples/callbacks/src/main.st
./src/interpreter/cpp/build/stratos.exe run examples/stdlib-examples/07_strings/src/main.st
```

## Example Categories

### Language Features
- **callbacks** - Higher-order functions and closures
- **arrays** - Array operations and manipulation
- **loops** - For loops, while loops, and iteration
- **maps** - Map/dictionary data structures
- **comments-and-documentation** - Code documentation examples

### Standard Library Examples
Located in `stdlib-examples/`:
- **01_math** - Mathematical operations
- **02_io** - File I/O operations
- **03_concurrency** - Async/await and concurrent operations
- **04_collections** - Lists, sets, queues, stacks
- **05_regex** - Regular expression matching
- **06_testing** - Testing framework usage
- **07_strings** - String manipulation and formatting

### Advanced Features
- **async-demo** - Asynchronous programming patterns
- **terminal-demo** - Terminal UI and input handling
- **regex-demo** - Advanced regex patterns
- **ffi_c_math** - FFI with C library
- **ffi_cpp_string** - FFI with C++ library

## Example Structure

Each example follows this structure:
```
example-name/
├── src/
│   └── main.st          # Main source file
├── README.md            # Example documentation (optional)
└── stratos.toml         # Project configuration (optional)
```
