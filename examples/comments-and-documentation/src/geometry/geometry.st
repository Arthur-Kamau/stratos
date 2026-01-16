package geometry;

use math;

class Circle {
    var x: double;
    var y: double;
    var radius: double;

    constructor(centerX: double, centerY: double, r: double) {
        this.x = centerX;
        this.y = centerY;
        this.radius = r;
    }

    pub fn area() double {
        return math.PI * this.radius * this.radius;
    }

    pub fn circumference() double {
        return 2.0 * math.PI * this.radius;
    }

    pub fn contains(px: double, py: double) bool {
        val dx = px - this.x;
        val dy = py - this.y;
        val distSquared = dx * dx + dy * dy;
        return distSquared <= (this.radius * this.radius);
    }
}

class Rectangle {
    var x: double;
    var y: double;
    var width: double;
    var height: double;

    constructor(topLeftX: double, topLeftY: double, w: double, h: double) {
        this.x = topLeftX;
        this.y = topLeftY;
        this.width = w;
        this.height = h;
    }

    pub fn area() double {
        return this.width * this.height;
    }

    pub fn perimeter() double {
        return 2.0 * (this.width + this.height);
    }

    pub fn contains(px: double, py: double) bool {
        return px >= this.x && px <= (this.x + this.width) &&
               py >= this.y && py <= (this.y + this.height);
    }

    pub fn diagonal() double {
        return math.sqrt(this.width * this.width + this.height * this.height);
    }
}

class Triangle {
    var x1: double;
    var y1: double;
    var x2: double;
    var y2: double;
    var x3: double;
    var y3: double;

    constructor(ax: double, ay: double, bx: double, by: double, cx: double, cy: double) {
        this.x1 = ax;
        this.y1 = ay;
        this.x2 = bx;
        this.y2 = by;
        this.x3 = cx;
        this.y3 = cy;
    }

    pub fn area() double {
        val area = 0.5 * math.abs(
            this.x1 * (this.y2 - this.y3) +
            this.x2 * (this.y3 - this.y1) +
            this.x3 * (this.y1 - this.y2)
        );
        return area;
    }
}
