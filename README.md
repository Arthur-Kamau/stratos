# Stratos

A fast, type-safe, compiled (currently interpreted), and fun-to-write programming language.

## Objective

* Fast
* Low-level (with managed abstractions where needed)
* Mix of functional and OOP paradigms
* Flexible (write scripts, huge monolith projects)
* Compiled/Interpreted language (supports both modes)
* Ability to generate static binaries

## Status

<img src="https://img.shields.io/badge/Status-Compiler%20Functional-blue?style=for-the-badge&logo=appveyor">
<br>
<p float="left">
 <img src="./icons/work.jpeg" height="25">
 <img src="./icons/work.jpeg" height="25">
 <img src="./icons/work.jpeg" height="25">
</p>

The **Stratos C++ Interpreter** is the primary implementation, featuring a handwritten recursive descent parser and AST-walking interpreter. It supports a growing Standard Library including File I/O, JSON, HTTP, SQLite/PostgreSQL/MySQL/Redis database support, regex, crypto, and more.

The **Stratos Package Manager** is integrated, allowing project creation (`stratos new`), building (`stratos build`), dependency management (`stratos deps`), and code formatting (`stratos fmt`).

**Tooling** includes a VS Code extension, IntelliJ plugin, Chrome-style DevTools (debugger, profiler, network monitor, memory inspector), an interactive web tutorial, and a documentation site.

## Quick Example

```stratos
package main;

import "std/strings";
import "std/math";

struct Point {
    x: float;
    y: float;
}

fn distance(a: Point, b: Point) float {
    val dx = a.x - b.x;
    val dy = a.y - b.y;
    return math.sqrt(dx * dx + dy * dy);
}

fn main() {
    val p1 = Point { x: 0.0, y: 0.0 };
    val p2 = Point { x: 3.0, y: 4.0 };
    println("Distance: ${distance(p1, p2)}");

    val names = ["Alice", "Bob", "Charlie"];
    for name in names {
        println("Hello, ${strings.toUpper(name)}!");
    }
}
```

## Getting Started

### Prerequisites

- C++20 compiler (GCC 10+, Clang 12+)
- CMake 3.20+
- System libraries: `libssl-dev libpq-dev libmysqlclient-dev libhiredis-dev`

### Build

```bash
git clone https://github.com/Arthur-Kamau/stratos.git
cd stratos/src
bash build.sh
```

This produces `src/build/stratos` (the compiler/interpreter binary) and `src/build/libstratos_runtime.a` (static runtime library).

### Make the Binary Globally Available

```bash
mkdir -p ~/bin
ln -sf "$(pwd)/src/build/stratos" ~/bin/stratos
export PATH="$HOME/bin:$PATH"  # Add to ~/.bashrc to make permanent
```

### Create and Run a Project

```bash
stratos new hello-world
cd hello-world
stratos run .
```

### Run Examples

```bash
stratos run ./examples/hello-world/
stratos run ./examples/oop-classes/
stratos run ./examples/async-demo/
```

## CLI Commands

| Command | Description |
|---------|-------------|
| `stratos run <file\|dir>` | Run a Stratos program |
| `stratos build <dir>` | Build a project |
| `stratos new <name>` | Create a new project |
| `stratos check <file\|dir>` | Syntax/semantic validation (no codegen) |
| `stratos fmt <file\|dir> [-w]` | Format code (`-w` writes in-place) |
| `stratos test` | Run tests |
| `stratos doc <file>` | Generate documentation |
| `stratos compile <dir>` | Compile to binary |
| `stratos deps` | Manage dependencies |

## Project Structure

```
stratos/
├── src/                        # C++ interpreter/compiler source
│   ├── include/stratos/        # Header files
│   ├── src/                    # Implementation (lexer, parser, runtime, etc.)
│   ├── build.sh                # Linux build script
│   └── CMakeLists.txt          # CMake configuration
├── std/                        # Standard library (23 modules)
├── examples/                   # 70+ example projects
├── external/                   # External packages (cli-args, colors, linq, valid)
├── docs/                       # Documentation site
├── stratos-tutorial/           # Interactive tutorial (SvelteKit + Monaco)
├── devtools/                   # Chrome-style DevTools UI
├── tooling/                    # IDE extensions (VS Code, IntelliJ)
├── tests/                      # Test infrastructure
└── scripts/                    # Install and test scripts
```

## Standard Library

| Module | Description |
|--------|-------------|
| `std/math` | Trigonometry, logarithms, statistics, random |
| `std/strings` | Case conversion, searching, splitting, replacing |
| `std/io` | File I/O, reading, writing |
| `std/fs` | File system operations, directories |
| `std/log` | Structured logging (debug, info, warn, error) |
| `std/time` | Time, duration, formatting |
| `std/collections` | List, Map, Set, Queue, Stack |
| `std/async` | Promises, async/await |
| `std/concurrent` | Goroutines, channels, mutex, waitgroup |
| `std/net` | TCP/UDP, HTTP server/client |
| `std/db` | SQLite, PostgreSQL, MySQL, Redis |
| `std/encoding` | JSON, base64, CSV, XML, YAML |
| `std/crypto` | Hashing, encryption |
| `std/regex` | Regular expressions |
| `std/os` | Environment, process, exec |
| `std/terminal` | Colors, cursor control, input |
| `std/testing` | Unit testing framework |
| `std/ffi` | Foreign function interface (C/C++) |
| `std/websocket` | WebSocket client/server |
| `std/template` | Template engine |
| `std/convert` | Type conversions |
| `std/zip` | Archive compression |
| `std/greeting` | Greeting utilities |

## Important Disclaimer

**Solo Project with AI Assistance**: Stratos is a solo project developed with heavy use of AI assistance (Claude Code/Gemini) throughout the language design, compiler implementation, standard library, and tooling. While this has enabled rapid development and exploration of language features, please be aware:

- This is an **experimental project** under active development
- The codebase may contain bugs, incomplete features, or unconventional design patterns
- **Use at your own risk** - not recommended for production use
- Testing coverage is limited and evolving
- Breaking changes may occur frequently as the language design evolves

If you encounter issues or have suggestions, please open an issue on GitHub. Contributions and feedback are welcome!

## VS Code Extension

For syntax highlighting and language support in VS Code:
1. Navigate to `tooling/vscode`
2. Follow the instructions in `tooling/vscode/README.md`

## Contributing

We welcome contributions! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for:

- Development setup and building from source
- Making the `stratos` binary globally available during development
- Running tests and using git hooks
- Code style guidelines and PR submission process

All code should follow the [Stratos Style Guide](STYLE_GUIDE.md). Use `stratos fmt . -w` to format your code before submitting PRs.

## License

Stratos is distributed under the terms of the Apache License (Version 2.0) and MIT license depending on the submodule. See `license` folder for details.

## Roadmap

Check the GitHub projects dashboard.
Design notes are available in `docs/design/`.

## Icon
<p align="left">
<img height="45" src="./icons/images.jpg"/>
</p>
