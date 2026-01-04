# Safe Operations & Error Handling Example

This example demonstrates defensive programming and safe error handling patterns in Stratos. By checking conditions before performing operations, we can prevent crashes and handle edge cases gracefully.

## Overview

The example shows how to write robust code that validates inputs and checks boundary conditions before attempting potentially unsafe operations.

## What This Example Demonstrates

1. **Safe Division**: Check for zero before dividing
2. **Array Bounds Checking**: Validate indices before array access
3. **Input Validation**: Validate data before processing
4. **Multi-condition Validation**: Handle edge cases with clear logic
5. **Defensive Programming**: Prevent errors rather than catching them

## Running the Example

```bash
# From the project root
../../src/interpreter/cpp/build/stratos run examples/expect/src/main.st

# Or using the Stratos CLI
stratos run examples/expect/src/main.st
```

## Expected Output

```
=== Stratos Safe Operations Example ===

1. Safe division:
✓ 10 / 2 = 5
✗ Cannot divide 10 by zero
  Error handled safely!

2. Safe array access:
✓ arr[2] = 30
✗ Index 10 out of bounds (array length: 5)
  Using boundary check prevented crash!

3. Safe string parsing:
✓ "42" is a valid number
✗ "not a number" is not a valid number
  Validation prevented invalid parse!

4. Input validation:
✓ Valid age: 25
  Category: Adult
✗ Age cannot be negative: -5
✗ Age seems unrealistic: 150

=== All examples completed ===
```

## Key Patterns

### Pattern 1: Check Before Operating
```stratos
fn safeDivide(a: int, b: int) void {
    if b == 0 {
        println("Error: Division by zero");
    } else {
        val result = a / b;
        println("Result: " + result);
    }
}
```

### Pattern 2: Bounds Checking
```stratos
fn safeGet(arr: Array<int>, index: int) void {
    if index < 0 || index >= arr.length() {
        println("Error: Index out of bounds");
    } else {
        val value = arr[index];
        println("Value: " + value);
    }
}
```

### Pattern 3: Input Validation
```stratos
fn validateAge(age: int) void {
    if age < 0 {
        println("Error: Age cannot be negative");
    } else if age > 120 {
        println("Error: Age unrealistic");
    } else {
        println("Valid age: " + age);
    }
}
```

### Pattern 4: Early Return on Error
```stratos
fn processData(data: string) void {
    if data.length() == 0 {
        println("Error: Empty data");
        return;  // Exit early
    }

    // Process data...
}
```

## Error Handling Philosophy

Stratos encourages **defensive programming**:

1. **Validate Early**: Check inputs at function entry
2. **Be Explicit**: Make error cases clear in code
3. **Fail Gracefully**: Handle errors without crashing
4. **Use Guards**: Check conditions before operations

## Comparison with Other Approaches

### Try-Catch (Java/JavaScript)
```java
try {
    int result = a / b;
} catch (ArithmeticException e) {
    System.out.println("Error: " + e);
}
```

### Error Returns (Go)
```go
if b == 0 {
    return 0, errors.New("division by zero")
}
result := a / b
return result, nil
```

### Guard Clauses (Stratos)
```stratos
if b == 0 {
    println("Error: division by zero");
    return;
}
val result = a / b;
```

## When to Use Each Pattern

| Pattern | Use When |
|---------|----------|
| **Guard Clauses** | Simple validation, early returns |
| **Bounds Checking** | Array/collection access |
| **Input Validation** | Processing user input |
| **Range Validation** | Checking numeric ranges |

## Benefits of This Approach

1. **No Hidden Control Flow**: No exceptions jumping around
2. **Explicit Error Handling**: Clear what happens on error
3. **Performance**: No exception overhead
4. **Predictable**: Easy to trace execution path
5. **Maintainable**: Error cases are visible

## Related Concepts

- **Defensive Programming**: Always validate assumptions
- **Fail-Fast**: Detect errors as early as possible
- **Guard Clauses**: Check conditions at function start
- **Boundary Conditions**: Handle edge cases explicitly

## Related Examples

- [Loops](../loops/) - Demonstrates safe iteration
- [Hello World](../hello-world/) - Basic Stratos syntax
- [Function Definition](../function-definition/) - Function patterns

## Learn More

- Check the standard library for more error handling patterns
- See other examples for practical applications
