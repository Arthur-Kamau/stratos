---
title: Basics
description: Variables, types, and basic operations in Stratos
---

# Basic Syntax and Operations

Learn the fundamentals of Stratos programming including variables, types, and basic operations.

## Variable Declarations

Stratos supports both mutable and immutable variables with type inference.

### Immutable Variables (val)

```stratos
package main;

fn main() {
    // Type inference
    val name = "Stratos";
    val age = 25;
    val pi = 3.14159;

    // Explicit types
    val country: string = "Kenya";
    val year: int = 2024;
    val temperature: double = 36.5;
}
```

### Mutable Variables (var)

```stratos
package main;

fn main() {
    // Can be reassigned
    var counter = 0;
    counter = counter + 1;
    counter++;  // Increment operator

    var status = "pending";
    status = "completed";
}
```

::: info Naming Convention
Use `val` by default for immutable values. Only use `var` when you need to reassign the variable. Immutable values lead to fewer bugs and more predictable code.
:::

## Primitive Types

Stratos has several built-in primitive types:

| Type | Description | Example |
|------|-------------|---------|
| `int` | Integer numbers | `42`, `-10`, `0` |
| `double` | Floating-point numbers | `3.14`, `-0.5`, `2.0` |
| `string` | Text values | `"Hello"`, `'World'` |
| `bool` | Boolean values | `true`, `false` |
| `char` | Single character | `'a'`, `'Z'` |

### Examples

```stratos
package main;

fn main() {
    // Integer operations
    val count: int = 100;
    val negative: int = -42;
    val zero: int = 0;

    // Floating-point operations
    val price: double = 19.99;
    val ratio: double = 0.75;
    val scientific: double = 1.5e10;

    // String operations
    val greeting: string = "Hello";
    val message: string = 'Welcome to Stratos';
    val empty: string = "";

    // Boolean operations
    val isActive: bool = true;
    val hasAccess: bool = false;

    // Character
    val initial: char = 'S';
    val newline: char = '\n';
}
```

::: tip Type Inference
Stratos has powerful type inference. You don't always need to specify types explicitly - the compiler can figure them out from context.
:::

## Arithmetic Operations

Stratos supports standard arithmetic operators:

```stratos
package main;

fn main() {
    val a = 10;
    val b = 3;

    // Basic operations
    val sum = a + b;         // 13
    val difference = a - b;  // 7
    val product = a * b;     // 30
    val quotient = a / b;    // 3 (integer division)
    val remainder = a % b;   // 1 (modulo)

    // With doubles
    val x = 10.0;
    val y = 3.0;
    val result = x / y;      // 3.333...

    // Compound assignment
    var count = 10;
    count += 5;   // count = 15
    count -= 3;   // count = 12
    count *= 2;   // count = 24
    count /= 4;   // count = 6

    // Increment/Decrement
    count++;      // count = 7
    count--;      // count = 6
    ++count;      // count = 7
    --count;      // count = 6
}
```

::: warning Integer Division
Division between two integers performs integer division (truncates the decimal part). Use doubles for precise decimal results.
:::

## String Operations

### Concatenation

```stratos
package main;

fn main() {
    val firstName = "John";
    val lastName = "Doe";

    // String concatenation with +
    val fullName = firstName + " " + lastName;
    print(fullName);  // "John Doe"

    // With numbers
    val age = 30;
    val message = "Age: " + age;
    print(message);  // "Age: 30"
}
```

### String Methods

```stratos
package main;

use strings;

fn main() {
    val text = "Hello, Stratos!";

    // Common methods (using standard library)
    val upper = strings.toUpper(text);                  // "HELLO, STRATOS!"
    val lower = strings.toLower(text);                  // "hello, stratos!"
    val length = strings.length(text);                  // 15
    val contains = strings.contains(text, "Stratos");   // true
}
```

## Comparison Operators

```stratos
package main;

fn main() {
    val x = 10;
    val y = 20;

    // Equality
    val equal = x == y;          // false
    val notEqual = x != y;       // true

    // Relational
    val lessThan = x < y;        // true
    val lessOrEqual = x <= y;    // true
    val greaterThan = x > y;     // false
    val greaterOrEqual = x >= y; // false

    // With strings
    val str1 = "apple";
    val str2 = "banana";
    val result = str1 == str2;   // false
}
```

## Logical Operators

Stratos supports logical operators using both keyword and symbolic syntax:

```stratos
package main;

fn main() {
    val isAdult = true;
    val hasLicense = false;

    // AND operator (keyword syntax - recommended)
    val canDrive = isAdult and hasLicense;  // false

    // OR operator (keyword syntax - recommended)
    val canVote = isAdult or hasLicense;   // true

    // NOT operator
    val isMinor = not isAdult;              // false

    // Alternative symbolic syntax (also supported)
    val canDrive2 = isAdult && hasLicense;  // false (equivalent to 'and')
    val canVote2 = isAdult || hasLicense;   // true (equivalent to 'or')
    val isMinor2 = !isAdult;                // false (equivalent to 'not')

    // Complex expressions with keyword syntax
    val age = 25;
    val hasPermission = age >= 18 and (hasLicense or isAdult);

    // Short-circuit evaluation
    // 'and' stops if left side is false
    // 'or' stops if left side is true
    val result = false and expensiveCheck();  // expensiveCheck() not called
    val result2 = true or expensiveCheck();   // expensiveCheck() not called
}
```

::: tip Keyword Syntax Recommended
Use `and`, `or`, and `not` for better readability. The `&&`, `||`, and `!` operators are supported for compatibility but keywords are more readable and easier to understand.
:::

## Type Conversion

Stratos provides two operators for converting between types: `as` (forced cast) and `as?` (safe cast).

### Forced Cast (`as`)

Use `as` when you are sure the conversion will succeed. If the cast is invalid at runtime, the program will terminate with an error.

```stratos
package main;

fn main() {
    // Numeric conversions
    val pi = 3.14;
    val integerPi = pi as int;       // 3 (truncates)

    val count = 10;
    val doubleCount = count as double; // 10.0

    // String conversions
    val strNum = "123";
    val num = strNum as int;         // 123

    // Boolean conversions
    val boolVal = true;
    val intVal = boolVal as int;     // 1
}
```

### Safe Cast (`as?`)

Use `as?` when the conversion might fail (e.g., parsing user input). If the cast fails, it returns `void` instead of crashing the program.

```stratos
package main;

fn main() {
    val valid = "123" as? int;       // Returns 123
    val invalid = "abc" as? int;     // Returns void

    // Check results before using
    if (valid != void) {
        print("Valid number: " + valid);
    }

    if (invalid == void) {
        print("Invalid number format");
    }
}
```

### Supported Conversions

| Source | Target | Behavior |
|--------|--------|----------|
| `int` | `double` | Promotion (1 -> 1.0) |
| `double` | `int` | Truncation (3.14 -> 3) |
| `string` | `int` | Parses integer, throws/returns void on failure |
| `string` | `double` | Parses float, throws/returns void on failure |
| `bool` | `int` | true -> 1, false -> 0 |
| `int` | `bool` | 0 -> false, non-zero -> true |
| `any` | `string` | String representation |

::: tip Safe vs Forced Cast
Use `as?` when parsing user input or dealing with data that might be invalid. Use `as` only when you're certain the conversion will succeed.
:::

::: warning Explicit Casting Required
Stratos requires explicit casting for most type conversions. This prevents accidental data loss and makes type conversions visible in your code.
:::

## Arrays

Arrays hold multiple values of the same type:

```stratos
package main;

fn main() {
    // Array literals
    val numbers = [1, 2, 3, 4, 5];
    val names = ["Alice", "Bob", "Charlie"];
    val mixed = [1, 2.5, 3];  // Type inferred as Array<double>

    // Explicit type
    val scores: Array<int> = [95, 87, 92, 100];

    // Access elements
    val first = numbers[0];   // 1
    val last = numbers[4];    // 5

    // Modify elements (if array is mutable)
    var mutableArray = [10, 20, 30];
    mutableArray[0] = 15;     // [15, 20, 30]

    // Array length
    val count = numbers.length;  // 5

    // Iterate over array
    for (num in numbers) {
        print(num);
    }

    // Iterate with index
    for (i, num in numbers) {
        print("Index " + i + ": " + num);
    }
}
```

::: info Array Indexing
Arrays in Stratos are zero-indexed, meaning the first element is at index 0. Accessing an index outside the array bounds will cause a runtime error.
:::

## Comments

```stratos
package main;

fn main() {
    // Single-line comment

    /*
       Multi-line comment
       Can span multiple lines
    */

    val x = 10;  // Inline comment

    /*
     * Documentation-style comment
     * Often used for function documentation
     */
}
```

## Complete Example

Here's a complete program demonstrating basic concepts:

```stratos
package main;

fn main() {
    // Variables
    val name = "Stratos";
    val version = 1.0;
    var counter = 0;

    // Arithmetic
    val a = 10;
    val b = 3;
    val sum = a + b;
    val product = a * b;

    // Output
    print("Language: " + name);
    print("Version: " + version);
    print("Sum: " + sum);
    print("Product: " + product);

    // Loop
    while (counter < 5) {
        print("Count: " + counter);
        counter++;
    }

    // Array
    val numbers = [1, 2, 3, 4, 5];
    var total = 0;

    for (num in numbers) {
        total = total + num;
    }

    print("Total: " + total);

    // Logical operators
    val isPositive = sum > 0;
    val isEven = sum % 2 == 0;
    val result = isPositive and not isEven;
    print("Positive and odd: " + result);
}
```

**Output:**
```
Language: Stratos
Version: 1.0
Sum: 13
Product: 30
Count: 0
Count: 1
Count: 2
Count: 3
Count: 4
Total: 15
Positive and odd: true
```

## Type System Quick Reference

| Category | Types | Description |
|----------|-------|-------------|
| Numeric | `int`, `double` | Whole and decimal numbers |
| Text | `string`, `char` | Text and single characters |
| Boolean | `bool` | True/false values |
| Collections | `Array<T>` | Ordered collection of elements |
| Special | `void` | Absence of value (similar to null) |

## Operator Precedence

Operators are evaluated in the following order (highest to lowest):

1. **Unary operators**: `++`, `--`, `!`, `not`, `-` (negation)
2. **Multiplicative**: `*`, `/`, `%`
3. **Additive**: `+`, `-`
4. **Relational**: `<`, `<=`, `>`, `>=`
5. **Equality**: `==`, `!=`
6. **Logical AND**: `and`, `&&`
7. **Logical OR**: `or`, `||`
8. **Assignment**: `=`, `+=`, `-=`, `*=`, `/=`

Use parentheses to override precedence:
```stratos
val result1 = 2 + 3 * 4;      // 14 (multiplication first)
val result2 = (2 + 3) * 4;    // 20 (addition first)
```

## Best Practices

::: tip Use val by Default
Prefer immutable variables (`val`) for better code clarity and fewer bugs. Only use `var` when you need mutability. Immutable code is easier to reason about and less prone to errors.
:::

::: tip Type Inference
Let the compiler infer types when possible. Use explicit types when it improves readability or when the type isn't obvious from context.
:::

::: tip Keyword Logical Operators
Use `and`, `or`, and `not` instead of `&&`, `||`, and `!` for better readability. The keywords make your code more self-documenting.
:::

::: warning Integer Division
Division between two integers performs integer division (truncates decimals). If you need decimal precision, convert at least one operand to `double`.
:::

::: warning Explicit Casting
Always use explicit casting (`as` or `as?`) when converting between types. This makes type conversions visible and prevents accidental data loss.
:::

::: tip Safe Casting
When parsing user input or handling potentially invalid data, use `as?` (safe cast) instead of `as` to avoid runtime crashes.
:::

## Next Steps

- [Functions](/guide/functions) - Learn about function definitions and lambdas
- [Control Flow](/guide/control-flow) - Conditionals and loops
- [Collections](/guide/collections) - Working with arrays, maps, and sets
- [Object-Oriented Programming](/guide/oop) - Classes and interfaces