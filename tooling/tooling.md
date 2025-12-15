## Tooling

Tooling contains various development assets for the Stratos programming language.

1.  **Compiler Plugins:** (Future)
    *   Integration with build systems and other tools to enhance the compilation process.

2.  **Language Server for IDE / Code Editor:** (In Progress)
    *   A foundational Language Server has been implemented for VS Code, providing essential language features.

3.  **Source Code Analysis Tool:** (Long-term Goal)
    *   Similar to tools like Dart Analyzer or PVS-Studio, for deeper code quality and bug detection.

4.  **Toolman (Build Tool):** (Future)
    *   The dedicated build tool for the Stratos programming language.

5.  **VS Code Plugin:** (Actively Developed)
    *   Provides rich language support for Stratos in Visual Studio Code.
    *   **Features include:**
        *   Syntax Highlighting (Keywords, Types, Operators, Strings, Comments).
        *   Basic Code Completion (for keywords like `val`, `var`, `fn`, `class`, `Optional`).
        *   Error Checking (integrates with the C++ compiler to display parse and semantic errors as VS Code diagnostics).
    *   **For details on setup, features, and usage, refer to:**
        *   [README.md](vscode/README.md) (`tooling/vscode/README.md`)

6.  **IntelliJ Idea Language Plugin:** (Future)
    *   Planned language support for IntelliJ IDEA.

*   For the VS Code Stratos Language Plugin, the `README.md` file and `documentation.md` (if it exists) provide more details on the current version, supported features, and planned enhancements.