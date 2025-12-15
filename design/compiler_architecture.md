# Compiler Architecture & Design

## 1. Lexer / Parser Generator
**Decision:** Hand-written Recursive Descent Parser.
**Reasoning:**
*   **No Dependencies:** Simplifies build on Windows (no Flex/Bison requirements).
*   **Control:** Easier to handle complex logic (like significant whitespace if we ever needed it, or complex error recovery).
*   **Performance:** A well-written C++ lexer/parser is incredibly fast.
*   **Education:** Better for understanding the language internals.

## 2. Token Types
The `Token` class will hold a `TokenType`, a `lexeme` (string), and `line`/`column` info.

### Token Categories
*   **Keywords:** `FN`, `VAR`, `VAL`, `IF`, `ELSE`, `WHILE`, `CLASS`, `RETURN`, `TRUE`, `FALSE`, `NONE`, `OPTIONAL`, `INT`, `STRING`, ...
*   **Operators:** `PLUS` (+), `MINUS` (-), `STAR` (*), `SLASH` (/), `EQ` (==), `ASSIGN` (=), `PIPE` (|>), `ELVIS` (?: - *Wait, we removed Elvis for ternary, but kept it for null coalescing? Yes.*)
*   **Punctuation:** `LPAREN`, `RPAREN`, `LBRACE`, `RBRACE`, `COMMA`, `DOT`, `SEMICOLON`, `COLON`.
*   **Literals:** `IDENTIFIER`, `STRING_LIT`, `NUMBER_LIT`.
*   **Control:** `EOF_TOKEN`, `ERROR`.

## 3. AST Structure
We will use a polymorphic class hierarchy rooted at `ASTNode`.
Smart pointers (`std::unique_ptr`) will handle ownership.

### Core Hierarchy
*   `ASTNode` (Virtual destructor, `accept()` for visitors)
    *   `Statement`
        *   `VarDeclaration`
        *   `FunctionDeclaration`
        *   `Block`
        *   `IfStatement`
        *   `ExpressionStatement`
    *   `Expression`
        *   `BinaryExpr` (+, -, *, /)
        *   `UnaryExpr` (!, -)
        *   `LiteralExpr` (Number, String, Boolean)
        *   `VariableExpr` (Accessing a var)
        *   `CallExpr`
        *   `PipeExpr`

## 4. Parsing Strategy
**Recursive Descent.**
*   `Parser` class holds a list of tokens (or a reference to the Lexer).
*   Methods map 1:1 to Grammar rules (e.g., `parseStatement()`, `parseExpression()`).
*   **Error Handling:** "Panic Mode" recovery (synchronize on semicolon or brace).

## 5. Build System
*   **CMake** targeting C++20.
*   Source structure: `src/lexer`, `src/parser`, `src/ast`.
