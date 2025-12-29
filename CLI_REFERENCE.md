# Stratos CLI Reference

Complete reference for the Stratos command-line interface.

## Table of Contents

- [Overview](#overview)
- [Commands](#commands)
  - [compile](#compile)
  - [run](#run)
  - [check](#check)
  - [fmt](#fmt)
  - [build](#build)
  - [new](#new)
  - [get](#get)
  - [test](#test)
- [Global Options](#global-options)
- [Examples](#examples)

## Overview

The `stratos` command-line tool provides everything you need to build, run, and manage Stratos projects.

```bash
stratos [command] [options] [arguments]
```

## Commands

### compile

Compile Stratos source files to LLVM IR.

**Usage:**
```bash
stratos compile <file.st> [options]
stratos compile <directory> [options]
stratos <file.st> [options]              # Shorthand
```

**Options:**
- `-o, --output <file>` - Specify output file path
- `-v, --verbose` - Enable verbose output
- `-r, --run` - Execute after compiling

**Examples:**
```bash
# Compile single file
stratos compile hello.st

# Compile with custom output
stratos compile hello.st -o bin/hello.ll

# Compile and run
stratos compile hello.st --run

# Compile all files in directory
stratos compile src/
```

**Output:**
```
Compiling: hello.st
  [Lexer]     OK (45 tokens)
  [Parser]    OK (3 statements)
  [Semantics] OK
  [Optimizer] Finished
  [CodeGen]   Generated hello.st.ll
```

### run

Execute a Stratos program directly without explicit compilation step.

**Usage:**
```bash
stratos run <file.st> [options]
stratos run <directory> [options]  # Uses stratos.conf
stratos run [options]              # Uses stratos.conf in current dir
```

**Options:**
- `-v, --verbose` - Show detailed execution information

**Examples:**
```bash
# Run a simple program
stratos run hello.st

# Run project directory (automatically detects stratos.conf)
stratos run samples/oop_demo/

# Run from inside project directory
cd samples/oop_demo
stratos run  # Uses stratos.conf entry point

# Run with verbose output
stratos run -v server.st

# Run with arguments
stratos run calculator.st 10 20
```

**How it works:**
1. Lexical analysis
2. Parsing
3. Semantic analysis
4. Direct execution via interpreter (no code generation)

**Project Directory Mode:**
When you run `stratos run` with a directory path or without arguments (in a directory with `stratos.conf`):
1. Detects `stratos.conf` and reads the entry point
2. **Changes working directory** to the project directory
3. Runs the entry point file
4. Restores the original directory

This ensures proper module resolution for multi-file projects:
```bash
# Both work identically:
cd /anywhere && stratos run /path/to/my-project/
cd /path/to/my-project && stratos run
```

**Note:** The `run` command uses the interpreter for immediate execution, while `compile` generates LLVM IR for optimized native code.

### check

Parse and analyze code without code generation. Fast syntax and semantic validation.

**Usage:**
```bash
stratos check <file.st> [options]
stratos check <directory> [options]
```

**Options:**
- `-v, --verbose` - Show detailed checking information

**Examples:**
```bash
# Check single file
stratos check src/main.st

# Check with verbose output
stratos check src/main.st -v

# Check all .st files in directory
stratos check src/

# Check entire project
stratos check .
```

**Single File Output:**
```bash
$ stratos check src/main.st
✓ src/main.st is valid
```

**Verbose Single File Output:**
```bash
$ stratos check src/main.st -v
Checking: src/main.st
  ✓ Lexer OK (87 tokens)
  ✓ Parser OK (5 statements)
  ✓ Semantics OK
✓ src/main.st is valid
```

**Directory Output:**
```bash
$ stratos check src/
Checking all .st files in: src/
✓ src/main.st
✓ src/utils.st
✓ src/model.st

========================================
Check Summary
========================================
Total files:    3
Passed:         3
Failed:         0

All files passed! ✓
```

**Use Cases:**
- **Quick Validation**: Fast syntax checking during development
- **CI/CD Integration**: Verify code quality before building
- **IDE Integration**: Provide real-time error checking
- **Pre-commit Hooks**: Validate code before committing
- **Large Codebase Validation**: Check entire project structure

**What it checks:**
- ✅ Lexical correctness (valid tokens)
- ✅ Syntactic correctness (valid grammar)
- ✅ Semantic correctness (type checking, scope resolution)
- ❌ Does NOT generate code
- ❌ Does NOT execute code
- ❌ Does NOT check external dependencies

**Performance:**
```
Compilation:  ~500ms  (lexer + parser + semantics + optimizer + codegen)
Check:        ~100ms  (lexer + parser + semantics only)
```

The `check` command is approximately 5x faster than full compilation, making it ideal for quick validation.

### fmt

Format Stratos source files according to standard style guidelines.

**Usage:**
```bash
stratos fmt <file.st> [options]
stratos fmt <directory> [options]
```

**Options:**
- `-w, --write` - Write formatted code back to file (in-place formatting)
- `--check` - Check if files are formatted (exit 1 if not)
- `-v, --verbose` - Show detailed formatting information

**Examples:**
```bash
# Print formatted code to stdout
stratos fmt src/main.st

# Format file in-place
stratos fmt src/main.st -w

# Check if file is formatted
stratos fmt src/main.st --check

# Format all .st files in directory
stratos fmt src/ -w

# Check formatting for entire project
stratos fmt . --check
```

**Default Output (stdout):**
```bash
$ stratos fmt src/main.st
package main;

fn hello() {
    println("hello world");
}

fn main() {
    hello();
}
```

**Write Mode Output:**
```bash
$ stratos fmt src/main.st -w
✓ Formatted: src/main.st

Formatted 1 file(s)
```

**Check Mode Output (formatted):**
```bash
$ stratos fmt src/ --check

========================================
Format Check Summary
========================================
Files checked:       3
Already formatted:   3
Need formatting:     0

All files are formatted! ✓
```

**Check Mode Output (needs formatting):**
```bash
$ stratos fmt src/ --check
✗ src/utils.st is not formatted
✗ src/model.st is not formatted

========================================
Format Check Summary
========================================
Files checked:       3
Already formatted:   1
Need formatting:     2

Some files are not formatted.
Run: stratos fmt <file> -w to format them
```

**Style Guidelines:**
- 4 spaces for indentation (no tabs)
- Opening braces on same line
- Space after keywords (if, while, for, fn, class)
- No space before opening parenthesis in function calls
- Space around operators (=, +, -, *, /, ==, !=, etc.)
- Blank line between top-level declarations

**Use Cases:**
- **Code Formatting**: Standardize code style across team
- **CI/CD Integration**: Enforce formatting in pull requests
- **Pre-commit Hooks**: Auto-format before committing
- **Editor Integration**: Format on save

**Important Notes:**
- Comments are not preserved during formatting as they are not part of the AST
- **The formatter requires syntactically valid code** - files with syntax errors may cause crashes
- Always run `stratos check <file>` before formatting to ensure code is valid

**Example workflow:**
```bash
# 1. Check for syntax errors first
stratos check src/main.st

# 2. If check passes, format the code
stratos fmt src/main.st -w
```

### build

Build a project using configuration from `stratos.conf`.

**Usage:**
```bash
stratos build [project_dir] [options]
```

**Options:**
- `-v, --verbose` - Show detailed build information

**Examples:**
```bash
# Build current project
stratos build

# Build specific project
stratos build my-app

# Build with verbose output
stratos build -v
```

**Project Structure:**
```
my-project/
├── stratos.conf       # Project configuration
├── src/
│   └── main.st        # Entry point
├── deps/              # Dependencies
└── build/             # Output directory
```

**Output:**
```
Building project: my-app v1.0.0
Compiling 3 file(s)...
✓ Build successful in 245ms
Output: ./build/my-app.ll
```

### new

Create a new Stratos project with boilerplate structure.

**Usage:**
```bash
stratos new <project-name>
```

**Examples:**
```bash
# Create new project
stratos new my-app

# Then build and run
cd my-app
stratos build
./build/my-app
```

**Generated Structure:**
```
my-app/
├── stratos.conf       # Project configuration
├── src/
│   └── main.st        # Sample main file
├── build/             # Build output
├── libs/              # Libraries
├── tests/             # Tests
├── README.md          # Documentation
└── .gitignore         # Git ignore rules
```

**Sample main.st:**
```stratos
package main;

use log;
use math;

fn main() {
    log.info("Hello from my-app!");

    val result = math.sqrt(16.0);
    log.info("Square root of 16 is: " + result);
}
```

### get

Fetch dependencies from `stratos.conf` or download specific dependency.

**Usage:**
```bash
stratos get                    # Fetch all dependencies
stratos get <url>              # Fetch specific dependency
stratos get --update           # Update all dependencies
stratos get --verify           # Verify lock file
```

**Dependency Formats:**
```
github.com/user/repo@v1.0.0    # GitHub with version tag
github.com/user/repo@main      # GitHub with branch
github.com/user/repo@abc123    # GitHub with commit hash
https://github.com/user/repo   # Full GitHub URL
path:../local-lib              # Local directory
```

**Examples:**
```bash
# Fetch all dependencies from stratos.conf
stratos get

# Fetch specific dependency
stratos get github.com/user/my-lib@v1.0.0

# Update all to latest versions
stratos get --update

# Verify dependencies match lock file
stratos get --verify
```

**Output:**
```
Fetching 2 dependencies...
✓ Successfully fetched: my-lib
✓ Successfully fetched: utils

=== Summary ===
Successfully fetched: 2
```

### test

Run test cases from the `cases/` directory.

**Usage:**
```bash
stratos test [options]
```

**Options:**
- `--verbose` - Show detailed test output

**Examples:**
```bash
# Run all tests
stratos test

# Run with verbose output
stratos test --verbose
```

**Output:**
```
Running 5 test cases...
✓ test_variables.st
✓ test_functions.st
✓ test_classes.st
✓ test_loops.st
✓ test_conditionals.st

All tests passed! (5/5)
```

## Global Options

These options work with most commands:

| Option | Description |
|--------|-------------|
| `-v, --verbose` | Enable verbose output |
| `-o, --output <file>` | Specify output file (compile only) |
| `-r, --run` | Execute after compiling (compile only) |
| `--help` | Show help information |
| `--version` | Show version information |

## Examples

### Development Workflow

```bash
# 1. Create new project
stratos new my-app
cd my-app

# 2. Quick check during development
stratos check src/main.st

# 3. Check entire project
stratos check src/

# 4. Run for testing
stratos run src/main.st

# 5. Build for production
stratos build

# 6. Compile specific file
stratos compile src/utils.st -o build/utils.ll
```

### CI/CD Pipeline

```bash
#!/bin/bash
# ci-build.sh

# Fetch dependencies
stratos get

# Verify lock file
stratos get --verify || exit 1

# Quick validation (fast)
stratos check . || exit 1

# Run tests
stratos test || exit 1

# Full build
stratos build || exit 1

echo "✓ Build successful!"
```

### Pre-commit Hook

```bash
#!/bin/bash
# .git/hooks/pre-commit

# Quick check all staged .st files
for file in $(git diff --cached --name-only --diff-filter=ACM | grep '\.st$'); do
    echo "Checking $file..."
    stratos check "$file" || {
        echo "✗ Check failed for $file"
        exit 1
    }
done

echo "✓ All checks passed"
```

### IDE Integration

For language servers or IDE plugins:

```bash
# Check file on save (fast feedback)
stratos check src/main.st

# Get detailed errors (verbose mode)
stratos check src/main.st -v

# Check entire workspace
stratos check workspace/
```

### Large Codebase Validation

```bash
# Check all source files
stratos check src/ --verbose

# Filter results
stratos check src/ 2>&1 | grep "✗"  # Show only errors

# Count valid files
stratos check src/ 2>&1 | grep -c "✓"
```

## Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | Error (compilation, runtime, or validation failure) |

## Performance Tips

1. **Use `check` for quick validation** - 5x faster than full compilation
2. **Use `run` for testing** - No compilation step, immediate execution
3. **Use `compile` for production** - Generates optimized LLVM IR
4. **Use `--no-rebuild` in test scripts** - Skip unnecessary rebuilds

## Comparison: check vs run vs compile

| Feature | `check` | `run` | `compile` |
|---------|---------|-------|-----------|
| Lexical analysis | ✅ | ✅ | ✅ |
| Parsing | ✅ | ✅ | ✅ |
| Semantic analysis | ✅ | ✅ | ✅ |
| Optimization | ❌ | ❌ | ✅ |
| Code generation | ❌ | ❌ | ✅ |
| Execution | ❌ | ✅ | ❌ |
| Speed | ~100ms | ~200ms | ~500ms |
| Use case | Validation | Testing | Production |

## Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `STRATOS_CACHE` | Dependency cache directory | `~/.stratos/cache` |
| `STRATOS_INSTALL_DIR` | Installation directory | `~/.stratos` |

## Getting Help

```bash
# Show general help
stratos --help

# Show version
stratos --version
```

For more information, visit the [documentation](docs/).
