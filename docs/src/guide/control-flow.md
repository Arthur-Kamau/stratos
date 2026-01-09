---
title: Control Flow
description: If statements, loops, and pattern matching in Stratos
---

# Control Flow

Learn how to control program flow in Stratos using conditionals, loops, and pattern matching.

## If Statements

### Basic If-Else

```stratos
package main;

fn main() {
    val age = 20;

    if (age >= 18) {
        print("Adult");
    } else {
        print("Minor");
    }
}
```

### If-Else-If Chains

```stratos
package main;

fn main() {
    val age = 20;

    if (age > 60) {
        print("Senior");
    } else if (age > 18) {
        print("Adult");
    } else {
        print("Child");
    }
}
```

### If as Expression (Ternary)

In Stratos, `if` is an expression that returns a value:

```stratos
package main;

fn main() {
    val age = 20;
    val status = if (age >= 18) "Adult" else "Minor";
    print(status);  // "Adult"

    // More complex example
    val grade = 85;
    val result = if (grade >= 90) {
        "Excellent"
    } else if (grade >= 75) {
        "Good"
    } else if (grade >= 60) {
        "Pass"
    } else {
        "Fail"
    };
    print(result);  // "Good"
}
```

::: tip If as Expression
In Stratos, `if` is an expression that returns a value. This allows you to assign the result directly to a variable, reducing the need for mutable state.
:::

## While Loops

### Basic While Loop

```stratos
package main;

fn main() {
    var i = 0;
    while (i < 10) {
        print(i);
        i++;
    }
}
// Prints: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
```

### While with Break

```stratos
package main;

fn main() {
    var count = 0;
    while (true) {
        print(count);
        count++;

        if (count >= 5) {
            break;  // Exit the loop
        }
    }
}
// Prints: 0, 1, 2, 3, 4
```

### While with Continue

```stratos
package main;

fn main() {
    var i = 0;
    while (i < 10) {
        i++;

        if (i % 2 == 0) {
            continue;  // Skip even numbers
        }

        print(i);
    }
}
// Prints: 1, 3, 5, 7, 9
```

::: tip Loop Control
Use `break` to exit a loop early and `continue` to skip to the next iteration. These are especially useful with infinite loops or when processing collections.
:::

## For Loops

### Iterating Over Arrays

```stratos
package main;

fn main() {
    val fruits = ["apple", "banana", "orange"];

    for (fruit in fruits) {
        print(fruit);
    }
}
// Output:
// apple
// banana
// orange
```

### Iterating with Index

```stratos
package main;

fn main() {
    val numbers = [10, 20, 30, 40, 50];

    for (i, num in numbers) {
        print("Index " + i + ": " + num);
    }
}
// Output:
// Index 0: 10
// Index 1: 20
// Index 2: 30
// Index 3: 40
// Index 4: 50
```

### Range-Based Loops

```stratos
package main;

fn main() {
    // Loop from 0 to 4 (exclusive end)
    for (i in 0..5) {
        print(i);
    }
    // Prints: 0, 1, 2, 3, 4

    // Loop from 1 to 10 (inclusive end)
    for (i in 1..=10) {
        print(i);
    }
    // Prints: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
}
```

::: info Range Syntax
- `a..b` creates a range from `a` to `b-1` (exclusive end)
- `a..=b` creates a range from `a` to `b` (inclusive end)
:::

## Pattern Matching (When)

Stratos provides powerful pattern matching with the `when` expression.

### Basic When Expression

```stratos
package main;

fn main() {
    val userType = "admin";

    when (userType) {
        "admin" -> {
            print("Full Access");
        }
        "editor" -> {
            print("Edit Access");
        }
        "viewer" -> {
            print("Read Only");
        }
        else -> {
            print("No Access");
        }
    }
}
// Output: Full Access
```

### When as Expression

```stratos
package main;

fn main() {
    val userType = "editor";

    val accessLevel = when (userType) {
        "admin" -> "Full Access"
        "editor" -> "Edit Access"
        "viewer" -> "Read Only"
        else -> "No Access"
    };

    print(accessLevel);  // "Edit Access"
}
```

### Pattern Matching with Numbers

```stratos
package main;

fn main() {
    val score = 85;

    val grade = when (score) {
        90..100 -> "A"
        80..89 -> "B"
        70..79 -> "C"
        60..69 -> "D"
        else -> "F"
    };

    print("Grade: " + grade);  // "Grade: B"
}
```

### Multiple Conditions

```stratos
package main;

fn main() {
    val day = "Monday";

    when (day) {
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday" -> {
            print("Weekday");
        }
        "Saturday", "Sunday" -> {
            print("Weekend");
        }
        else -> {
            print("Invalid day");
        }
    }
}
// Output: Weekday
```

### Pattern Matching with Guards

Guards allow you to add conditions to patterns:

```stratos
package main;

fn main() {
    val age = 25;
    val hasLicense = true;

    when (age) {
        _ if age >= 18 and hasLicense -> {
            print("Can drive");
        }
        _ if age >= 18 -> {
            print("Can vote but not drive");
        }
        else -> {
            print("Minor");
        }
    }
}
// Output: Can drive
```

::: info Logical Operators
Use `and`, `or`, and `not` keywords for better readability. The `&&`, `||`, and `!` symbols are also supported for compatibility.
:::

## Complete Example: Number Classifier

Here's a complete program demonstrating various control flow constructs:

```stratos
package main;

fn classifyNumber(num: int) string {
    return when {
        num < 0 -> "Negative"
        num == 0 -> "Zero"
        num > 0 and num <= 10 -> "Small Positive"
        num > 10 and num <= 100 -> "Medium Positive"
        else -> "Large Positive"
    };
}

fn isPrime(n: int) bool {
    if (n <= 1) return false;
    if (n <= 3) return true;

    var i = 2;
    while (i * i <= n) {
        if (n % i == 0) {
            return false;
        }
        i++;
    }

    return true;
}

fn main() {
    // Test classification
    val numbers = [-5, 0, 7, 42, 150];

    for (num in numbers) {
        val classification = classifyNumber(num);
        print(num + " is " + classification);
    }

    // Check for primes
    print("\nPrime numbers from 1 to 20:");
    for (i in 1..=20) {
        if (isPrime(i)) {
            print(i);
        }
    }

    // User type access control
    val users = ["admin", "editor", "viewer", "guest"];

    for (user in users) {
        val access = when (user) {
            "admin" -> "full"
            "editor" -> "write"
            "viewer" -> "read"
            else -> "none"
        };

        print(user + " has " + access + " access");
    }
}
```

**Output:**
```
-5 is Negative
0 is Zero
7 is Small Positive
42 is Medium Positive
150 is Large Positive

Prime numbers from 1 to 20:
2
3
5
7
11
13
17
19

admin has full access
editor has write access
viewer has read access
guest has none access
```

## FizzBuzz Example

A classic programming exercise demonstrating control flow:

```stratos
package main;

fn main() {
    for (i in 1..=100) {
        val output = when {
            i % 15 == 0 -> "FizzBuzz"
            i % 3 == 0 -> "Fizz"
            i % 5 == 0 -> "Buzz"
            else -> i.toString()
        };

        print(output);
    }
}
```

## Pattern Matching with Option Types

When expressions work seamlessly with Option types:

```stratos
package main;

fn divide(a: int, b: int) Option<int> {
    if (b == 0) {
        return None;
    }
    return Some(a / b);
}

fn main() {
    val result = divide(10, 2);

    when (result) {
        Some(value) -> print("Result: " + value)
        None -> print("Cannot divide by zero")
    }
}
```

## Pattern Matching with Result Types

Handle errors elegantly with Result pattern matching:

```stratos
package main;

fn parseAge(input: string) Result<int, Error> {
    val age = input.toInt();
    
    if (age < 0) {
        return Err(Error("Age cannot be negative"));
    }
    if (age > 150) {
        return Err(Error("Age seems unrealistic"));
    }
    
    return Ok(age);
}

fn main() {
    val inputs = ["25", "-5", "200", "abc"];

    for (input in inputs) {
        val result = parseAge(input);

        match (result) {
            Ok(age) -> print(input + " is valid: " + age + " years")
            Err(error) -> print(input + " is invalid: " + error.message)
        }
    }
}
```

::: tip Match vs When
Both `match` and `when` can be used for pattern matching. Use `match` when working with Result and Option types for clarity, and `when` for general pattern matching.
:::

## Best Practices

::: tip Use When for Multiple Conditions
Pattern matching with `when` is more readable than long if-else-if chains, especially when dealing with multiple cases.
:::

::: tip Prefer Expressions Over Statements
Use `if` and `when` as expressions to reduce mutable variables and make code more functional. This leads to fewer bugs and more predictable code.
:::

::: tip Exhaustive Pattern Matching
Always handle all cases in pattern matching. Use the `else` branch when you can't enumerate all possibilities.
:::

::: warning Avoid Infinite Loops
Always ensure your `while` loops have a proper exit condition or use `break` to exit. Infinite loops can hang your program.
:::

::: info Exhaustive Matching
The `else` branch in `when` expressions is required unless all possible cases are covered. This prevents runtime errors from unhandled cases.
:::

::: tip Use Guards for Complex Conditions
When patterns need additional conditions, use guards (`_ if condition`) to keep your pattern matching clean and readable.
:::

## Control Flow Quick Reference

| Construct | Purpose | Example |
|-----------|---------|---------|
| `if-else` | Conditional execution | `if (x > 0) { ... } else { ... }` |
| `if` expression | Conditional value | `val result = if (x > 0) "pos" else "neg"` |
| `while` | Loop while condition is true | `while (i < 10) { i++; }` |
| `for-in` | Iterate over collection | `for (x in array) { ... }` |
| `for-in` (indexed) | Iterate with index | `for (i, x in array) { ... }` |
| `range` (exclusive) | Numeric range | `for (i in 0..5) { ... }` |
| `range` (inclusive) | Numeric range | `for (i in 1..=10) { ... }` |
| `when` | Pattern matching | `when (x) { 1 -> ..., else -> ... }` |
| `when` expression | Pattern matching value | `val result = when (x) { ... }` |
| `when` with guards | Conditional patterns | `_ if condition -> ...` |
| `match` | Pattern matching (Result/Option) | `match (result) { Ok(v) -> ..., Err(e) -> ... }` |
| `break` | Exit loop early | `break;` |
| `continue` | Skip to next iteration | `continue;` |
| `return` | Exit function | `return value;` |

## Advanced Pattern Matching

### Destructuring in Patterns

```stratos
package main;

class Point {
    var x: int;
    var y: int;

    constructor(x: int, y: int) {
        this.x = x;
        this.y = y;
    }
}

fn classifyPoint(point: Point) string {
    return when {
        point.x == 0 and point.y == 0 -> "Origin"
        point.x == 0 -> "On Y-axis"
        point.y == 0 -> "On X-axis"
        point.x > 0 and point.y > 0 -> "Quadrant I"
        point.x < 0 and point.y > 0 -> "Quadrant II"
        point.x < 0 and point.y < 0 -> "Quadrant III"
        else -> "Quadrant IV"
    };
}

fn main() {
    val points = [
        Point(0, 0),
        Point(3, 4),
        Point(-2, 5),
        Point(-1, -3)
    ];

    for (point in points) {
        val location = classifyPoint(point);
        print("(" + point.x + ", " + point.y + ") is at: " + location);
    }
}
```

## Next Steps

- [Functions](/guide/functions) - Learn about function definitions and lambdas
- [Error Handling](/guide/error-handling) - Work with Result and Option types
- [Pattern Matching](/guide/pattern-matching) - Deep dive into advanced patterns
- [Collections](/guide/collections) - Working with arrays, maps, and sets