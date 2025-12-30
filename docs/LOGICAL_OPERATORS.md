# Logical Operators Implementation

## Overview

Stratos supports logical operators using both keyword syntax (like Kotlin) and symbolic syntax (like C/Java). The keyword syntax is recommended for better readability.

## Supported Operators

| Keyword | Symbol | Description | Example |
|---------|--------|-------------|---------|
| `and` | `&&` | Logical AND | `a and b` or `a && b` |
| `or` | `||` | Logical OR | `a or b` or `a \|\| b` |
| `not` | `!` | Logical NOT | `not a` or `!a` |

## Features

### Short-Circuit Evaluation

Both `and` and `or` operators use short-circuit evaluation:

- **AND**: If the left operand is `false`, the right operand is not evaluated
- **OR**: If the left operand is `true`, the right operand is not evaluated

```stratos
// expensiveFunction() is NOT called
val result1 = false and expensiveFunction();

// expensiveFunction() is NOT called
val result2 = true or expensiveFunction();
```

### Type Safety

Logical operators return boolean values and work with any truthy/falsy values:

- **Truthy**: `true`, non-zero numbers, non-empty strings
- **Falsy**: `false`, `0`, empty strings, `None`

## Examples

### Basic Usage

```stratos
val isAdult = true;
val hasLicense = false;

// Keyword syntax (recommended)
val canDrive = isAdult and hasLicense;  // false
val canVote = isAdult or hasLicense;    // true
val isMinor = not isAdult;              // false

// Symbolic syntax (also supported)
val canDrive2 = isAdult && hasLicense;  // false
val canVote2 = isAdult || hasLicense;   // true
val isMinor2 = !isAdult;                // false
```

### Complex Conditions

```stratos
val age = 25;
val hasLicense = true;
val hasInsurance = true;

// Multiple conditions
if (age >= 18 and hasLicense and hasInsurance) {
    println("Can drive legally");
}

// Mixed operators
if ((age < 18 or age > 65) and not hasInsurance) {
    println("High risk");
}
```

### Pattern Matching with Guards

```stratos
val age = 25;
val hasLicense = true;

when (age) {
    _ if age >= 18 and hasLicense -> {
        println("Can drive");
    }
    _ if age >= 18 -> {
        println("Can vote but not drive");
    }
    else -> {
        println("Minor");
    }
}
```

## Implementation Details

### Lexer

Both keyword and symbolic syntaxes are tokenized:

- Keywords: `and`, `or`, `not` → `TokenType::AND`, `TokenType::OR`, `TokenType::NOT`
- Symbols: `&&`, `||`, `!` → same token types

**File**: `src/interpreter/cpp/src/lexer/Lexer.cpp`

```cpp
// Keywords mapping
{"and", TokenType::AND},
{"or", TokenType::OR},
{"not", TokenType::NOT}

// Symbol tokenization
case '&':
    if (match('&')) addToken(TokenType::AND);
    break;
case '|':
    if (match('|')) addToken(TokenType::OR);
    break;
```

### Parser

Logical operators are parsed with proper precedence:

1. `or` (lowest precedence)
2. `and`
3. Equality (`==`, `!=`)
4. Comparison (`<`, `>`, `<=`, `>=`)

**File**: `src/interpreter/cpp/src/parser/Parser.cpp`

```cpp
std::unique_ptr<Expr> Parser::logicOr() {
    std::unique_ptr<Expr> expr = logicAnd();
    while (match({TokenType::OR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = logicAnd();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::logicAnd() {
    std::unique_ptr<Expr> expr = equality();
    while (match({TokenType::AND})) {
        Token op = previous();
        std::unique_ptr<Expr> right = equality();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}
```

### Interpreter

Short-circuit evaluation is implemented in the interpreter:

**File**: `src/interpreter/cpp/src/runtime/Interpreter.cpp`

```cpp
// Handle logical operators with short-circuit evaluation
if (expr.op.type == TokenType::AND) {
    expr.left->accept(*this);
    RuntimeValue left = lastValue;

    // Short-circuit: if left is false, don't evaluate right
    if (!isTruthy(left)) {
        lastValue = RuntimeValue(std::any(false), "bool");
        return;
    }

    expr.right->accept(*this);
    RuntimeValue right = lastValue;
    lastValue = RuntimeValue(std::any(isTruthy(right)), "bool");
    return;
}

if (expr.op.type == TokenType::OR) {
    expr.left->accept(*this);
    RuntimeValue left = lastValue;

    // Short-circuit: if left is true, don't evaluate right
    if (isTruthy(left)) {
        lastValue = RuntimeValue(std::any(true), "bool");
        return;
    }

    expr.right->accept(*this);
    RuntimeValue right = lastValue;
    lastValue = RuntimeValue(std::any(isTruthy(right)), "bool");
    return;
}
```

## Testing

Test file: `/examples/test-logical-ops.st`

Tests cover:
- ✅ Basic AND/OR/NOT operations
- ✅ Short-circuit evaluation
- ✅ Complex expressions
- ✅ Integration with comparisons
- ✅ Real-world use cases

Run tests:
```bash
cd examples
../src/interpreter/cpp/build/stratos run test-logical-ops.st
```

## Style Guide

### Recommended

```stratos
// ✅ Use keyword syntax
if (age >= 18 and hasLicense) { }
if (isWeekend or isHoliday) { }
val result = not isActive;

// ✅ Use parentheses for clarity with mixed operators
if ((age < 18 or age > 65) and not hasInsurance) { }
```

### Acceptable

```stratos
// ✔️ Symbolic syntax works but is less readable
if (age >= 18 && hasLicense) { }
if (isWeekend || isHoliday) { }
val result = !isActive;
```

## Migration Guide

If you have code using symbolic operators, no changes are needed. Both syntaxes work:

```stratos
// Old code (still works)
if (x > 0 && y > 0) { }

// New style (recommended)
if (x > 0 and y > 0) { }
```

## Related Documentation

- [Control Flow Guide](/docs/src/guide/control-flow.md)
- [Basics Guide](/docs/src/guide/basics.md)
- [WebSocket Examples](/examples/websocket/README.md)

## Version History

- **v0.2.0-dev** (2025-12-30): Added support for `and`, `or`, `not` keyword syntax with short-circuit evaluation
- **v0.1.0**: Initial release with `&&`, `||`, `!` symbolic syntax
