/**
 * Doc Example - Demonstration of Stratos Documentation Generator
 *
 * This is the main entry point for the documentation example project.
 * It demonstrates various geometric and mathematical utilities that
 * are fully documented using Stratos doc comments.
 *
 * @since 1.0.0
 */
package main;

use mathutils;
use geometry;

/**
 * Main entry point for the application.
 *
 * Demonstrates the usage of documented functions and classes
 * from the mathutils and geometry modules.
 */
fn main() {
    println("=== Stratos Documentation Example ===\n");

    // Demonstrate statistical functions
    println("Statistical Functions:");
    val numbers = [2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0];
    println("Numbers: [2, 4, 4, 4, 5, 5, 7, 9]");
    // Note: Array syntax may vary in actual implementation

    // Demonstrate number theory
    println("\nNumber Theory:");
    println("Is 17 prime? ");
    val prime17 = mathutils.isPrime(17);
    println(prime17);

    println("Factorial of 5: ");
    val fact5 = mathutils.factorial(5);
    println(fact5);

    println("GCD of 48 and 18: ");
    val gcdResult = mathutils.gcd(48, 18);
    println(gcdResult);

    // Demonstrate geometry
    println("\nGeometry:");
    val circle = geometry.Circle(0.0, 0.0, 5.0);
    println("Circle with radius 5:");
    println("  Area: ");
    // println(circle.area());

    val rect = geometry.Rectangle(0.0, 0.0, 10.0, 5.0);
    println("Rectangle 10x5:");
    println("  Area: ");
    // println(rect.area());

    println("\n=== Example Complete ===");
}
