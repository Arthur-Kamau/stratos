interface Shape {
    fn area() double;
}

class Rectangle : Shape {
    fn area() double {
        return 50.0;
    }
}

val rect = Rectangle(10.0, 5.0);
print(42);
