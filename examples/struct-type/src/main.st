package main;

struct Point {
    x: int;
    y: int;
}
struct Point3D {
    x: int;
    y: int;
    z: int;
}

struct Color {
    r: int;
    g: int;
    b: int;
}

struct Color3D {
    r: int;
    g: int;
    b: int;
    a: int;
}

struct mouse {
    point: Point;
    color: Color;
}
struct screen{
    width: int;
    height: int;
    color: Color;
    mouse: mouse;
    pixels: array<int>;
    data: array<Point3D>;

}
fn main() {
    println("--- Struct Demo ---");
    println("Structs defined successfully.");

    val p = Point { x: 1, y: 2 };
    println("Point: ({}, {})", p.x, p.y);

    val m = mouse { point: Point { x: 0, y: 0 }, color: Color { r: 255, g: 255, b: 255 } };
    println("Mouse: ({}, {})", m.point.x, m.point.y);

    val s = screen { width: 1024, height: 768, color: Color { r: 255, g: 255, b: 255 }, mouse: m, pixels: [1, 2, 3, 4, 5], data: [] };
    println("Screen: ({}, {})", s.width, s.height);
    println("Mouse: ({}, {})", s.mouse.point.x, s.mouse.point.y);
    println("Mouse Color: ({}, {}, {})", s.mouse.color.r, s.mouse.color.g, s.mouse.color.b);
}