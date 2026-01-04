// Example: Basic Math Operations
// Demonstrates basic arithmetic and math operations

package main;

fn main() {
    println("=== Basic Math Example ===");
    println("");

    // Basic arithmetic
    val a = 10;
    val b = 5;
    println("a = 10, b = 5");
    println("a + b =");
    println(a + b);
    println("a - b =");
    println(a - b);
    println("a * b =");
    println(a * b);
    println("");

    // Working with doubles
    val x = 10.5;
    val y = 2.0;
    println("x = 10.5, y = 2.0");
    println("x + y =");
    println(x + y);
    println("x * y =");
    println(x * y);
    println("");

    // Factorial calculation
    println("Factorial of 5:");
    val n = 5;
    var result = 1;
    var i = 1;
    while i <= n {
        result = result * i;
        i = i + 1;
    }
    println(result);
}
