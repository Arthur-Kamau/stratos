// Example: Basic Math Operations
// Demonstrates basic arithmetic and math operations

package main;

fn main() {
    print("=== Basic Math Example ===");
    print("");

    // Basic arithmetic
    val a = 10;
    val b = 5;
    print("a = 10, b = 5");
    print("a + b =");
    print(a + b);
    print("a - b =");
    print(a - b);
    print("a * b =");
    print(a * b);
    print("");

    // Working with doubles
    val x = 10.5;
    val y = 2.0;
    print("x = 10.5, y = 2.0");
    print("x + y =");
    print(x + y);
    print("x * y =");
    print(x * y);
    print("");

    // Factorial calculation
    print("Factorial of 5:");
    val n = 5;
    var result = 1;
    var i = 1;
    while (i <= n) {
        result = result * i;
        i = i + 1;
    }
    print(result);
}
