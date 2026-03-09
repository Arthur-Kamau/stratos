# Add a Language Feature

Guide for implementing a new language feature in the Stratos compiler/interpreter.

## Arguments
- $ARGUMENTS: Description of the feature to add (e.g., "optional chaining operator", "generic constraints", "pattern matching")

## Instructions

Implementing a new language feature requires changes across multiple compiler stages. Follow this pipeline:

### Step 1: Token Definition
**File:** `src/include/stratos/Token.h`
- Add new token types to the `TokenType` enum if needed
- Add keyword mappings if the feature introduces new keywords

### Step 2: Lexer
**File:** `src/src/lexer/Lexer.cpp`
- Add lexing rules for new operators or keywords
- Handle new character sequences

### Step 3: AST Node
**File:** `src/include/stratos/AST.h`
- Define new AST node types (structs/classes) for the feature
- Add to the AST visitor pattern if used
- Follow existing patterns: nodes inherit from `ASTNode` or `Expression`/`Statement`

### Step 4: Parser
**File:** `src/src/parser/Parser.cpp`
- Add parsing rules for the new syntax
- Create the new AST nodes during parsing
- Handle precedence for operators
- Read existing parse methods for reference patterns

### Step 5: Semantic Analysis
**File:** `src/src/sema/SemanticAnalyzer.cpp`
- Add type checking rules for the new feature
- Validate semantic correctness
- Report meaningful error messages

### Step 6: Interpreter
**File:** `src/src/runtime/Interpreter.cpp`
- Add interpretation logic for the new AST nodes
- Handle runtime evaluation

### Step 7: (Optional) IR Code Generation
**File:** `src/src/codegen/IRGenerator.cpp`
- Add LLVM IR generation for compiled mode

### Step 8: (Optional) Formatter
**File:** `src/src/formatter/Formatter.cpp`
- Add formatting rules for the new syntax

### Step 9: Testing
- Create an example project in `examples/` demonstrating the feature
- Create `examples/<feature-name>/stratos.conf` and `examples/<feature-name>/src/main.st`
- Run the example to verify: `./src/build/stratos run ./examples/<feature-name>/`

### Step 10: Documentation
- Update relevant docs in `docs/`
- Add to CLAUDE.md if it's a significant syntax addition

### Workflow

1. Read $ARGUMENTS to understand what feature is requested
2. Read existing similar features in the codebase for patterns
3. Plan which files need changes
4. Implement changes in pipeline order (Token → Lexer → AST → Parser → Sema → Interpreter)
5. Create a test example
6. Rebuild with `/build`
7. Test with `/run-example`
