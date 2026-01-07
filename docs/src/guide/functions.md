---
title: Functions
description: Function declarations, parameters, and the pipe operator in Stratos
---

# Functions

Learn how to define and use functions in Stratos, including the powerful pipe operator for function composition.

## Function Basics

### Simple Function Declaration

```stratos
fn greet() {
    print("Hello, Stratos!");
}

fn main() {
    greet();  // Call the function
}
// Output: Hello, Stratos!
```

### Functions with Parameters

```stratos
fn greet(name: string) {
    print("Hello, " + name + "!");
}

fn main() {
    greet("Alice");   // Hello, Alice!
    greet("Bob");     // Hello, Bob!
}
```

### Functions with Return Values

```stratos
fn add(a: int, b: int) int {
    return a + b;
}

fn multiply(x: double, y: double) double {
    return x * y;
}

fn main() {
    val sum = add(5, 3);          // 8
    val product = multiply(4.5, 2.0);  // 9.0

    print("Sum: " + sum);
    print("Product: " + product);
}
```

### Single-Expression Functions

For simple functions, you can omit the braces and `return`:

```stratos
fn square(x: int) int = x * x;

fn isEven(n: int) bool = n % 2 == 0;

fn max(a: int, b: int) int = if (a > b) a else b;

fn main() {
    print(square(5));      // 25
    print(isEven(4));      // true
    print(max(10, 20));    // 20
}
```

## Multiple Parameters

```stratos
fn calculateArea(width: double, height: double) double {
    return width * height;
}

fn formatName(first: string, last: string, middle: string) string {
    return first + " " + middle + " " + last;
}

fn main() {
    val area = calculateArea(5.0, 10.0);
    print("Area: " + area);  // 50.0

    val fullName = formatName("John", "Doe", "Q");
    print(fullName);  // John Q Doe
}
```

## The Pipe Operator (|>)

The pipe operator allows you to chain function calls in a readable, left-to-right manner.

### Basic Pipe Usage

```stratos
fn double(x: int) int {
    return x * 2;
}

fn square(x: int) int {
    return x * x;
}

fn main() {
    // Without pipe operator
    val result1 = square(double(5));
    print(result1);  // 100

    // With pipe operator (more readable)
    val result2 = 5 |> double() |> square();
    print(result2);  // 100

    // Equivalent to: square(double(5))
}
```

### Pipe Operator Chains

```stratos
fn increment(x: int) int = x + 1;
fn double(x: int) int = x * 2;
fn square(x: int) int = x * x;

fn main() {
    // Chain multiple operations
    val result = 3
        |> increment()  // 4
        |> double()     // 8
        |> square();    // 64

    print(result);  // 64
}
```

### Pipe with String Operations

```stratos
use strings;

fn main() {
    val text = "  Hello, Stratos!  ";

    // Chain string transformations
    val result = text
        |> strings.trim()
        |> strings.toUpper()
        |> strings.replace("STRATOS", "WORLD");

    print(result);  // HELLO, WORLD!
}
```

## Function Composition

Combine functions to create more complex operations:

```stratos
fn add(a: int, b: int) int = a + b;
fn multiply(a: int, b: int) int = a * b;
fn subtract(a: int, b: int) int = a - b;

fn calculate(x: int, y: int) int {
    val sum = add(x, y);
    val product = multiply(x, y);
    return subtract(product, sum);
}

fn main() {
    val result = calculate(5, 3);
    // (5 * 3) - (5 + 3) = 15 - 8 = 7
    print(result);  // 7
}
```

## Higher-Order Functions

Functions can accept other functions as parameters and return functions as results.

### Functions as Parameters

```stratos
fn apply(x: int, operation: Function) int {
    return operation(x);
}

fn double(x: int) int = x * 2;
fn square(x: int) int = x * x;

fn main() {
    // Pass named functions
    val doubled = apply(5, double);
    print(doubled);  // 10

    val squared = apply(5, square);
    print(squared);  // 25

    // Pass lambda expressions
    val tripled = apply(5, (x) => x * 3);
    print(tripled);  // 15
}
```

### Functions Returning Functions

```stratos
fn makeOperation(operation: string) Function {
    if (operation == "double") {
        return (x) => x * 2;
    } else if (operation == "square") {
        return (x) => x * x;
    } else {
        return (x) => x;  // identity
    }
}

fn main() {
    val doubler = makeOperation("double");
    val squarer = makeOperation("square");

    print(doubler(10));  // 20
    print(squarer(5));   // 25
}
```

### Practical Example: Array Operations

```stratos
fn forEach(arr: Array<int>, action: Function) void {
    for val i in 0..arr.length() {
        action(arr[i]);
    }
}

fn filter(arr: Array<int>, predicate: Function) Array<int> {
    val result = [];
    for val i in 0..arr.length() {
        if (predicate(arr[i])) {
            result.push(arr[i]);
        }
    }
    return result;
}

fn main() {
    val numbers = [1, 2, 3, 4, 5, 6];

    // Print each number
    forEach(numbers, (n) => println(n));

    // Filter even numbers
    val evens = filter(numbers, (n) => n % 2 == 0);
    println(evens);  // [2, 4, 6]

    // Filter numbers > 3
    val large = filter(numbers, (n) => n > 3);
    println(large);  // [4, 5, 6]
}
```

## Recursive Functions

Functions can call themselves:

```stratos
fn factorial(n: int) int {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

fn fibonacci(n: int) int {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

fn main() {
    print("Factorials:");
    for (i in 1..=5) {
        print(i + "! = " + factorial(i));
    }

    print("\nFibonacci:");
    for (i in 0..=10) {
        print("fib(" + i + ") = " + fibonacci(i));
    }
}
```

### Output

```
Factorials:
1! = 1
2! = 2
3! = 6
4! = 24
5! = 120

Fibonacci:
fib(0) = 0
fib(1) = 1
fib(2) = 1
fib(3) = 2
fib(4) = 3
fib(5) = 5
fib(6) = 8
fib(7) = 13
fib(8) = 21
fib(9) = 34
fib(10) = 55
```

## Default Parameters

```stratos
fn greet(name: string, greeting: string = "Hello") {
    print(greeting + ", " + name + "!");
}

fn main() {
    greet("Alice");              // Hello, Alice!
    greet("Bob", "Hi");          // Hi, Bob!
    greet("Charlie", "Welcome"); // Welcome, Charlie!
}
```

## Variable Arguments (Variadic)

```stratos
fn sum(...numbers: int) int {
    var total = 0;
    for (num in numbers) {
        total += num;
    }
    return total;
}

fn main() {
    print(sum(1, 2, 3));           // 6
    print(sum(10, 20, 30, 40));    // 100
    print(sum(5));                 // 5
}
```

## Complete Example: Math Utilities

```stratos
package main;

use math;

// Basic arithmetic functions
fn add(a: int, b: int) int = a + b;
fn subtract(a: int, b: int) int = a - b;
fn multiply(a: int, b: int) int = a * b;
fn divide(a: int, b: int) int = a / b;

// Utility functions
fn square(x: int) int = x * x;
fn cube(x: int) int = x * x * x;
fn double(x: int) int = x * 2;
fn increment(x: int) int = x + 1;

// Higher-level functions
fn isEven(n: int) bool = n % 2 == 0;
fn isOdd(n: int) bool = !isEven(n);

fn isPrime(n: int) bool {
    if (n <= 1) return false;
    if (n <= 3) return true;

    if (n % 2 == 0 || n % 3 == 0) {
        return false;
    }

    var i = 5;
    while (i * i <= n) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
        i += 6;
    }

    return true;
}

// Pipe operator examples
fn processNumber(x: int) int {
    return x
        |> increment()   // x + 1
        |> double()      // (x + 1) * 2
        |> square();     // ((x + 1) * 2)^2
}

fn main() {
    // Test basic functions
    print("5 + 3 = " + add(5, 3));
    print("10 - 4 = " + subtract(10, 4));
    print("6 * 7 = " + multiply(6, 7));
    print("20 / 4 = " + divide(20, 4));

    // Test utility functions
    print("\nSquares:");
    for (i in 1..=5) {
        print(i + "^2 = " + square(i));
    }

    // Test pipe operator
    print("\nPipe operator:");
    val result = processNumber(3);
    // 3 -> 4 -> 8 -> 64
    print("processNumber(3) = " + result);

    // Test prime checking
    print("\nPrime numbers from 1 to 20:");
    for (i in 1..=20) {
        if (isPrime(i)) {
            print(i);
        }
    }

    // Even/Odd classification
    print("\nEven/Odd:");
    for (i in 1..=10) {
        val type = if (isEven(i)) "even" else "odd";
        print(i + " is " + type);
    }
}
```

## Lambda Expressions (Arrow Functions)

Stratos supports concise anonymous functions using arrow syntax `=>`.

### Basic Syntax

```stratos
// Single parameter (no parentheses needed)
val double = (x) => x * 2;

// Multiple parameters
val add = (a, b) => a + b;

// Block body lambda (explicit return)
val multiply = (a, b) => {
    val result = a * b;
    return result;
};

fn main() {
    print(double(5));      // 10
    print(add(5, 3));      // 8
    print(multiply(4, 5)); // 20
}
```

### Passing as Arguments

Lambdas are commonly used as callbacks or with higher-order functions.

```stratos
fn process(value: int, callback: Function) int {
    return callback(value);
}

fn main() {
    // Pass lambda directly
    val result1 = process(10, (x) => x * 2);
    print(result1);  // 20

    val result2 = process(5, (x) => {
        val y = x + 1;
        return y * y;
    });
    print(result2);  // 36
}
```

### Capturing Variables (Closures)

Lambdas capture variables from their surrounding scope, creating closures.

```stratos
fn makeAdder(n: int) Function {
    return (x) => x + n;  // 'n' is captured from outer scope
}

fn makeMultiplier(factor: int) Function {
    return (x) => x * factor;
}

fn main() {
    val add10 = makeAdder(10);
    val add5 = makeAdder(5);

    print(add10(5));  // 15
    print(add5(5));   // 10

    val triple = makeMultiplier(3);
    print(triple(7)); // 21
}
```

### Nested Functions with Callbacks

Functions can be defined inside other functions and use callbacks:

```stratos
fn main() {
    // Define a helper function inside main
    fn apply(value: int, op: Function) int {
        return op(value);
    }

    // Use with inline lambdas
    val res1 = apply(10, (x) => x * 2);
    print("10 * 2 = " + res1);  // 10 * 2 = 20

    // Closures capture environment
    val factor = 5;
    val res2 = apply(3, (x) => x * factor);
    print("3 * 5 = " + res2);   // 3 * 5 = 15
}
```

::: tip Lambda Syntax
Use `=>` (fat arrow) for lambda expressions. The old `->` syntax is not supported for lambdas.
:::

::: warning Keyword Conflicts
Avoid using keywords like `val`, `var`, `fn`, `if`, etc. as parameter names. Use descriptive names like `value`, `item`, `element` instead.
:::

### Complete Callback Example

```stratos
package main;

fn main() {
    println("--- Callback Showcase ---");

    // 1. Simple callback
    fn apply(value: int, op: Function) int {
        return op(value);
    }

    val res1 = apply(10, (x) => x * 2);
    println("10 * 2 = " + res1);

    // 2. Closure capturing environment
    val factor = 5;
    val res2 = apply(3, (x) => x * factor);
    println("3 * 5 = " + res2);

    // 3. Returning a function (Higher-Order Function)
    fn makeAdder(n: int) Function {
        return (x) => x + n;
    }

    val add10 = makeAdder(10);
    println("5 + 10 = " + add10(5));

    // 4. Map implementation using callbacks
    fn map(arr: Array<int>, transform: Function) Array<int> {
        val result = [];

        for val i in 0..arr.length() {
            val item = arr[i];
            val transformed = transform(item);
            result.push(transformed);
        }
        return result;
    }

    println("--- Done ---");
}
```

## Best Practices

::: tip
**Use the pipe operator**: For function chains, the pipe operator improves readability by showing the data flow from left to right.
:::

::: tip
**Single Responsibility**: Each function should do one thing well. Break complex operations into smaller, composable functions.
:::

::: tip
**Meaningful names**: Function names should clearly describe what they do. Use verbs for actions (`calculate`, `process`, `validate`).
:::

::: warning
**Avoid deep recursion**: Recursive functions without tail-call optimization can cause stack overflow for large inputs.
:::

::: info
**Type annotations**: Always specify parameter and return types for clarity and type safety.
:::

## Function Quick Reference

| Feature | Syntax | Example |
|---------|--------|---------|
| Basic function | `fn name() { ... }` | `fn greet() { print("Hi"); }` |
| With parameters | `fn name(param: Type)` | `fn add(a: int, b: int)` |
| With return type | `fn name() Type` | `fn double(x: int) int` |
| Single expression | `fn name() Type = expr` | `fn square(x: int) int = x * x` |
| Pipe operator | `value \|> func()` | `5 \|> double() \|> square()` |
| Lambda (single expr) | `(params) => expr` | `(x) => x * 2` |
| Lambda (block) | `(params) => { ... }` | `(x) => { return x * 2; }` |
| Higher-order function | `fn name(f: Function) Type` | `fn apply(x: int, f: Function) int` |
| Closure | `return (params) => expr` | `return (x) => x + n` |

## Next Steps


