package main;

struct Point {
    x: int;
    y: int;
}

fn main() {
    println("--- Struct Demo ---");
    println("Structs defined successfully.");

    val p = Point { x: 1, y: 2 };
    println("Point: ({}, {})", p.x, p.y);
}