// Stratos Interpreter Demo
// This program demonstrates the interpreter executing Stratos code directly
package main;

use math;
use strings;
use log;

fn square(x: int) int {
    return x * x;
}

fn main() {
    log.info("=== Stratos Interpreter Demo ===");

    // Test 1: Basic arithmetic
    val a = 10;
    val b = 20;
    val sum = a + b;
    log.info("10 + 20 = " + sum);

    // Test 2: Math functions
    val sqrtResult = math.sqrt(144.0);
    log.info("sqrt(144) = " + sqrtResult);

    val sinResult = math.sin(0.0);
    log.info("sin(0) = " + sinResult);

    // Test 3: String functions
    val message = "hello world";
    val upper = strings.toUpper(message);
    log.info("toUpper('hello world') = " + upper);

    val len = strings.length(message);
    log.info("length('hello world') = " + len);

    // Test 4: User-defined functions
    val squared = square(7);
    log.info("square(7) = " + squared);

    log.info("=== Demo Complete! ===");
}
