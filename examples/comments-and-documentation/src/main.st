/**
 * Main package for the Documentation Example.
 *
 * This package demonstrates how to use Stratos' built-in documentation
 * features including:
 * - Documentation comments (start with /** and end with * /)
 * - Tags (@param, @return, @example, etc.)
 * - Markdown formatting
 *
 * @since 1.0.0
 */
package main;

use math_utils;
use user;
use geometry;

/**
 * Main entry point for the documentation demo.
 *
 * This function orchestrates the demonstration of various
 * documented components in this project.
 *
 * @example
 * // Run the project
 * main();
 */
fn main() {
    println("=== Stratos Documentation Demo ===\n");

    // 1. Using documented math functions
    println("--- Math Utilities ---");
    val result = factorial(5);
    println("Factorial of 5 is: " + result);

    val primeCheck = isPrime(17);
    println("Is 17 prime? " + primeCheck);

    val gcdVal = gcd(48, 18);
    println("GCD of 48 and 18: " + gcdVal);

    if (isValidEmail("test@example.com")) {
        println("Email validation check passed");
    }
    println("");

    // 2. Using documented geometry classes
    println("--- Geometry ---");
    val circle = Circle(0.0, 0.0, 5.0);
    println("Circle(radius=5) Area: " + circle.area());
    println("Circle Circumference: " + circle.circumference());

    val rect = Rectangle(0.0, 0.0, 10.0, 5.0);
    println("Rectangle(10x5) Area: " + rect.area());
    println("Rectangle Perimeter: " + rect.perimeter());
    println("");

    // 3. Using documented user class
    println("--- User System ---");
    val alice = User("Alice", "alice@example.com");
    if (alice.login("password123")) {
        println("User " + alice.name + " logged in successfully.");
    }
}
