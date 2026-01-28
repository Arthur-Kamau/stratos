// String Interpolation Example
// Demonstrates Kotlin-style string interpolation with $var and ${expr}

package main;

fn main() {
    println("=== String Interpolation Demo ===");

    // Basic variable interpolation
    val name = "Stratos";
    val version = "1.0";
    println("Welcome to $name version $version!");

    // Expression interpolation
    val x = 10;
    val y = 20;
    println("The sum of $x and $y is ${x + y}");

    // Mixed interpolation
    val firstName = "John";
    val lastName = "Doe";
    val age = 30;
    println("$firstName $lastName is $age years old");

    // Interpolation with method calls
    val greeting = "hello";
    println("Uppercase: ${greeting.toUpper()}");

    // Nested expressions
    val a = 5;
    val b = 3;
    println("Math: $a * $b = ${a * b}, $a + $b = ${a + b}");

    // Boolean interpolation
    val isActive = true;
    println("Status: active = $isActive");

    // Escaping $ with backslash
    println("Price: \$100 (escaped dollar sign)");

    // Complex expressions
    val items = 5;
    val price = 10;
    println("Total: $items items at \$$price each = \$${items * price}");

    println("");
    println("=== String Interpolation Complete ===");
}
