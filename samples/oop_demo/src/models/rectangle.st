package models;

// Rectangle class implementing Shape
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

    fn getWidth() double {
        return this.width;
    }
}
