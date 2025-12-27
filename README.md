# Stratos
A fast, type-safe, compiled(Interpreted), and fun-to-write programming language.


## Objective

*   Fast
*   Low-level (with managed abstractions where needed)
*   Mix of functional and OOP paradigms
*   Flexible (write scripts, huge monolith projects)
*   Compiled/Interpreted language (supports both modes)
*   Ability to generate static binaries.

## Status

<img src="https://img.shields.io/badge/Status-Compiler%20Functional-blue?style=for-the-badge&logo=appveyor">
<br>
<p float="left">
 <img src="./icons/work.jpeg" height="25">
 <img src="./icons/work.jpeg" height="25">
 <img src="./icons/work.jpeg" height="25">
</p>

The **Stratos C++ Compiler** now features Lexing, Parsing, Semantic Analysis, and LLVM IR Code Generation. The **Stratos Interpreter** provides direct AST execution for rapid development and testing. A VS Code extension provides basic language tooling.

**Two execution modes available**:
- `stratos compile <file.st>` - Compile to LLVM IR for optimization and production builds
- `stratos run <file.st>` - Direct interpretation for immediate execution and output

## ⚠️ Important Disclaimer

**Solo Project with AI Assistance**: Stratos is a solo project developed with heavy use of AI assistance (Claude Code/Gemini 3) throughout the language design, compiler implementation, standard library, and tooling. While this has enabled rapid development and exploration of language features, please be aware:

- This is an **experimental project** under active development
- The codebase may contain bugs, incomplete features, or unconventional design patterns
- **Use at your own risk** - not recommended for production use
- Testing coverage is limited and evolving
- Breaking changes may occur frequently as the language design evolves

If you encounter issues or have suggestions, please open an issue on GitHub. Contributions and feedback are welcome!



## Getting Started

### Compiler & Interpreter
To build and use the Stratos compiler and interpreter:
1.  Navigate to `interpreter/C++`.
2.  Follow the instructions in `interpreter/C++/README.md`.

The build produces a single `stratos` binary that supports both compilation and interpretation modes.

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