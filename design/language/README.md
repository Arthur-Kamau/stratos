# Stratos Language Design Guide

This guide helps you understand when to use each of Stratos's organizational and type constructs.

## Quick Reference

| Construct | Purpose | Can Be Instantiated? | Can Inherit? | Has State? |
|-----------|---------|---------------------|--------------|------------|
| **Package** | Code organization & modules | ❌ No | ❌ No | ❌ No |
| **Class** | Objects with behavior & state | ✅ Yes | ✅ Yes | ✅ Yes |
| **Struct** | Plain data containers | ✅ Yes | ❌ No | ✅ Yes (immutable preferred) |
| **Interface** | Contracts/abstract behavior | ❌ No | ✅ Yes (interfaces can extend) | ❌ No |

---

## When to Use Each Construct

### 📦 PACKAGE

**Use when you need to:**
- Organize code into modules (Go-style)
- Group related code in a directory
- Prevent name collisions
- Create module boundaries

**Examples:**
```stratos
// math/operations.st
package math

fn abs(x: int) int { ... }
fn max(a: int, b: int) int { ... }
```

```stratos
// main.st
package main

use math;

fn main() {
    val result = math.abs(-5);
}
```

**Think of package as:**
- One directory = one package (like Go)
- Package declaration at top of each file
- Use `use` keyword for imports
- Cannot be instantiated - just a container

**See:** `packages.md` for full documentation

---

### 🏗️ CLASS

**Use when you need:**
- Objects with both behavior (methods) AND state (fields)
- Inheritance and polymorphism
- Reference semantics
- Complex objects that encapsulate logic and data

**Examples:**
```stratos
class FileReader {
    var path: string;
    var isOpen: bool;

    constructor(p: string) {
        this.path = p;
        this.isOpen = false;
    }

    fn open() bool { ... }
    fn read() Optional<string> { ... }
    fn close() { ... }
}

class Database {
    var connection: Connection;
    var queryHistory: list<string>;

    fn connect(host: string) { ... }
    fn query(sql: string) Optional<Result> { ... }
}
```

**Characteristics:**
- Can have constructors and destructors
- Can inherit from other classes or implement interfaces
- Can have mutable state
- Reference type (passed by reference)

**See:** `inheritance.md`, `constructor_destructor.md`

---

### 📦 STRUCT

**Use when you need:**
- Simple data containers
- Value semantics (copy by value)
- Grouping related data without complex behavior
- Lightweight objects

**Examples:**
```stratos
struct Point {
    x: double,
    y: double
}

struct Color {
    r: int,
    g: int,
    b: int
}

struct UserConfig {
    theme: string,
    fontSize: int,
    autoSave: bool
}
```

**Characteristics:**
- Just data fields (no inheritance)
- Value type (copied when assigned/passed)
- Lightweight - no vtables or overhead
- Can have methods in some designs, but primarily for data

**When to choose struct over class:**
- No need for inheritance
- Primarily data, minimal behavior
- Want value semantics (copy instead of reference)
- Small, simple objects (Point, Color, Rectangle)

---

### 📋 INTERFACE

**Use when you need:**
- Define a contract (set of methods) that classes must implement
- Polymorphism without inheritance
- Multiple types to share behavior without sharing implementation
- Abstract behavior definition

**Examples:**
```stratos
interface Drawable {
    fn draw() unit;
    fn getBounds() Rectangle;
}

interface Serializable {
    fn serialize() string;
    fn deserialize(data: string) bool;
}

class Shape : Drawable {
    fn draw() { ... }
    fn getBounds() Rectangle { ... }
}
```

**Characteristics:**
- Only method signatures (no implementation)
- Cannot be instantiated
- Classes implement interfaces
- Interfaces can extend other interfaces
- Used for contracts and polymorphism

**See:** `inheritance.md`

---

## Decision Tree

```
Do you need to organize utility functions or create a module?
├─ YES → Use PACKAGE
└─ NO ↓

Do you need to define a contract/behavior that others will implement?
├─ YES → Use INTERFACE
└─ NO ↓

Do you need inheritance or complex behavior with state?
├─ YES → Use CLASS
└─ NO ↓

Do you just need to group related data?
└─ YES → Use STRUCT
```

---

## Common Patterns

### Pattern 1: Package + Struct + Functions
**For modules with data types and utility functions**

```stratos
package Geometry {
    struct Point {
        x: double,
        y: double
    }

    struct Rectangle {
        topLeft: Point,
        width: double,
        height: double
    }

    fn distance(p1: Point, p2: Point) double { ... }
    fn area(rect: Rectangle) double { ... }
}
```

### Pattern 2: Interface + Class
**For polymorphism and contracts**

```stratos
interface Animal {
    fn makeSound() unit;
    fn move() unit;
}

class Dog : Animal {
    fn makeSound() { print("Woof"); }
    fn move() { print("Running"); }
}

class Bird : Animal {
    fn makeSound() { print("Chirp"); }
    fn move() { print("Flying"); }
}
```

### Pattern 3: Package + Class
**For organizing complex objects**

```stratos
package Database {
    struct Config {
        host: string,
        port: int
    }

    class Connection {
        var config: Config;
        var isConnected: bool;

        constructor(cfg: Config) { ... }
        fn connect() { ... }
        fn query(sql: string) Optional<Result> { ... }
    }

    fn createConnection(host: string, port: int) Connection { ... }
}
```

---

## Real-World Examples

### Example 1: Math Library
```stratos
package Math {
    // Constants
    val PI: double = 3.14159;

    // Utility functions
    fn sqrt(x: double) double { ... }
    fn pow(base: double, exp: double) double { ... }

    // Data structures
    struct Vector3 {
        x: double,
        y: double,
        z: double
    }

    fn dotProduct(v1: Vector3, v2: Vector3) double { ... }
}
```

### Example 2: Graphics System
```stratos
package Graphics {
    interface Renderable {
        fn render() unit;
    }

    struct Color {
        r: int, g: int, b: int
    }

    class Sprite : Renderable {
        var position: Point;
        var color: Color;

        fn render() { ... }
        fn move(dx: double, dy: double) { ... }
    }
}
```

### Example 3: File I/O
```stratos
package IO {
    struct Path {
        value: string
    }

    enum FileMode {
        Read,
        Write,
        Append
    }

    fn readFile(path: Path) Optional<string> { ... }
    fn writeFile(path: Path, content: string) bool { ... }

    class FileStream {
        var path: Path;
        var mode: FileMode;

        constructor(p: Path, m: FileMode) { ... }
        fn read() Optional<string> { ... }
        fn write(data: string) bool { ... }
        fn close() unit { ... }
    }
}
```

---

## Anti-Patterns to Avoid

### ❌ Don't: Use package when you need state
```stratos
// BAD - packages cannot have fields
package Counter {
    var count: int;  // ERROR!
    fn increment() { ... }
}

// GOOD - use a class
class Counter {
    var count: int;
    fn increment() { this.count += 1; }
}
```

### ❌ Don't: Use class when struct is sufficient
```stratos
// BAD - unnecessary complexity
class Point {
    var x: double;
    var y: double;
    constructor(x: double, y: double) { ... }
}

// GOOD - simple data container
struct Point {
    x: double,
    y: double
}
```

### ❌ Don't: Use struct when you need inheritance
```stratos
// BAD - structs don't support inheritance
struct Animal {
    name: string
}
// Cannot inherit from struct!

// GOOD - use class for inheritance
class Animal {
    var name: string;
}

class Dog : Animal {
    fn bark() { ... }
}
```

---

## Summary

| If you need... | Use |
|----------------|-----|
| Organize utility functions | **Package** |
| Group related types and functions | **Package** |
| Objects with behavior and state | **Class** |
| Inheritance or polymorphism | **Class** + **Interface** |
| Simple data containers | **Struct** |
| Define a contract | **Interface** |
| Value semantics (copy) | **Struct** |
| Reference semantics | **Class** |

**Remember:** Choose the simplest construct that meets your needs. Start with structs for data, classes for objects, packages for organization, and interfaces for contracts.

---

## Further Reading

- `packages.md` - Complete package documentation
- `inheritance.md` - Classes and inheritance
- `types.md` - Data types overview
- `variables.md` - Variable declarations
