// 05_oop.st
// Classes, Interfaces, Inheritance

// Interface - defines a contract
interface Shape {
    fn area() double;
    fn perimeter() double;
}

// Base class implementing interface
class Rectangle : Shape {
    var width: double;
    var height: double;

    constructor(w: double, h: double) {
        this.width = w;
        this.height = h;
    }

    fn area() double {
        return this.width * this.height;
    }

    fn perimeter() double {
        return 2.0 * (this.width + this.height);
    }
}

// Derived class inheriting from Rectangle
class Square : Rectangle {
    constructor(side: double) {
        super(side, side);  // Call parent constructor
    }

    // Override to provide specialized implementation
    override fn perimeter() double {
        return 4.0 * this.width;
    }
}

// Another class implementing the same interface
class Circle : Shape {
    var radius: double;

    constructor(r: double) {
        this.radius = r;
    }

    fn area() double {
        return 3.14159 * this.radius * this.radius;
    }

    fn perimeter() double {
        return 2.0 * 3.14159 * this.radius;
    }
}

// Usage - polymorphism
val rect = Rectangle(10.0, 5.0);
val square = Square(7.0);
val circle = Circle(3.0);

print(rect.area());      // 50.0
print(square.area());    // 49.0
print(circle.area());    // 28.27
