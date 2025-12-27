# Stratos
A fast, type-safe, compiled(Interpreted), and fun-to-write programming language.


## Objective

*   Fast
*   Low-level (with managed abstractions where needed)
*   Mix of functional and OOP paradigms
*   Flexible (write scripts, huge monolith projects)
*   Compiled/Interpretes=d language
*   Ability to generate static binaries.

## Status

<img src="https://img.shields.io/badge/Status-Compiler%20Functional-blue?style=for-the-badge&logo=appveyor">
<br>
<p float="left">
 <img src="./icons/work.jpeg" height="25">
 <img src="./icons/work.jpeg" height="25">
 <img src="./icons/work.jpeg" height="25">
</p>

The **Stratos C++ Compiler** now features Lexing, Parsing, Semantic Analysis, and LLVM IR Code Generation. A VS Code extension provides basic language tooling.

## Plan

-   Language design and documentations ✅ (See `design/` and project documentation)
-   Tooling ✅ (VS Code extension with basic LSP features)
-   Lexer & Parser (Analysis) ✅ (C++ implementation complete)
-   Semantic Analysis & Type Checking ✅ (C++ implementation complete)
-   Stratos IR (Code Generation to LLVM IR) ✅ (C++ implementation complete)
-   Stratos WorkMan Tool. (Future)
-   Printing the frontend to CPP. (Future)

## Getting Started

### Compiler
To build and use the Stratos compiler:
1.  Navigate to `compiler/C++`.
2.  Follow the instructions in `compiler/C++/README.md`.

### VS Code Extension
To get language support in VS Code:
1.  Navigate to `tooling/vscode`.
2.  Follow the instructions in `tooling/vscode/README.md`.

## Building

To install the compiler, build tools, and dependencies, check the installing section in the documentation.

## License
Stratos is distributed under the terms of the Apache License (Version 2.0) and MIT license depending on the submodule. See `license` folder for details.

## Roadmap
Check the GitHub projects dashboard.
Some design notes are available in the `design` folder.

## Contributing
Please be sure to review Stratos contributing guidelines to learn how to help the project. (Check Documentation)

## Icon
<p align="left">
<img height="45" src="./icons/images.jpg"/>
</p>