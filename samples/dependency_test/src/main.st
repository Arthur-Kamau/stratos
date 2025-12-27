package main;

use sample;

fn main() {
    println("=== Testing Stratos Dependency Management ===\n");

    // Test greet function
    val greeting = sample.greet("Stratos User");
    println(greeting);

    // Test add function
    val sum = sample.add(10, 20);
    println("10 + 20 = " + sum);

    // Test multiply function
    val product = sample.multiply(5, 6);
    println("5 * 6 = " + product);

    // Test factorial function
    val fact5 = sample.factorial(5);
    println("5! = " + fact5);

    // Test max function
    val maximum = sample.max(42, 17);
    println("max(42, 17) = " + maximum);

    // Test min function
    val minimum = sample.min(42, 17);
    println("min(42, 17) = " + minimum);

    println("\n=== All tests completed successfully! ===");
}
