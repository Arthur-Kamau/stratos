# Inheritance

Stratos uses single inheritance for classes. This allows code reuse and polymorphic behavior while keeping the type hierarchy simple.

## Class Inheritance

Classes can inherit from a single parent class using the `:` syntax.

### Basic Example

```stratos
class Shape {
    var color: string;

    constructor(c: string) {
        this.color = c;
    }

    fn hasAngles() bool {
        return false;  // Default implementation
    }

    fn area() double {
        return 0.0;  // Default implementation
    }
}

class Rectangle : Shape {
    var width: double;
    var height: double;

    constructor(c: string, w: double, h: double) {
        super(c);  // Call parent constructor
        this.width = w;
        this.height = h;
    }

    override fn hasAngles() bool {
        return true;
    }

    override fn area() double {
        return this.width * this.height;
    }
}
```

### Inheritance Rules

- ✅ **Single inheritance only**: A class can inherit from only ONE parent class
- ✅ **Override methods**: Use `override` keyword to override parent methods
- ✅ **Call parent constructor**: Use `super(args)` to call parent constructor
- ❌ **No multiple inheritance**: Cannot inherit from multiple classes
- ❌ **Structs cannot inherit**: Only classes support inheritance

---

## Interfaces

Interfaces define contracts that classes must implement. Unlike classes, a class can implement multiple interfaces.

### Basic Interface

```stratos
interface Drawable {
    fn draw() unit;
    fn getBounds() Rectangle;
}

interface Serializable {
    fn serialize() string;
}

// Class can implement multiple interfaces
class Shape : Drawable, Serializable {
    fn draw() {
        print("Drawing shape");
    }

    fn getBounds() Rectangle {
        // Return bounding rectangle
    }

    fn serialize() string {
        return "shape_data";
    }
}
```

### Interface Inheritance

Interfaces can extend other interfaces:

```stratos
interface Animal {
    fn makeSound() unit;
}

interface Pet : Animal {
    fn play() unit;
}

class Dog : Pet {
    fn makeSound() {
        print("Woof");
    }

    fn play() {
        print("Fetching ball");
    }
}
```

---

## Class vs Struct vs Interface

### When to Use Each

| Feature | Class | Struct | Interface |
|---------|-------|--------|-----------|
| Inheritance | ✅ Yes | ❌ No | ✅ Yes (interface extends interface) |
| Instantiation | ✅ Yes | ✅ Yes | ❌ No |
| Has fields/state | ✅ Yes | ✅ Yes | ❌ No |
| Has methods | ✅ Yes | Limited | ❌ No (only signatures) |
| Multiple inheritance | ❌ No | ❌ No | ✅ Yes (implement multiple) |
| Polymorphism | ✅ Yes | ❌ No | ✅ Yes |

### Use CLASS when:
- You need inheritance
- You have complex behavior with state
- You want polymorphism through class hierarchy
- Example: `GameEngine`, `DatabaseConnection`, `FileReader`

### Use STRUCT when:
- You just need to group data
- No inheritance required
- Simple data containers
- Example: `Point`, `Color`, `Config`

### Use INTERFACE when:
- You want to define a contract
- You need multiple types to share behavior without sharing implementation
- You want polymorphism without inheritance
- Example: `Drawable`, `Comparable`, `Serializable`

---

## Complete Example

```stratos
// Interface defining a contract
interface Renderable {
    fn render() unit;
    fn getPosition() Point;
}

// Data structure (no inheritance)
struct Point {
    x: double,
    y: double
}

struct Color {
    r: int,
    g: int,
    b: int
}

// Base class
class Shape : Renderable {
    var position: Point;
    var color: Color;

    constructor(pos: Point, col: Color) {
        this.position = pos;
        this.color = col;
    }

    fn render() {
        print("Rendering shape");
    }

    fn getPosition() Point {
        return this.position;
    }

    fn area() double {
        return 0.0;  // Override in subclasses
    }
}

// Derived class
class Circle : Shape {
    var radius: double;

    constructor(pos: Point, col: Color, r: double) {
        super(pos, col);
        this.radius = r;
    }

    override fn render() {
        print("Rendering circle");
    }

    override fn area() double {
        return 3.14159 * this.radius * this.radius;
    }
}

// Another derived class
class Rectangle : Shape {
    var width: double;
    var height: double;

    constructor(pos: Point, col: Color, w: double, h: double) {
        super(pos, col);
        this.width = w;
        this.height = h;
    }

    override fn render() {
        print("Rendering rectangle");
    }

    override fn area() double {
        return this.width * this.height;
    }
}

// Usage - polymorphism
fn renderShapes(shapes: list<Shape>) {
    for shape in shapes {
        shape.render();  // Calls appropriate override
        print("Area: " + shape.area());
    }
}
```

---

## Abstract Classes vs Interfaces

Stratos uses interfaces for defining contracts. If you need shared implementation, use a base class with methods that can be overridden.

### Interface (Pure Contract)
```stratos
interface Animal {
    fn makeSound() unit;
    fn move() unit;
}

class Dog : Animal {
    fn makeSound() { print("Woof"); }
    fn move() { print("Running"); }
}
```

### Base Class (Shared Implementation)
```stratos
class Animal {
    var name: string;

    constructor(n: string) {
        this.name = n;
    }

    fn makeSound() {
        print("Generic animal sound");
    }

    fn introduce() {
        print("I am " + this.name);
    }
}

class Dog : Animal {
    constructor(n: string) {
        super(n);
    }

    override fn makeSound() {
        print("Woof");
    }

    // Inherits introduce() from Animal
}
```

---

## Best Practices

### ✅ DO:
- Keep inheritance hierarchies shallow (prefer composition over deep inheritance)
- Use interfaces for defining contracts
- Use `override` keyword when overriding methods
- Call `super()` in constructors when needed
- Prefer composition over inheritance when possible

### ❌ DON'T:
- Try to use multiple class inheritance (not supported)
- Inherit from structs (not supported)
- Forget to implement all interface methods
- Create deep inheritance hierarchies (>3 levels usually indicates bad design)

---

## Summary

- **Classes** support single inheritance for code reuse and polymorphism
- **Interfaces** define contracts and support multiple implementation
- **Structs** are for plain data and do NOT support inheritance
- Use the `override` keyword when overriding parent methods
- Use `super()` to call parent constructors
- Prefer interfaces for contracts, classes for shared implementation

See also: `README.md` for guidance on when to use class vs struct vs interface
