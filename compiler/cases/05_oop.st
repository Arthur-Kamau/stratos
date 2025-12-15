// 05_oop.st
// Classes, Interfaces, Inheritance

interface Shape {
    fn area() double;
}

class Rectangle : Shape {
    var width: double;
    var height: double;

    constructor(w: double, h: double) {
        this.width = w;
        this.height = h;
    }

    override fn area() double {
        return this.width * this.height;
    }
}

class Square : Rectangle {
    constructor(side: double) {
        // Assuming super call syntax, though not explicitly defined in basic docs yet
        // standardizing on super(args) or similar
        // super(side, side); 
        this.width = side;
        this.height = side;
    }
}
