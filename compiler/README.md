# Stratos Compiler Collection

This directory contains the source code for the Stratos programming language compiler.

## Structure

*   **`C++/`**: The main reference implementation of the Stratos compiler.
    *   **Status:** Functional prototype (Lexer, Parser, Semantic Analysis, LLVM IR Code Generation).
    *   **Build:** Uses CMake.
    *   **Output:** Generates LLVM IR (`.ll`) which can be compiled with `clang`.
    *   **See:** `C++/README.md` for detailed build and usage instructions.

*   **`cases/`**: A collection of Stratos source code examples (`.st` files) used for testing the compiler's functionality (control flow, OOP, functions, etc.).

## Quick Start

To build the compiler:

1.  Navigate to `C++/`.
2.  Follow the build steps in the `C++/README.md`.
