/**
 * Geometry module - Geometric shapes and calculations
 *
 * This module provides classes and functions for working with basic
 * geometric shapes including circles, rectangles, and triangles.
 *
 * @since 1.0.0
 */
package geometry;

use math;

// ============================================================================
// Circle Class
// ============================================================================

/**
 * Represents a circle in 2D space.
 *
 * A circle is defined by a center point (x, y) and a radius.
 * This class provides methods for calculating area, circumference,
 * and checking if a point lies within the circle.
 *
 * @example
 * val circle = Circle(0.0, 0.0, 5.0);
 * val area = circle.area();  // Returns ~78.54
 * @since 1.0.0
 */
class Circle {
    /** X coordinate of circle center */
    var x: double;

    /** Y coordinate of circle center */
    var y: double;

    /** Radius of the circle (must be positive) */
    var radius: double;

    /**
     * Creates a new Circle instance.
     *
     * @param centerX X coordinate of center
     * @param centerY Y coordinate of center
     * @param r Radius of the circle
     */
    constructor(centerX: double, centerY: double, r: double) {
        this.x = centerX;
        this.y = centerY;
        this.radius = r;
    }

    /**
     * Calculates the area of the circle.
     *
     * Formula: A = πr²
     *
     * @return The area of the circle
     * @example
     * val c = Circle(0.0, 0.0, 10.0);
     * val a = c.area();  // Returns ~314.16
     */
    fn area() double {
        return math.PI * this.radius * this.radius;
    }

    /**
     * Calculates the circumference of the circle.
     *
     * Formula: C = 2πr
     *
     * @return The circumference of the circle
     * @example
     * val c = Circle(0.0, 0.0, 10.0);
     * val circ = c.circumference();  // Returns ~62.83
     */
    fn circumference() double {
        return 2.0 * math.PI * this.radius;
    }

    /**
     * Checks if a point lies within the circle.
     *
     * A point is inside the circle if its distance from the center
     * is less than or equal to the radius.
     *
     * @param px X coordinate of the point to test
     * @param py Y coordinate of the point to test
     * @return True if the point is inside or on the circle, false otherwise
     * @example
     * val c = Circle(0.0, 0.0, 5.0);
     * val inside = c.contains(3.0, 4.0);   // Returns true (distance = 5)
     * val outside = c.contains(10.0, 0.0); // Returns false
     */
    fn contains(px: double, py: double) bool {
        val dx = px - this.x;
        val dy = py - this.y;
        val distSquared = dx * dx + dy * dy;
        return distSquared <= (this.radius * this.radius);
    }
}

// ============================================================================
// Rectangle Class
// ============================================================================

/**
 * Represents a rectangle in 2D space.
 *
 * A rectangle is defined by its top-left corner (x, y) and its
 * width and height dimensions. All angles are right angles (90°).
 *
 * @example
 * val rect = Rectangle(0.0, 0.0, 10.0, 5.0);
 * val area = rect.area();  // Returns 50.0
 * @since 1.0.0
 */
class Rectangle {
    /** X coordinate of top-left corner */
    var x: double;

    /** Y coordinate of top-left corner */
    var y: double;

    /** Width of the rectangle (must be positive) */
    var width: double;

    /** Height of the rectangle (must be positive) */
    var height: double;

    /**
     * Creates a new Rectangle instance.
     *
     * @param topLeftX X coordinate of top-left corner
     * @param topLeftY Y coordinate of top-left corner
     * @param w Width of the rectangle
     * @param h Height of the rectangle
     */
    constructor(topLeftX: double, topLeftY: double, w: double, h: double) {
        this.x = topLeftX;
        this.y = topLeftY;
        this.width = w;
        this.height = h;
    }

    /**
     * Calculates the area of the rectangle.
     *
     * Formula: A = width × height
     *
     * @return The area of the rectangle
     * @example
     * val r = Rectangle(0.0, 0.0, 10.0, 5.0);
     * val a = r.area();  // Returns 50.0
     */
    fn area() double {
        return this.width * this.height;
    }

    /**
     * Calculates the perimeter of the rectangle.
     *
     * Formula: P = 2(width + height)
     *
     * @return The perimeter of the rectangle
     * @example
     * val r = Rectangle(0.0, 0.0, 10.0, 5.0);
     * val p = r.perimeter();  // Returns 30.0
     */
    fn perimeter() double {
        return 2.0 * (this.width + this.height);
    }

    /**
     * Checks if a point lies within the rectangle.
     *
     * @param px X coordinate of the point to test
     * @param py Y coordinate of the point to test
     * @return True if the point is inside or on the rectangle boundary
     * @example
     * val r = Rectangle(0.0, 0.0, 10.0, 5.0);
     * val inside = r.contains(5.0, 2.5);   // Returns true
     * val outside = r.contains(15.0, 2.5); // Returns false
     */
    fn contains(px: double, py: double) bool {
        return px >= this.x && px <= (this.x + this.width) &&
               py >= this.y && py <= (this.y + this.height);
    }

    /**
     * Calculates the diagonal length of the rectangle.
     *
     * Uses the Pythagorean theorem: diagonal = √(width² + height²)
     *
     * @return The length of the rectangle's diagonal
     * @example
     * val r = Rectangle(0.0, 0.0, 3.0, 4.0);
     * val d = r.diagonal();  // Returns 5.0
     */
    fn diagonal() double {
        return math.sqrt(this.width * this.width + this.height * this.height);
    }
}

// ============================================================================
// Triangle Class
// ============================================================================

/**
 * Represents a triangle in 2D space.
 *
 * A triangle is defined by three vertices. This implementation
 * stores the triangle as three points (x1,y1), (x2,y2), (x3,y3).
 *
 * @since 1.0.0
 */
class Triangle {
    /** X coordinate of first vertex */
    var x1: double;
    /** Y coordinate of first vertex */
    var y1: double;

    /** X coordinate of second vertex */
    var x2: double;
    /** Y coordinate of second vertex */
    var y2: double;

    /** X coordinate of third vertex */
    var x3: double;
    /** Y coordinate of third vertex */
    var y3: double;

    /**
     * Creates a new Triangle instance.
     *
     * @param ax X coordinate of first vertex
     * @param ay Y coordinate of first vertex
     * @param bx X coordinate of second vertex
     * @param by Y coordinate of second vertex
     * @param cx X coordinate of third vertex
     * @param cy Y coordinate of third vertex
     */
    constructor(ax: double, ay: double, bx: double, by: double, cx: double, cy: double) {
        this.x1 = ax;
        this.y1 = ay;
        this.x2 = bx;
        this.y2 = by;
        this.x3 = cx;
        this.y3 = cy;
    }

    /**
     * Calculates the area of the triangle.
     *
     * Uses the cross product formula:
     * A = 0.5 × |x1(y2 - y3) + x2(y3 - y1) + x3(y1 - y2)|
     *
     * @return The area of the triangle
     * @example
     * val t = Triangle(0.0, 0.0, 4.0, 0.0, 0.0, 3.0);
     * val a = t.area();  // Returns 6.0
     */
    fn area() double {
        val area = 0.5 * math.abs(
            this.x1 * (this.y2 - this.y3) +
            this.x2 * (this.y3 - this.y1) +
            this.x3 * (this.y1 - this.y2)
        );
        return area;
    }
}
