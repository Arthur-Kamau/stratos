// 06_packages.st
// Packages for Code Organization (Go-style)

// Package declaration at top of file
// In a real project, this would be in math/operations.st
package math

fn add(a: int, b: int) int {
    return a + b;
}

fn multiply(a: int, b: int) int {
    return a * b;
}

fn abs(x: int) int {
    if (x < 0) {
        return -x;
    }
    return x;
}

// Struct in package
struct Point {
    x: double,
    y: double
}

fn distance(p1: Point, p2: Point) double {
    val dx = p1.x - p2.x;
    val dy = p1.y - p2.y;
    return 0.0; // sqrt(dx*dx + dy*dy) - simplified
}

// This demonstrates what main.st would look like:
// package main
//
// use math;
//
// fn main() {
//     val sum = math.add(10, 20);
//     val product = math.multiply(5, 6);
//     val absValue = math.abs(-15);
//
//     print(sum);       // 30
//     print(product);   // 30
//     print(absValue);  // 15
// }

// Note: In Go-style packages:
// - One directory = one package
// - All .st files in same directory declare same package
// - Package name should match directory name
// - Use 'use' keyword to import packages
// - Access with qualified names: packagename.function()
