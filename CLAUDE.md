# Stratos Programming Language

A fast, type-safe programming language with a C++ interpreter/compiler. The syntax is a blend of Go, Kotlin, and TypeScript.

## Project Structure

```
stratos/
├── src/                        # C++ interpreter/compiler source
│   ├── include/stratos/        # Header files (.h)
│   ├── src/                    # Implementation files (.cpp)
│   │   ├── lexer/              # Lexical analysis (Lexer.cpp)
│   │   ├── parser/             # Syntax analysis (Parser.cpp)
│   │   ├── sema/               # Semantic analysis (SemanticAnalyzer.cpp)
│   │   ├── codegen/            # IR code generation (IRGenerator.cpp)
│   │   ├── optimizer/          # IR optimization (Optimizer.cpp)
│   │   ├── runtime/            # Interpreter, GC, FFI, HTTP, WebSocket
│   │   ├── config/             # Project config, deps, cache, lock files
│   │   ├── devtools/           # Logger, DevToolsServer, MemoryProfiler, DebugEngine
│   │   ├── formatter/          # Code formatter (Formatter.cpp)
│   │   ├── doc/                # Doc generation (HTML, Markdown, JSON)
│   │   └── main.cpp            # Entry point
│   ├── libs/sqlite/            # SQLite amalgamation (vendored)
│   ├── build.sh                # Linux build script
│   ├── build.ps1               # Windows (PowerShell) build script
│   ├── CMakeLists.txt          # CMake build configuration
│   └── BUILD.md                # Build documentation
├── std/                        # Standard library (.st files)
│   ├── async/                  # Async/await, promises
│   ├── collections/            # List, Map, Set, Queue, Stack
│   ├── concurrent/             # Goroutines, channels, mutex, waitgroup
│   ├── convert/                # Type conversion utilities
│   ├── crypto/                 # Hashing, encryption
│   ├── db/                     # SQLite, PostgreSQL, MySQL, Redis
│   ├── encoding/               # JSON, base64, CSV, XML, YAML
│   ├── ffi/                    # Foreign function interface
│   ├── fs/                     # File system operations
│   ├── io/                     # I/O operations
│   ├── log/                    # Structured logging
│   ├── math/                   # Math functions
│   ├── net/                    # TCP/UDP/HTTP networking
│   ├── os/                     # OS interface, env, process
│   ├── regex/                  # Regular expressions
│   ├── strings/                # String utilities
│   ├── template/               # Template engine
│   ├── terminal/               # Terminal colors, cursor, input
│   ├── testing/                # Unit testing framework
│   ├── time/                   # Time, duration, formatting
│   ├── websocket/              # WebSocket client/server
│   └── zip/                    # Archive compression
├── examples/                   # Example projects (70+ demos)
├── external/                   # External packages (git submodules)
│   ├── stratos-cli-args/       # CLI argument parsing
│   ├── stratos-colors/         # Terminal colors package
│   ├── stratos-linq/           # LINQ-style queries
│   └── stratos-valid/          # Validation library
├── docs/                       # Documentation site (VitePress)
├── stratos-tutorial/           # Interactive tutorial (SvelteKit + Monaco)
├── devtools/                   # Chrome-style DevTools UI
├── tooling/                    # IDE extensions
│   ├── vscode/                 # VS Code extension
│   └── intellij/               # IntelliJ plugin
├── tests/                      # Test infrastructure
│   ├── devtools/               # DevTools tests
│   └── memory/                 # Memory/GC tests
├── scripts/                    # Utility scripts
│   ├── install.sh              # Installation script
│   └── test-all.sh             # Automated test runner
└── .github/                    # CI/CD workflows
```

## Building

### Prerequisites
- C++20 compiler (GCC 10+, Clang 12+)
- CMake 3.20+
- System libraries: pthread, dl, ssl, crypto, pq (PostgreSQL), mysqlclient, hiredis

### Build Commands
```bash
cd src && bash build.sh          # Build on Linux (produces build/stratos)
cd src && pwsh build.ps1         # Build on Windows (produces build/stratos.exe)
```

The build produces two artifacts:
- `src/build/stratos` — the compiler/interpreter binary
- `src/build/libstratos_runtime.a` — static runtime library

### Quick Rebuild
```bash
./rebuild-stratos.sh             # Convenience wrapper
```

## CLI Commands

The `stratos` binary supports these commands:

| Command | Description |
|---------|-------------|
| `stratos run <file/dir>` | Run a Stratos program |
| `stratos build <dir>` | Build a project |
| `stratos new <name>` | Create a new project |
| `stratos check <file/dir>` | Syntax/semantic validation (no codegen) |
| `stratos fmt <file/dir> [-w]` | Format code (`-w` writes in-place) |
| `stratos test` | Run tests |
| `stratos doc <file>` | Generate documentation |
| `stratos compile <dir> [-v]` | Compile to binary |
| `stratos deps` | Manage dependencies |

## Stratos Language Quick Reference

### File Extension
`.st`

### Project Configuration (`stratos.conf`)
```hocon
project {
  name = my_project
  version = "1.0.0"
  description = "Project description"
}

build {
  entry = src/main.st
  output = build/my_project
}

dependencies {
  // name = "git_url"
}
```

### Package Declaration & Imports
```stratos
package main;

import "std/math";
import "std/strings";
import "std/io" as fileio;

// Alternative short form:
use collections;
use async;
use db;
```

### Variables
```stratos
val x = 42;            // immutable (type inferred)
var y: string = "hi";  // mutable with explicit type
val z: float = 3.14;
```

### Types
Primitives: `int`, `float`, `double`, `string`, `bool`, `char`, `void`, `any`
Low-level: `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `usize`, `isize`
Collections: `Array<T>`, `Map<K, V>`, `array`, `map`
Special: `nil`, `null`, `optional`, `unit`
Generic: `Result<T, E>`, `Option<T>`, `Future<T>`, `Channel<T>`

### Functions
```stratos
fn add(a: int, b: int) int {
    return a + b;
}

fn greet(name: string) string => "Hello, ${name}!";  // expression body

// Lambda / closures
val double = fn(x: int) int => x * 2;
```

### Control Flow
```stratos
// If/else
if condition {
    // ...
} else if other {
    // ...
} else {
    // ...
}

// When (pattern matching, like Kotlin's when)
when value {
    1 -> println("one");
    2, 3 -> println("two or three");
    in 4..10 -> println("four to ten");
    is string -> println("it's a string");
    else -> println("default");
}

// Loops
for i in 0..10 { }
for item in collection { }
while condition { }
loop { break; }  // infinite loop

// Select (Go-style channel select)
select {
    case msg = <-channel1 -> handle(msg);
    case <-channel2 -> handleOther();
    default -> println("no message");
}
```

### Classes & OOP
```stratos
class Animal {
    var name: string;       // fields use var/val
    var age: int;

    constructor(name: string, age: int) {
        this.name = name;
        this.age = age;
    }

    fn speak() string {
        return "${this.name} speaks";
    }
}

class Dog : Animal {            // inheritance with ':'
    constructor(name: string, age: int) {
        this.name = name;
        this.age = age;
    }

    pub fn speak() string {     // pub for public access
        return "${this.name} barks";
    }
}
```

### Interfaces
```stratos
interface Drawable {
    fn draw() void;
    fn area() float;
}

class Circle : Drawable {       // implements via ':'
    // must implement draw() and area()
}
```

### Structs
```stratos
struct Point {
    x: float;
    y: float;
}

val p = Point { x: 1.0, y: 2.0 };
```

### Enums
```stratos
enum Color {
    Red,
    Green,
    Blue,
    Custom(r: int, g: int, b: int)
}

val c = Color.Red;
```

### Arrays & Maps
```stratos
val nums: array<int> = [1, 2, 3, 4, 5];
val names: map<string, int> = {"alice": 1, "bob": 2};

// Array methods: push, pop, length, map, filter, forEach, reduce
nums.push(6);
val doubled = nums.map(fn(x: int) int => x * 2);
```

### String Interpolation
```stratos
val name = "World";
println("Hello, ${name}!");      // expression interpolation
println("Hello, $name!");        // simple variable interpolation
println("Sum: ${2 + 3}");
println("Price: \$100");         // escaped dollar sign
```

### Printing
```stratos
println("with newline");
print("no newline");
printf("formatted: {} + {} = {}\n", a, b, a + b);  // {} placeholders
```

### Error Handling
```stratos
val result = expect divide(10, 0) else {
    println("Division failed");
    return;
};

// Try/catch
try {
    riskyOperation();
} catch (e: Error) {
    println("Error: ${e.message}");
}
```

### Async/Await
```stratos
async fn fetchData(url: string) string {
    val response = await http.get(url);
    return response.body;
}
```

### Concurrency (Go-style)
```stratos
import "std/concurrent";

val ch = Channel<int>();
go fn() { ch.send(42); };
val value = ch.receive();
```

### Defer
```stratos
fn processFile() {
    val f = open("data.txt");
    defer f.close();
    // f.close() called when function exits
}
```

### Destructuring
```stratos
val (x, y, z) = getTuple();
val {name, age} = getObject();
```

### Type Casting
```stratos
val x = 42 as float;
val s = x as string;
```

### Comments
```stratos
// Single line comment

/* Multi-line
   comment */

/// Documentation comment
/// @param x The input value
/// @returns The result
fn documented(x: int) int { return x; }
```

## C++ Code Conventions

- **Standard**: C++20
- **Naming**: PascalCase for classes/structs, camelCase for functions/variables, UPPER_SNAKE_CASE for constants
- **Indentation**: 4 spaces
- **Braces**: Opening brace on same line
- **Headers**: All in `src/include/stratos/`
- **Pointers**: Attach to type (`int* ptr`)

## Compilation Pipeline

```
Source (.st) → Lexer → Tokens → Parser → AST → SemanticAnalyzer → Annotated AST
  → IRGenerator → LLVM IR → Optimizer → Optimized IR → Output
```

For interpretation: AST → Interpreter (AST-walking)

## Testing

```bash
cd scripts && bash test-all.sh           # Run all example tests
bash test-all.sh -v                       # Verbose
bash test-all.sh --test hello-world       # Specific test
stratos check src/main.st                 # Quick syntax validation
```

## Key Architecture Notes

- **Interpreter**: AST-walking interpreter with garbage collector
- **GC**: Mark-and-sweep garbage collector (can be disabled with `--no-gc`)
- **FFI**: C/C++ foreign function interface via shared libraries
- **Native Registry**: Maps stdlib function names to C++ implementations via `std::any`
- **Type System**: Supports generics with type parameters, inference, and monomorphization
- **Async Runtime**: Promise-based with event loop, supports goroutines and channels
- **Package Manager**: Built-in project creation, dependency resolution, and building
- **DevTools**: Chrome-style debugging with network monitor, profiler, and memory inspector

## Important Paths for Development

| Task | Files to modify |
|------|----------------|
| Add keyword/token | `src/include/stratos/Token.h`, `src/src/lexer/Lexer.cpp` |
| Add syntax | `src/include/stratos/AST.h`, `src/src/parser/Parser.cpp` |
| Add runtime behavior | `src/src/runtime/Interpreter.cpp` |
| Add native function | `src/src/runtime/NativeRegistry.cpp` |
| Add std module | Create `std/<module>/init.st`, register in NativeRegistry |
| Add type feature | `src/include/stratos/TypeSystem.h`, `src/src/runtime/TypeSystem.cpp` |
| Modify GC | `src/include/stratos/GarbageCollector.h`, `src/src/runtime/GarbageCollector.cpp` |
| Add CLI command | `src/src/main.cpp` |
| Update formatter | `src/src/formatter/Formatter.cpp` |
| Add example | Create `examples/<name>/stratos.conf` + `examples/<name>/src/main.st` |
