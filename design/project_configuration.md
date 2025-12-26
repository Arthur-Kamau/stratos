# Project Configuration & Multi-File Compilation

## Overview

Stratos now supports project-based compilation with configuration files, allowing you to organize code across multiple files with dependency management.

## Configuration File Format

Projects are configured using `stratos.conf` files in INI-style format.

### Basic Structure

```ini
[project]
name = project_name
version = 1.0.0
author = Your Name
type = executable  # or "library"

[build]
entry = src/main.st          # Entry point for executables
source_dir = src             # Source directory
output = build/output        # Output file path (without .ll extension)

[dependencies]
math_lib = ../math_lib       # dependency_name = relative_path

[compile]
sources = file1.st, file2.st, file3.st  # Explicit source file list (comma-separated)

[exports]
# For libraries - define public API
main = src/main_module.st
```

### Section Details

#### `[project]` Section

- **name** (required): Project identifier
- **version** (required): Semantic version
- **author** (optional): Project author
- **type** (optional): `executable` (default) or `library`

#### `[build]` Section

- **entry** (optional): Main entry point file for executables
- **source_dir** (optional): Source code directory
- **output** (optional): Output file path (defaults to `build/<name>`)

#### `[dependencies]` Section

Lists project dependencies as key-value pairs:
```ini
[dependencies]
lib_name = ../path/to/lib
another_lib = ../../shared/another_lib
```

*Note: Dependency resolution not yet fully implemented*

#### `[compile]` Section

- **sources**: Comma-separated list of source files in compilation order

This is the recommended way to control file ordering:

```ini
[compile]
sources = src/base.st, src/derived.st, src/main.st
```

#### `[exports]` Section

For library projects, defines public API exports:

```ini
[exports]
math = src/math.st
utils = src/utils.st
```

## Building Projects

### Command Syntax

```bash
# Build current directory (looks for stratos.conf)
stratos build

# Build specific project directory
stratos build path/to/project

# Build with verbose output
stratos build -v
stratos build path/to/project --verbose
```

### Build Process

1. **Locate Configuration**: Find `stratos.conf` in project directory
2. **Parse Config**: Extract project metadata and build settings
3. **Gather Sources**: Collect source files based on config
4. **Compile**: Process all files and merge AST
5. **Generate**: Create single LLVM IR output file

### Example: Hello World Project

**Directory Structure:**
```
hello_world/
├── stratos.conf
└── src/
    └── main.st
```

**stratos.conf:**
```ini
[project]
name = hello_world
version = 1.0.0
author = Stratos Developer

[build]
entry = src/main.st
output = build/hello_world
```

**src/main.st:**
```stratos
package main;

fn main() {
    print("Hello, Stratos!");
}
```

**Build:**
```bash
$ stratos build hello_world
Building project: hello_world v1.0.0
Compiling 1 file(s)...
✓ Build successful in 2ms
Output: hello_world/build/hello_world.ll
```

## Multi-File Compilation

### Compilation Order

Files are compiled in the order specified in `[compile].sources`:

```ini
[compile]
sources = src/interfaces.st, src/classes.st, src/main.st
```

**Important**: Classes must be defined before they are used!

### Example: OOP Demo Project

**Directory Structure:**
```
oop_demo/
├── stratos.conf
└── src/
    ├── models/
    │   ├── shape.st      # Interface
    │   └── rectangle.st   # Implementation
    └── main.st           # Entry point
```

**stratos.conf:**
```ini
[project]
name = oop_demo
version = 1.0.0

[build]
source_dir = src
output = build/oop_demo

[compile]
sources = src/models/shape.st, src/models/rectangle.st, src/main.st
```

**src/models/shape.st:**
```stratos
package models;

interface Shape {
    fn area() double;
    fn perimeter() double;
}
```

**src/models/rectangle.st:**
```stratos
package models;

class Rectangle : Shape {
    var width: double;
    var height: double;

    constructor(w: double, h: double) {
        this.width = w;
        this.height = h;
    }

    fn area() double {
        return this.width * this.height;
    }

    fn perimeter() double {
        return 2.0 * (this.width + this.height);
    }
}
```

**src/main.st:**
```stratos
package main;

fn main() {
    val rect = Rectangle(10.0, 5.0);
    print(rect.area());
    print(rect.perimeter());
}
```

**Build:**
```bash
$ stratos build oop_demo -v
Building project: oop_demo v1.0.0
Project root: oop_demo
Project type: executable
Compiling 3 file(s)...
  Processing: oop_demo/src/models/shape.st
  Processing: oop_demo/src/models/rectangle.st
  Processing: oop_demo/src/main.st
✓ Build successful in 3ms
Output: oop_demo/build/oop_demo.ll
```

## Implementation Details

### AST Merging

All source files are parsed independently, then their ASTs are merged:

```cpp
std::vector<std::unique_ptr<Stmt>> allStatements;

for (const auto& file : sourceFiles) {
    // Parse each file
    Lexer lexer(source);
    Parser parser(tokens);
    auto statements = parser.parse();

    // Merge into combined AST
    for (auto& stmt : statements) {
        allStatements.push_back(std::move(stmt));
    }
}

// Analyze and generate from merged AST
analyzer.analyze(allStatements);
generator.generate(allStatements);
```

### Class Resolution

Classes from all files are collected during IR generation:

1. **First Pass**: Scan all files for class declarations
2. **Metadata Collection**: Build class info table
3. **Struct Generation**: Create LLVM struct types
4. **Code Generation**: Generate constructors and methods

This allows classes to be used across files.

### Package Declarations

Package declarations don't create separate namespaces (yet):

```stratos
package models;  // Declares file belongs to 'models' package

class Rectangle { }  // Rectangle is globally accessible
```

*Note: True package namespacing is not yet implemented*

## Sample Projects

The `samples/` directory contains example projects:

### 1. hello_world
Simple single-file executable

### 2. math_lib
Library project with math functions

### 3. oop_demo
Multi-file OOP demonstration with interfaces and classes

## Command Reference

```bash
# Single file compilation (legacy)
stratos file.st
stratos compile file.st

# Multi-file compilation
stratos compile directory/  # Compiles all .st files

# Project build
stratos build               # Build current directory
stratos build path/         # Build specific project

# Testing
stratos test               # Run test suite
stratos test -v            # Run with verbose output

# Help
stratos --help
stratos --version
```

## Best Practices

### 1. Organize by Feature

```
my_project/
├── stratos.conf
└── src/
    ├── models/      # Data structures
    ├── services/    # Business logic
    └── main.st      # Entry point
```

### 2. Explicit Source Ordering

Always specify `sources` in config to control compilation order:

```ini
[compile]
sources = src/interfaces.st, src/implementations.st, src/main.st
```

### 3. One Class Per File

Keep classes in separate files for better organization:

```
src/models/
├── shape.st       # interface Shape
├── rectangle.st   # class Rectangle
└── circle.st      # class Circle
```

### 4. Consistent Package Names

Use package names that match directory structure:

```
src/models/shape.st:
    package models;

src/services/math.st:
    package services;
```

## Future Enhancements

1. **Automatic Dependency Resolution**
   - Topological sorting of source files
   - Import statement analysis

2. **True Package Namespacing**
   - Qualified names: `models.Rectangle`
   - Import/use statements
   - Public/private visibility

3. **Dependency Management**
   - Download external dependencies
   - Version constraints
   - Lock files

4. **Build Caching**
   - Incremental compilation
   - Only recompile changed files
   - Build artifact caching

5. **Library Support**
   - Generate static/dynamic libraries
   - Header file generation
   - API documentation

6. **Test Integration**
   - Test discovery
   - Test execution
   - Coverage reports
