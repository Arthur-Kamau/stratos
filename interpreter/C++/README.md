# Stratos C++ Compiler

## Overview
This directory contains the C++ implementation of the Stratos programming language compiler. It's designed to translate Stratos source code (`.st` files) into LLVM Intermediate Representation (`.ll` files), which can then be compiled into native executables using standard LLVM tools like `clang`.

## Architecture
The compiler pipeline is structured into several phases:

1.  **Lexical Analysis (Lexer):** (`src/lexer/`)
    *   Converts raw source code into a stream of tokens (e.g., keywords, identifiers, operators).
2.  **Syntactic Analysis (Parser):** (`src/parser/`)
    *   Takes the token stream and builds an Abstract Syntax Tree (AST), ensuring the code adheres to Stratos's grammar.
3.  **Semantic Analysis (Analyzer):** (`src/sema/`)
    *   Traverses the AST to perform checks like scope resolution (ensuring variables are defined), basic type validation (e.g., 'this' and 'super'), and redefinition checks. It uses a Symbol Table to manage scopes.
4.  **Code Generation (IRGenerator):** (`src/codegen/`)
    *   Walks the semantically validated AST and generates LLVM Intermediate Representation (IR) in textual `.ll` format. This phase handles basic types (int, double, bool, string), control flow (if, while), and function definitions.

## Goals (Achieved & Ongoing)
1.  **Move to LLVM infrastructure:** Achieved (generates LLVM IR).
2.  **Improve Stratos speed:** In progress (LLVM IR allows for significant optimization).
3.  **Ability to generate standalone executables:** Achieved (via LLVM IR).

## Supported Language Features
The compiler currently supports:
*   Variable declarations (`val`, `var`) with type annotations (`int`, `double`, `string`, `bool`, `Optional<T>`).
*   Arithmetic and comparison expressions.
*   Basic control flow (`if`/`else`, `while`).
*   Function declarations and calls.
*   Classes and Namespaces (basic parsing and semantic checks; full IR generation is in progress).
*   Literal values (numbers, strings, booleans, `None`).
*   Pipe operator (`|>`).

## Building the Compiler

### Prerequisites

Ensure you have the following installed:
- **C++ compiler** (g++ 9+ or clang 10+)
- **CMake** 3.15 or later
- **LLVM** 14 or later (optional, for LLVM IR generation)
- **Git**

### Quick Build

The easiest way to build:

```bash
cd interpreter/C++
bash build.sh
```

This will produce the `stratos` binary in the `build/` directory.

### Manual Build (Alternative)

If you prefer to build manually:

1.  **Navigate to the C++ directory:**
    ```bash
    cd interpreter/C++
    mkdir -p build
    cd build
    ```
2.  **Configure CMake:**
    ```bash
    cmake ..
    ```
3.  **Build the executable:**
    ```bash
    cmake --build .
    ```

This will produce the `stratos` binary in the `build/` directory.

## Usage

### Basic Usage

The `stratos` binary supports both compilation and interpretation modes:

```bash
# Run a Stratos program directly (interpreter mode)
./build/stratos run <path_to_file.st>

# Compile a Stratos program (compiler mode)
./build/stratos compile <path_to_file.st>

# Show version information
./build/stratos --version
```

### Making the Binary Globally Available (For Developers)

During development, you can make `stratos` available globally so you don't need to type the full path:

```bash
# Create symlink (run from project root)
mkdir -p ~/bin
ln -sf "$(pwd)/interpreter/C++/build/stratos" ~/bin/stratos

# Add to PATH (add this to ~/.bashrc or ~/.zshrc)
export PATH="$HOME/bin:$PATH"

# Reload shell
source ~/.bashrc  # or ~/.zshrc

# Now you can use 'stratos' from anywhere:
stratos --version
stratos run samples/hello_world/src/main.st
```

**Benefits**: Every time you rebuild, the global `stratos` command automatically uses the new binary.

### Running Tests

From the project root:

```bash
# Run all tests
../../test-all.sh

# Run with verbose output
../../test-all.sh -v

# Run specific test
../../test-all.sh --test hello_world
```

## Further Development
*   Expanding type checking and inference.
*   Implementing advanced control flow (`when` expression).
*   Full IR generation for classes, inheritance, and complex types.
*   Adding more sophisticated optimization passes.