---
title: Control Flow
description: If statements, loops, and pattern matching in Stratos
---

# Control Flow

Learn how to control program flow in Stratos using conditionals, loops, and pattern matching.

## If Statements

### Basic If-Else

```stratos
val age = 20;

if (age >= 18) {
    print("Adult");
} else {
    print("Minor");
}
```

### If-Else-If Chains

```stratos
val age = 20;

if (age > 60) {
    print("Senior");
} else if (age > 18) {
    print("Adult");
} else {
    print("Child");
}
```

### If as Expression (Ternary)

In Stratos, `if` is an expression that returns a value:

```stratos
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
```

## While Loops

### Basic While Loop

```stratos
var i = 0;
while (i < 10) {
    print(i);
    i++;
}
// Prints: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
```

### While with Break

```stratos
var count = 0;
while (true) {
    print(count);
    count++;

    if (count >= 5) {
        break;  // Exit the loop
    }
}
// Prints: 0, 1, 2, 3, 4
```

### While with Continue

```stratos
var i = 0;
while (i < 10) {
    i++;

    if (i % 2 == 0) {
        continue;  // Skip even numbers
    }

    print(i);
}
// Prints: 1, 3, 5, 7, 9
```

## For Loops

### Iterating Over Arrays

```stratos
val fruits = ["apple", "banana", "orange"];

for (fruit in fruits) {
    print(fruit);
}
// Output:
// apple
// banana
// orange
```

### Iterating with Index

```stratos
val numbers = [10, 20, 30, 40, 50];

for (i, num in numbers) {
    print("Index " + i + ": " + num);
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
// Loop from 0 to 4
for (i in 0..5) {
    print(i);
}
// Prints: 0, 1, 2, 3, 4

// Loop from 1 to 10
for (i in 1..=10) {
    print(i);
}
// Prints: 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
```

## Pattern Matching (When)

Stratos provides powerful pattern matching with the `when` expression.

### Basic When Expression

```stratos
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
// Output: Full Access
```

### When as Expression

```stratos
val userType = "editor";

val accessLevel = when (userType) {
    "admin" -> "Full Access"
    "editor" -> "Edit Access"
    "viewer" -> "Read Only"
    else -> "No Access"
};

print(accessLevel);  // "Edit Access"
```

### Pattern Matching with Numbers

```stratos
val score = 85;

val grade = when (score) {
    90..100 -> "A"
    80..89 -> "B"
    70..79 -> "C"
    60..69 -> "D"
    else -> "F"
};

print("Grade: " + grade);  // "Grade: B"
```

### Multiple Conditions

```stratos
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
// Output: Weekday
```

### Pattern Matching with Guards

```stratos
val age = 25;
val hasLicense = true;

when (age) {
    _ if age >= 18 && hasLicense -> {
        print("Can drive");
    }
    _ if age >= 18 -> {
        print("Can vote but not drive");
    }
    else -> {
        print("Minor");
    }
}
// Output: Can drive
```

## Complete Example: Number Classifier

Here's a complete program demonstrating various control flow constructs:

```stratos
package main;

fn classifyNumber(num: int) string {
    return when {
        num < 0 -> "Negative"
        num == 0 -> "Zero"
        num > 0 && num <= 10 -> "Small Positive"
        num > 10 && num <= 100 -> "Medium Positive"
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

### Output

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

## Best Practices

::: tip
**Use `when` for multiple conditions**: Pattern matching with `when` is more readable than long if-else-if chains.
:::

::: tip
**Prefer expressions over statements**: Use `if` and `when` as expressions to reduce mutable variables and make code more functional.
:::

::: warning
**Avoid infinite loops**: Always ensure your `while` loops have a proper exit condition or use `break` to exit.
:::

::: info
**Exhaustive matching**: The `else` branch in `when` expressions is required unless all possible cases are covered.
:::

## Control Flow Quick Reference

| Construct | Purpose | Example |
|-----------|---------|---------|
| `if-else` | Conditional execution | `if (x > 0) { ... } else { ... }` |
| `while` | Loop while condition is true | `while (i < 10) { i++; }` |
| `for-in` | Iterate over collection | `for (x in array) { ... }` |
| `when` | Pattern matching | `when (x) { 1 -> ..., else -> ... }` |
| `break` | Exit loop early | `break;` |
| `continue` | Skip to next iteration | `continue;` |
| `return` | Exit function | `return value;` |

## Next Steps


