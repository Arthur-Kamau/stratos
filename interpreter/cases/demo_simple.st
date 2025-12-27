// Simple Stratos Interpreter Demo
package main;

use log;

fn square(x: int) int {
    return x * x;
}

fn main() {
    log.info("=== Stratos Interpreter Demo ===");

    // Test 1: Basic output
    print("Hello from Stratos!");

    // Test 2: Variables and arithmetic
    val a = 10;
    val b = 20;
    val sum = a + b;
    print("10 + 20 = ");
    print(sum);

    // Test 3: User functions
    val result = square(8);
    print("square(8) = ");
    print(result);

    // Test 4: Native logging
    log.info("Logging works!");
    log.info("Demo complete!");
}
