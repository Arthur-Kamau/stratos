// Simple Demo - No stdlib dependencies
// This example works without requiring standard library modules

package main;

fn add(a: int, b: int) int {
    return a + b;
}

fn multiply(a: int, b: int) int {
    return a * b;
}

fn factorial(n: int) int {
    if n <= 1 {
        return 1;
    }
    return n * factorial(n - 1);
}

fn main() {
    println("=== Simple Math Demo ===");

    // Basic arithmetic
    val x = 10;
    val y = 20;
    println("x = " + x);
    println("y = " + y);
    println("x + y = " + add(x, y));
    println("x * y = " + multiply(x, y));

    // Factorial
    val n = 5;
    println("factorial(" + n + ") = " + factorial(n));

    // Control flow
    println("\nCounting to 5:");
    var i = 1;
    while i <= 5 {
        println(i);
        i = i + 1;
    }

    println("\nDemo complete!");
}
