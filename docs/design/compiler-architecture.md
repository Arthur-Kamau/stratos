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
*   **Keywords:** `FN`, `VAR`, `VAL`, `IF`, `ELSE`, `WHILE`, `CLASS`, `RETURN`, `TRUE`, `FALSE`, `NONE`, `OPTIONAL`, `INT`, `STRING`, `DOUBLE`, `BOOL`, `VOID`, `UNIT`, `ASYNC`, `AWAIT`, `BREAK`, `CONTINUE`, `FOR`, `WHEN`, `STRUCT`, `INTERFACE`, `ENUM`, `PACKAGE`, `USE`, `AS`, `CONSTRUCTOR`, `PUB`, `NOT`, `TYPE`, `I8`, `I16`, `I32`, `I64`, `U8`, `U16`, `U32`, `U64`, `F32`, `F64`, `USIZE`, `ISIZE`
*   **Operators:** `PLUS` (+), `MINUS` (-), `STAR` (*), `SLASH` (/), `EQ` (==), `ASSIGN` (=), `PIPE` (|>), `ELVIS` (?:), `BANG_EQUAL` (!=), `LESS_EQUAL` (<=), `GREATER_EQUAL` (>=), `ARROW` (-> or =>), `PLUS_EQUAL` (+=), `MINUS_EQUAL` (-=), `STAR_EQUAL` (*=), `SLASH_EQUAL` (/=), `LEFT_SHIFT` (<<), `RIGHT_SHIFT` (>>), `BITWISE_OR` (|), `BITWISE_AND` (&), `BITWISE_XOR` (^), `BITWISE_NOT` (~), `QUESTION_DOT` (?.)
*   **Punctuation:** `LPAREN`, `RPAREN`, `LBRACE`, `RBRACE`, `COMMA`, `DOT`, `SEMICOLON`, `COLON`, `LEFT_BRACKET`, `RIGHT_BRACKET`, `DOT_DOT`, `DOT_DOT_DOT`
*   **Literals:** `IDENTIFIER`, `STRING_LIT`, `NUMBER_LIT`, `CHAR_LIT`
*   **Control:** `EOF_TOKEN`, `ERROR`, `DOC_COMMENT`

## 3. AST Structure
We will use a polymorphic class hierarchy rooted at `ASTNode`.
Smart pointers (`std::unique_ptr`) will handle ownership.

### Core Hierarchy
*   `ASTNode` (Virtual destructor, `accept()` for visitors)
    *   `Statement`
        *   `VarDeclaration` (val/var with type annotations)
        *   `FunctionDeclaration` (fn, async fn, methods)
        *   `ClassDeclaration`
        *   `StructDeclaration`
        *   `InterfaceDeclaration`
        *   `EnumDeclaration`
        *   `TypeAliasDeclaration`
        *   `PackageDeclaration`
        *   `UseStmt` (module imports)
        *   `Block`
        *   `IfStatement`
        *   `WhileStatement`
        *   `ForStatement`
        *   `ReturnStatement`
        *   `BreakStatement`
        *   `ContinueStatement`
        *   `ExpressionStatement`
        *   `PrintStatement`
    *   `Expression`
        *   `BinaryExpr` (+, -, *, /, ==, !=, <, >, <=, >=, &&, ||, etc.)
        *   `UnaryExpr` (!, -, not)
        *   `LiteralExpr` (Number, String, Boolean, Char, None)
        *   `VariableExpr` (Accessing a var)
        *   `CallExpr` (function/method calls)
        *   `PipeExpr` (|> operator)
        *   `CastExpr` (as, as? operators)
        *   `AwaitExpr` (await keyword)
        *   `MapLiteralExpr` (map creation)
        *   `ArrayLiteralExpr` (array creation)
        *   `LambdaExpr` (arrow functions)
        *   `StructInitExpr` (struct initialization)
        *   `WhenExpr` (when expressions)
        *   `IndexExpr` (array indexing)
        *   `GroupingExpr` (parentheses)

## 4. Parsing Strategy
**Recursive Descent.**
*   `Parser` class holds a list of tokens (or a reference to the Lexer).
*   Methods map 1:1 to Grammar rules (e.g., `parseStatement()`, `parseExpression()`).
*   **Error Handling:** "Panic Mode" recovery (synchronize on semicolon or brace).
*   **Documentation Comments:** Collects and attaches `/** */` comments to declarations
*   **Advanced Features:**
    - Generic type parameter parsing (<T, E>)
    - Lambda/arrow function parsing (=>)
    - When expression parsing (when(x) { ... })
    - Range expression parsing (0..10)
    - Optional type parsing (Optional<T>)

## 5. Memory Management

### Current Implementation (GC-Based)
*   **Mark-and-sweep garbage collector** for automatic memory management
*   **Cycle detection** for circular references
*   **Reference counting** for objects

### Future Plans (GC-Free Mode)
*   **dfer<T>**: Unique ownership smart pointer
*   **ref<T>**: Shared ownership smart pointer (reference counted)
*   **weak<T>**: Weak references (non-owning)
*   **stack<T>**: Stack-allocated objects
*   **new** and **delete** keywords for manual management
*   **Destructors**: `~ClassName()` syntax
*   **--no-gc flag**: GC-free mode for performance-critical code

## 6. Build System
*   **CMake** targeting C++20.
*   Source structure: `src/lexer`, `src/parser`, `src/ast`.
