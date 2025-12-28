package main;

// Simple class without interface inheritance
class Circle {
    var radius: double;

    constructor(r: double) {
        this.radius = r;
    }

    fn area() double {
        return 3.14159 * this.radius * this.radius;
    }

    fn getRadius() double {
        return this.radius;
    }
}

fn main() {
    val circle = Circle(5.0);
    print("Radius:");
    print(circle.getRadius());
    print("Area:");
    print(circle.area());
}
