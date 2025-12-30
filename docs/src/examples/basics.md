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
// Type inference
val name = "Stratos";
val age = 25;
val pi = 3.14159;

// Explicit types
val country: string = "Kenya";
val year: int = 2024;
val temperature: double = 36.5;
```

### Mutable Variables (var)

```stratos
// Can be reassigned
var counter = 0;
counter = counter + 1;
counter++;  // Increment operator

var status = "pending";
status = "completed";
```

::: info
**Naming Convention**: Use `val` by default for immutable values. Only use `var` when you need to reassign the variable.
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
```

## Arithmetic Operations

Stratos supports standard arithmetic operators:

```stratos
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
```

## String Operations

### Concatenation

```stratos
val firstName = "John";
val lastName = "Doe";

// String concatenation with +
val fullName = firstName + " " + lastName;
print(fullName);  // "John Doe"

// With numbers
val age = 30;
val message = "Age: " + age;
print(message);  // "Age: 30"
```

### String Methods

```stratos
val text = "Hello, Stratos!";

// Common methods (using standard library)
use strings;

val upper = strings.toUpper(text);      // "HELLO, STRATOS!"
val lower = strings.toLower(text);      // "hello, stratos!"
val length = strings.length(text);      // 15
val contains = strings.contains(text, "Stratos");  // true
```

## Comparison Operators

```stratos
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
```

## Logical Operators

```stratos
val isAdult = true;
val hasLicense = false;

// AND operator
val canDrive = isAdult && hasLicense;  // false

// OR operator
val canVote = isAdult || hasLicense;   // true

// NOT operator
val isMinor = !isAdult;                // false

// Complex expressions
val age = 25;
val hasPermission = age >= 18 && (hasLicense || isAdult);
```

## Type Conversion

```stratos
// Implicit conversion (where safe)
val intValue = 42;
val doubleValue: double = intValue;  // int to double

// Explicit conversion
val pi = 3.14159;
val rounded: int = pi as int;        // 3

// String conversion
val number = 42;
val text = number.toString();        // "42"
```

## Arrays

Arrays hold multiple values of the same type:

```stratos
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
```

## Comments

```stratos
// Single-line comment

/*
   Multi-line comment
   Can span multiple lines
*/

val x = 10;  // Inline comment
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
}
```

### Output

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
```

## Best Practices

::: tip
**Use `val` by default**: Prefer immutable variables for better code clarity and fewer bugs. Only use `var` when you need mutability.
:::

::: tip
**Type inference**: Let the compiler infer types when possible. Use explicit types when it improves readability or when the type isn't obvious.
:::

::: warning
**Integer division**: Division between two integers performs integer division. Use doubles for precise decimal results.
:::

## Next Steps


