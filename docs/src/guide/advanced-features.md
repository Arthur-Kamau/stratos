---
title: Advanced Features
description: Defer statements, destructuring, structs, string interpolation, and other advanced Stratos features
---

# Advanced Features

This guide covers advanced Stratos features including defer statements, destructuring, structs, string interpolation, and more.

## Defer Statements

The `defer` statement schedules a function call to be executed when the surrounding function returns. Deferred calls are executed in LIFO (Last-In-First-Out) order.

### Basic Defer

```stratos
package main;

fn processFile() {
    val file = io.open("data.txt");
    defer file.close();  // Executed when function returns

    // Process the file...
    val content = file.read();
    print(content);

    // file.close() is called here automatically
}

fn main() {
    processFile();
}
```

### Multiple Defers

When multiple defer statements are used, they execute in reverse order (LIFO):

```stratos
package main;

fn multipleDefers() {
    defer print("First defer (executed last)");
    defer print("Second defer (executed second)");
    defer print("Third defer (executed first)");

    print("Function body");
}

fn main() {
    multipleDefers();
}
// Output:
// Function body
// Third defer (executed first)
// Second defer (executed second)
// First defer (executed last)
```

### Resource Cleanup Pattern

Defer is ideal for cleanup operations:

```stratos
package main;

use concurrent;

fn withMutex() {
    val mutex = concurrent.newMutex();

    mutex.lock();
    defer mutex.unlock();  // Always unlocks, even on error

    // Critical section...
    updateSharedState();
}

fn withDatabase() {
    val conn = db.connect("localhost:5432");
    defer conn.close();

    val tx = conn.beginTransaction();
    defer tx.rollback();  // Rollback if not committed

    // Perform operations...
    tx.commit();
}
```

::: tip Defer Best Practices
- Use defer for cleanup operations like closing files, releasing locks, or closing connections
- Defer is executed even if the function panics or returns early
- Keep deferred functions simple and side-effect free when possible
:::

## Destructuring

Destructuring allows you to extract values from arrays and ranges into individual variables.

### Array Destructuring

```stratos
package main;

fn main() {
    val numbers = [10, 20, 30];

    // Destructure into individual variables
    val (a, b, c) = numbers;

    print(a);  // 10
    print(b);  // 20
    print(c);  // 30

    // Mutable destructuring
    var (x, y, z) = [1, 2, 3];
    x = 100;
    print(x);  // 100
}
```

### Range Destructuring

```stratos
package main;

fn main() {
    // Destructure from range
    val (first, second, third) = 1..4;

    print(first);   // 1
    print(second);  // 2
    print(third);   // 3
}
```

### Partial Destructuring

If the array has more elements than variables, extra elements are ignored. If fewer, remaining variables get default values:

```stratos
package main;

fn main() {
    // Fewer variables than elements
    val (a, b) = [1, 2, 3, 4, 5];
    print(a);  // 1
    print(b);  // 2

    // More variables than elements (defaults to 0)
    val (x, y, z) = [100, 200];
    print(z);  // 0
}
```

## Structs

Structs are lightweight data containers, similar to classes but without methods. They're ideal for grouping related data.

### Basic Struct Definition

```stratos
package main;

struct Point {
    x: int;
    y: int;
}

struct Color {
    r: int;
    g: int;
    b: int;
}

fn main() {
    val p = Point { x: 10, y: 20 };
    print("Point: (" + p.x + ", " + p.y + ")");

    val c = Color { r: 255, g: 128, b: 0 };
    print("Color: rgb(" + c.r + ", " + c.g + ", " + c.b + ")");
}
```

### Nested Structs

```stratos
package main;

struct Point {
    x: int;
    y: int;
}

struct Color {
    r: int;
    g: int;
    b: int;
}

struct Mouse {
    position: Point;
    color: Color;
}

struct Screen {
    width: int;
    height: int;
    backgroundColor: Color;
    mouse: Mouse;
}

fn main() {
    val mouse = Mouse {
        position: Point { x: 100, y: 200 },
        color: Color { r: 255, g: 255, b: 255 }
    };

    val screen = Screen {
        width: 1920,
        height: 1080,
        backgroundColor: Color { r: 0, g: 0, b: 0 },
        mouse: mouse
    };

    print("Mouse at: (" + screen.mouse.position.x + ", " + screen.mouse.position.y + ")");
}
```

::: info Struct vs Class
- Use **structs** for simple data containers without behavior
- Use **classes** when you need methods, constructors, or inheritance
- Structs are value types; classes are reference types
:::

## String Interpolation

String interpolation allows you to embed expressions directly in strings using `$` syntax.

### Variable Interpolation

```stratos
package main;

fn main() {
    val name = "Alice";
    val age = 30;

    // Simple variable interpolation with $
    print("Name: $name, Age: $age");
    // Output: Name: Alice, Age: 30
}
```

### Expression Interpolation

For complex expressions, use `${...}`:

```stratos
package main;

fn main() {
    val x = 10;
    val y = 20;

    // Expression interpolation
    print("Sum: ${x + y}");
    print("Product: ${x * y}");
    print("Is x greater? ${x > y}");

    // Output:
    // Sum: 30
    // Product: 200
    // Is x greater? false
}
```

### Combining Interpolation Styles

```stratos
package main;

fn main() {
    val firstName = "John";
    val lastName = "Doe";
    val scores = [85, 92, 78];

    print("Student: $firstName $lastName");
    print("Average: ${(scores[0] + scores[1] + scores[2]) / 3}");
    print("Greeting: Hello, ${firstName + " " + lastName}!");
}
```

::: tip Interpolation Syntax
- `$variable` - For simple variable references
- `${expression}` - For complex expressions, method calls, or arithmetic
:::

## Select Statement

The `select` statement is used for channel multiplexing, allowing you to wait on multiple channel operations simultaneously.

```stratos
package main;

use concurrent;

fn main() {
    val ch1 = concurrent.newChannel<int>(1);
    val ch2 = concurrent.newChannel<string>(1);

    // Send values in separate goroutines
    concurrent.go(|| {
        concurrent.sleep(100);
        ch1.send(42);
    });

    concurrent.go(|| {
        concurrent.sleep(50);
        ch2.send("hello");
    });

    // Select waits on multiple channels
    select {
        case receive ch1:
            print("Received from ch1")
        case receive ch2:
            print("Received from ch2")
        default:
            print("No channel ready")
    }
}
```

## Type Aliases

Type aliases create alternative names for types, improving code readability:

```stratos
package main;

// Type aliases
type UserId = int;
type Email = string;
type Handler = Function<Request, Response, void>;
type Callback = Function<int, bool>;

fn processUser(id: UserId, email: Email) {
    print("Processing user " + id + " with email " + email);
}

fn main() {
    val userId: UserId = 12345;
    val email: Email = "user@example.com";

    processUser(userId, email);
}
```

## Bitwise Operators

Stratos supports standard bitwise operations:

| Operator | Description | Example |
|----------|-------------|---------|
| `&` | Bitwise AND | `a & b` |
| `\|` | Bitwise OR | `a \| b` |
| `^` | Bitwise XOR | `a ^ b` |
| `~` | Bitwise NOT | `~a` |
| `<<` | Left shift | `a << 2` |
| `>>` | Right shift | `a >> 2` |

```stratos
package main;

fn main() {
    val a = 0b1010;  // 10 in binary
    val b = 0b1100;  // 12 in binary

    print("AND: " + (a & b));   // 0b1000 = 8
    print("OR:  " + (a | b));   // 0b1110 = 14
    print("XOR: " + (a ^ b));   // 0b0110 = 6
    print("NOT: " + (~a));      // Inverts all bits

    print("Left shift:  " + (a << 2));  // 40
    print("Right shift: " + (a >> 1));  // 5
}
```

## Low-Level Numeric Types

For performance-critical code, Stratos provides fixed-size numeric types:

### Signed Integers

| Type | Size | Range |
|------|------|-------|
| `i8` | 8 bits | -128 to 127 |
| `i16` | 16 bits | -32,768 to 32,767 |
| `i32` | 32 bits | -2^31 to 2^31-1 |
| `i64` | 64 bits | -2^63 to 2^63-1 |
| `isize` | Platform | Architecture dependent |

### Unsigned Integers

| Type | Size | Range |
|------|------|-------|
| `u8` | 8 bits | 0 to 255 |
| `u16` | 16 bits | 0 to 65,535 |
| `u32` | 32 bits | 0 to 2^32-1 |
| `u64` | 64 bits | 0 to 2^64-1 |
| `usize` | Platform | Architecture dependent |

### Floating Point

| Type | Size | Precision |
|------|------|-----------|
| `f32` | 32 bits | Single precision |
| `f64` | 64 bits | Double precision |

```stratos
package main;

fn main() {
    val byte: u8 = 255;
    val word: u16 = 65535;
    val dword: u32 = 4294967295;
    val qword: u64 = 18446744073709551615;

    val signed: i32 = -42;
    val float32: f32 = 3.14;
    val float64: f64 = 3.14159265358979;

    print("Byte: " + byte);
    print("Float64: " + float64);
}
```

## Safe Navigation Operator

The safe navigation operator `?.` allows you to safely access properties on potentially null objects:

```stratos
package main;

fn main() {
    val user: Optional<User> = getUser(123);

    // Safe navigation - returns None if user is None
    val name = user?.name;
    val email = user?.profile?.email;

    // Chain multiple safe accesses
    val city = user?.address?.city;

    if (city != None) {
        print("City: " + city);
    }
}
```

## Elvis Operator

The elvis operator `?:` provides a default value when the left side is null:

```stratos
package main;

fn main() {
    val name: Optional<string> = getName();

    // Use default if name is None
    val displayName = name ?: "Anonymous";

    print("Hello, " + displayName);

    // Chain with safe navigation
    val city = user?.address?.city ?: "Unknown";
}
```

## Result and Optional Types

### Creating Result Values

```stratos
package main;

fn divide(a: int, b: int) Result<int, string> {
    if (b == 0) {
        return Result.err("Division by zero");
    }
    return Result.ok(a / b);
}

fn main() {
    val result1 = divide(10, 2);
    val result2 = divide(10, 0);

    when (result1) {
        Ok(value) -> print("Result: " + value)
        Err(error) -> print("Error: " + error)
    }
}
```

### Creating Optional Values

```stratos
package main;

fn findUser(id: int) Optional<User> {
    if (id == 0) {
        return Optional.none();
    }
    return Optional.some(User(id, "Alice"));
}

fn main() {
    val user = findUser(123);

    if (user.isSome()) {
        print("Found: " + user.unwrap().name);
    }

    // With default
    val name = findUser(0).unwrapOr(User(0, "Guest")).name;
}
```

### Optional Methods

```stratos
package main;

fn main() {
    val opt = Optional.some(42);

    print(opt.isSome());     // true
    print(opt.isNone());     // false
    print(opt.unwrap());     // 42
    print(opt.unwrapOr(0));  // 42

    val empty = Optional.none<int>();
    print(empty.unwrapOr(100));  // 100
}
```

## Documentation Comments

Stratos supports documentation comments for generating API documentation:

### Single-Line Doc Comments

```stratos
/// Calculates the factorial of a number.
/// @param n The number to calculate factorial for
/// @return The factorial of n
fn factorial(n: int) int {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
```

### Multi-Line Doc Comments

```stratos
/**
 * Represents a user in the system.
 *
 * This class handles user authentication and profile management.
 * Use the static create() method to instantiate new users.
 */
class User {
    var name: string;
    var email: string;

    /**
     * Creates a new user with the given credentials.
     * @param name The user's display name
     * @param email The user's email address
     */
    constructor(name: string, email: string) {
        this.name = name;
        this.email = email;
    }
}
```

## Visibility Modifiers

Control access to class members with visibility modifiers:

```stratos
package main;

class Example {
    var publicField: int;           // Public by default
    pub var explicitPublic: string; // Explicitly public

    fn privateMethod() {            // Private by default
        print("Only accessible within class");
    }

    pub fn publicMethod() {         // Explicitly public
        print("Accessible from anywhere");
    }
}

fn main() {
    val ex = Example();
    ex.publicField = 10;     // OK
    ex.explicitPublic = "hi"; // OK
    ex.publicMethod();        // OK
    // ex.privateMethod();    // Error: private method
}
```

## Complete Example

```stratos
package main;

use io;
use concurrent;

struct Config {
    host: string;
    port: int;
    debug: bool;
}

fn loadConfig() Result<Config, string> {
    val content = io.readFile("config.json");
    if (content == "") {
        return Result.err("Failed to read config");
    }

    return Result.ok(Config {
        host: "localhost",
        port: 8080,
        debug: true
    });
}

fn main() {
    val configResult = loadConfig();

    when (configResult) {
        Ok(config) -> {
            val url = "http://${config.host}:${config.port}";
            print("Server starting at $url");

            if (config.debug) {
                defer print("Debug: Server shutdown complete");
            }

            // Destructure for convenience
            val (host, port, _) = [config.host, config.port, config.debug];
            print("Host: $host, Port: $port");
        }
        Err(error) -> {
            print("Configuration error: $error");
        }
    }
}
```

## Next Steps

- [Concurrency](/guide/concurrency) - Goroutines and channels
- [Async Programming](/guide/async) - Async/await patterns
- [Error Handling](/guide/error-handling) - Working with Result types
- [Standard Library](/reference/stdlib) - Explore built-in modules
