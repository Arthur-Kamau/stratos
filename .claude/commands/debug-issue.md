# Debug a Stratos Issue

Diagnose and fix issues in Stratos programs or the compiler itself.

## Arguments
- $ARGUMENTS: Description of the issue (e.g., "parser crashes on nested when", "GC not collecting", error message text)

## Instructions

### For Stratos Program Issues

1. If a `.st` file path is mentioned, read the file
2. Try running it: `./src/build/stratos run <file-or-dir>`
3. Analyze the error output
4. Common Stratos program issues:
   - **Syntax errors**: Check against language syntax in CLAUDE.md
   - **Import not found**: Verify the import path matches a std/ module or local file
   - **Type mismatch**: Check variable types and function signatures
   - **Undefined variable/function**: Check scope and spelling
   - **Runtime errors**: Check array bounds, null access, division by zero

### For Compiler/Interpreter Issues

1. Determine which stage the error occurs in:
   - **Lexer**: Token-level errors (unexpected characters, unterminated strings)
   - **Parser**: Syntax-level errors (unexpected token, missing brace)
   - **Semantic Analyzer**: Type errors, undefined references
   - **Interpreter**: Runtime panics, infinite loops, crashes

2. Read the relevant source file(s) based on the stage:
   - Lexer issues: `src/src/lexer/Lexer.cpp`
   - Parser issues: `src/src/parser/Parser.cpp`, `src/include/stratos/AST.h`
   - Semantic issues: `src/src/sema/SemanticAnalyzer.cpp`
   - Runtime issues: `src/src/runtime/Interpreter.cpp`
   - GC issues: `src/src/runtime/GarbageCollector.cpp`
   - Native function issues: `src/src/runtime/NativeRegistry.cpp`

3. Search for the error message in the C++ source to find where it's thrown

4. Propose a fix with specific file and line changes

5. After fixing, rebuild with `/build` and test the fix

### Debugging Tips
- Run with `-v` for verbose output: `./src/build/stratos run <path> -v`
- Use `stratos check <file>` for fast syntax validation
- Check if the issue reproduces with a minimal example
- Look at similar working examples in `examples/` for correct syntax
