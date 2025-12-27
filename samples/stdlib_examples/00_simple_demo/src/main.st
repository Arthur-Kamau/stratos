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
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

fn main() {
    print("=== Simple Math Demo ===");

    // Basic arithmetic
    val x = 10;
    val y = 20;
    print("x = " + x);
    print("y = " + y);
    print("x + y = " + add(x, y));
    print("x * y = " + multiply(x, y));

    // Factorial
    val n = 5;
    print("factorial(" + n + ") = " + factorial(n));

    // Control flow
    print("\nCounting to 5:");
    var i = 1;
    while (i <= 5) {
        print(i);
        i = i + 1;
    }

    print("\nDemo complete!");
}
