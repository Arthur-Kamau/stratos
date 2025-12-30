---
title: Object-Oriented Programming
description: Classes, interfaces, and inheritance in Stratos
---

# Object-Oriented Programming

Stratos supports object-oriented programming with classes, interfaces, and inheritance. Learn how to structure your code using OOP principles.

## Classes

### Basic Class Definition

```stratos
class Person {
    var name: string;
    var age: int;

    constructor(name: string, age: int) {
        this.name = name;
        this.age = age;
    }

    fn greet() {
        print("Hello, I'm " + this.name);
    }
}

fn main() {
    val person = Person("Alice", 30);
    person.greet();  // Hello, I'm Alice
    print(person.age);  // 30
}
```

### Class with Methods

```stratos
class Rectangle {
    var width: double;
    var height: double;

    constructor(width: double, height: double) {
        this.width = width;
        this.height = height;
    }

    fn area() double {
        return this.width * this.height;
    }

    fn perimeter() double {
        return 2.0 * (this.width + this.height);
    }

    fn isSquare() bool {
        return this.width == this.height;
    }
}

fn main() {
    val rect = Rectangle(10.0, 5.0);

    print("Area: " + rect.area());           // 50.0
    print("Perimeter: " + rect.perimeter()); // 30.0
    print("Is square: " + rect.isSquare());  // false

    val square = Rectangle(7.0, 7.0);
    print("Is square: " + square.isSquare());  // true
}
```

## Interfaces

Interfaces define contracts that classes must implement:

```stratos
interface Shape {
    fn area() double;
    fn perimeter() double;
}

class Circle : Shape {
    var radius: double;

    constructor(radius: double) {
        this.radius = radius;
    }

    fn area() double {
        return 3.14159 * this.radius * this.radius;
    }

    fn perimeter() double {
        return 2.0 * 3.14159 * this.radius;
    }
}

class Rectangle : Shape {
    var width: double;
    var height: double;

    constructor(width: double, height: double) {
        this.width = width;
        this.height = height;
    }

    fn area() double {
        return this.width * this.height;
    }

    fn perimeter() double {
        return 2.0 * (this.width + this.height);
    }
}

fn printShapeInfo(shape: Shape) {
    print("Area: " + shape.area());
    print("Perimeter: " + shape.perimeter());
}

fn main() {
    val circle = Circle(5.0);
    val rectangle = Rectangle(10.0, 5.0);

    print("Circle:");
    printShapeInfo(circle);

    print("\nRectangle:");
    printShapeInfo(rectangle);
}
```

## Inheritance

Classes can inherit from other classes:

```stratos
class Animal {
    var name: string;
    var age: int;

    constructor(name: string, age: int) {
        this.name = name;
        this.age = age;
    }

    fn speak() {
        print(this.name + " makes a sound");
    }

    fn getInfo() string {
        return this.name + " (" + this.age + " years old)";
    }
}

class Dog : Animal {
    var breed: string;

    constructor(name: string, age: int, breed: string) {
        super(name, age);  // Call parent constructor
        this.breed = breed;
    }

    // Override parent method
    fn speak() {
        print(this.name + " barks!");
    }

    fn wagTail() {
        print(this.name + " wags tail happily");
    }
}

class Cat : Animal {
    var indoor: bool;

    constructor(name: string, age: int, indoor: bool) {
        super(name, age);
        this.indoor = indoor;
    }

    // Override parent method
    fn speak() {
        print(this.name + " meows!");
    }

    fn purr() {
        print(this.name + " purrs contentedly");
    }
}

fn main() {
    val dog = Dog("Max", 3, "Golden Retriever");
    val cat = Cat("Luna", 2, true);

    print(dog.getInfo());  // Max (3 years old)
    dog.speak();           // Max barks!
    dog.wagTail();         // Max wags tail happily

    print("\n");

    print(cat.getInfo());  // Luna (2 years old)
    cat.speak();           // Luna meows!
    cat.purr();            // Luna purrs contentedly
}
```

## Encapsulation

### Public and Private Members

```stratos
class BankAccount {
    var owner: string;           // Public by default
    private var balance: double; // Private field

    constructor(owner: string, initialBalance: double) {
        this.owner = owner;
        this.balance = initialBalance;
    }

    // Public methods to access private data
    fn getBalance() double {
        return this.balance;
    }

    fn deposit(amount: double) {
        if (amount > 0.0) {
            this.balance += amount;
            print("Deposited: $" + amount);
        }
    }

    fn withdraw(amount: double) bool {
        if (amount > 0.0 && amount <= this.balance) {
            this.balance -= amount;
            print("Withdrawn: $" + amount);
            return true;
        }
        print("Insufficient funds");
        return false;
    }
}

fn main() {
    val account = BankAccount("Alice", 1000.0);

    print("Owner: " + account.owner);
    print("Balance: $" + account.getBalance());

    account.deposit(500.0);
    account.withdraw(200.0);
    account.withdraw(2000.0);  // Insufficient funds

    print("Final balance: $" + account.getBalance());
}
```

## Polymorphism

Objects of different classes can be used interchangeably through interfaces:

```stratos
interface Drawable {
    fn draw();
}

class Circle : Drawable {
    var radius: double;

    constructor(radius: double) {
        this.radius = radius;
    }

    fn draw() {
        print("Drawing circle with radius " + this.radius);
    }
}

class Square : Drawable {
    var side: double;

    constructor(side: double) {
        this.side = side;
    }

    fn draw() {
        print("Drawing square with side " + this.side);
    }
}

class Triangle : Drawable {
    var base: double;
    var height: double;

    constructor(base: double, height: double) {
        this.base = base;
        this.height = height;
    }

    fn draw() {
        print("Drawing triangle: base=" + this.base + ", height=" + this.height);
    }
}

fn renderShapes(shapes: Array<Drawable>) {
    for (shape in shapes) {
        shape.draw();
    }
}

fn main() {
    val shapes = [
        Circle(5.0),
        Square(10.0),
        Triangle(8.0, 6.0),
        Circle(3.0)
    ];

    renderShapes(shapes);
}
```

### Output

```
Drawing circle with radius 5.0
Drawing square with side 10.0
Drawing triangle: base=8.0, height=6.0
Drawing circle with radius 3.0
```

## Static Members

Classes can have static members that belong to the class rather than instances:

```stratos
class MathUtils {
    static val PI: double = 3.14159;

    static fn square(x: double) double {
        return x * x;
    }

    static fn circleArea(radius: double) double {
        return MathUtils.PI * radius * radius;
    }
}

fn main() {
    print("PI = " + MathUtils.PI);
    print("Square of 5 = " + MathUtils.square(5.0));
    print("Circle area (r=3) = " + MathUtils.circleArea(3.0));
}
```

## Properties (Getters and Setters)

```stratos
class Temperature {
    private var celsius: double;

    constructor(celsius: double) {
        this.celsius = celsius;
    }

    // Getter
    fn getCelsius() double {
        return this.celsius;
    }

    // Setter with validation
    fn setCelsius(value: double) {
        if (value >= -273.15) {  // Absolute zero
            this.celsius = value;
        } else {
            print("Invalid temperature: below absolute zero");
        }
    }

    // Computed property
    fn getFahrenheit() double {
        return this.celsius * 9.0 / 5.0 + 32.0;
    }

    fn setFahrenheit(value: double) {
        this.celsius = (value - 32.0) * 5.0 / 9.0;
    }
}

fn main() {
    val temp = Temperature(25.0);

    print("Celsius: " + temp.getCelsius());
    print("Fahrenheit: " + temp.getFahrenheit());

    temp.setFahrenheit(100.0);
    print("After setting to 100°F:");
    print("Celsius: " + temp.getCelsius());

    temp.setCelsius(-300.0);  // Invalid
}
```

## Complete Example: Library System

```stratos
package main;

interface Borrowable {
    fn borrow(borrower: string) bool;
    fn returnItem() bool;
    fn isAvailable() bool;
}

class LibraryItem : Borrowable {
    var title: string;
    var id: string;
    private var borrowedBy: string?;

    constructor(title: string, id: string) {
        this.title = title;
        this.id = id;
        this.borrowedBy = null;
    }

    fn borrow(borrower: string) bool {
        if (this.isAvailable()) {
            this.borrowedBy = Some(borrower);
            print(this.title + " borrowed by " + borrower);
            return true;
        }
        print(this.title + " is not available");
        return false;
    }

    fn returnItem() bool {
        if (!this.isAvailable()) {
            print(this.title + " returned");
            this.borrowedBy = None;
            return true;
        }
        print(this.title + " was not borrowed");
        return false;
    }

    fn isAvailable() bool {
        return this.borrowedBy == null;
    }

    fn getStatus() string {
        return this.borrowedBy ?? "Available";
    }
}

class Book : LibraryItem {
    var author: string;
    var isbn: string;
    var pages: int;

    constructor(title: string, id: string, author: string, isbn: string, pages: int) {
        super(title, id);
        this.author = author;
        this.isbn = isbn;
        this.pages = pages;
    }

    fn getInfo() string {
        return "Book: " + this.title + " by " + this.author +
               " (" + this.pages + " pages, ISBN: " + this.isbn + ")";
    }
}

class DVD : LibraryItem {
    var director: string;
    var duration: int;  // minutes

    constructor(title: string, id: string, director: string, duration: int) {
        super(title, id);
        this.director = director;
        this.duration = duration;
    }

    fn getInfo() string {
        return "DVD: " + this.title + " directed by " + this.director +
               " (" + this.duration + " minutes)";
    }
}

class Library {
    var name: string;
    var items: Array<LibraryItem>;

    constructor(name: string) {
        this.name = name;
        this.items = [];
    }

    fn addItem(item: LibraryItem) {
        this.items.push(item);
        print("Added: " + item.title);
    }

    fn listAvailable() {
        print("\nAvailable items in " + this.name + ":");
        for (item in this.items) {
            if (item.isAvailable()) {
                print("  - " + item.title + " [" + item.id + "]");
            }
        }
    }

    fn listBorrowed() {
        print("\nBorrowed items:");
        for (item in this.items) {
            if (!item.isAvailable()) {
                print("  - " + item.title + " (borrowed by: " + item.getStatus() + ")");
            }
        }
    }
}

fn main() {
    val library = Library("City Library");

    // Add items
    val book1 = Book("The Pragmatic Programmer", "B001", "Hunt & Thomas", "978-0201616224", 352);
    val book2 = Book("Clean Code", "B002", "Robert Martin", "978-0132350884", 464);
    val dvd1 = DVD("The Matrix", "D001", "Wachowskis", 136);

    library.addItem(book1);
    library.addItem(book2);
    library.addItem(dvd1);

    // List available
    library.listAvailable();

    // Borrow items
    print("\nBorrowing items:");
    book1.borrow("Alice");
    dvd1.borrow("Bob");
    book2.borrow("Charlie");

    // Check status
    library.listAvailable();
    library.listBorrowed();

    // Return items
    print("\nReturning items:");
    book1.returnItem();

    // Final status
    library.listAvailable();
    library.listBorrowed();
}
```

## Best Practices

::: tip
**Single Responsibility**: Each class should have one clear purpose. If a class is doing too much, split it into smaller classes.
:::

::: tip
**Favor composition over inheritance**: Use interfaces and composition when possible. Deep inheritance hierarchies can be hard to maintain.
:::

::: tip
**Encapsulation**: Keep fields private and provide public methods to access them. This protects internal state and allows validation.
:::

::: info
**Interface naming**: Interfaces often describe capabilities. Use adjectives ending in "-able" (Drawable, Comparable, Serializable).
:::

::: warning
**Avoid god objects**: Don't create massive classes that do everything. Keep classes focused and cohesive.
:::

## OOP Quick Reference

| Concept | Syntax | Purpose |
|---------|--------|---------|
| Class | `class Name { ... }` | Define a type with data and behavior |
| Constructor | `constructor(params) { ... }` | Initialize new instances |
| Method | `fn methodName() { ... }` | Define behavior |
| Interface | `interface Name { ... }` | Define a contract |
| Inheritance | `class Child : Parent` | Extend a class |
| Implementation | `class Type : Interface` | Implement an interface |
| Super call | `super(args)` | Call parent constructor |
| Private | `private var field` | Restrict access |
| Static | `static fn method()` | Class-level member |
| This | `this.field` | Reference current instance |

## Next Steps


