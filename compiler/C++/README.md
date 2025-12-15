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

To build the `stratos.exe` compiler, ensure you have CMake and a C++20 compatible compiler (e.g., MinGW-w64 GCC, MSVC) installed.

1.  **Navigate to the build directory:**
    ```bash
    cd compiler/C++
    mkdir build
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
    This will produce `stratos.exe` in the `build` directory.

## Usage

You can run the `stratos.exe` on Stratos source files (`.st`) to generate LLVM IR.

### Basic Compilation
To compile a single Stratos source file:
```bash
./stratos.exe <path_to_your_file.st>
```
This will generate `path_to_your_file.st.ll` in the same directory.

### Running Test Cases
The compiler comes with a set of test cases in the `cases/` directory. You can run the compiler on all of them:
```bash
./stratos.exe ../../cases
```
This command will process each `.st` file in the `cases/` directory, perform Lexing, Parsing, Semantic Analysis, and Code Generation, printing the status for each.

## Further Development
*   Expanding type checking and inference.
*   Implementing advanced control flow (`when` expression).
*   Full IR generation for classes, inheritance, and complex types.
*   Adding more sophisticated optimization passes.