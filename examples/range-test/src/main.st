// Range Expression Example
// Demonstrates Kotlin/Go-style range syntax with ..

package main;

fn main() {
    println("=== Range Expression Demo ===");
    println("");

    // Basic range iteration
    println("1. Basic range (0..5):");
    for val i in 0..5 {
        println("  i = $i");
    }
    println("");

    // Range with different start
    println("2. Range from 3 to 8 (3..8):");
    for val n in 3..8 {
        println("  n = $n");
    }
    println("");

    // Using range for sum calculation
    println("3. Sum of 1..6:");
    var sum = 0;
    for val i in 1..6 {
        sum = sum + i;
    }
    println("  Sum = $sum");
    println("");

    // Nested ranges
    println("4. Multiplication table (1..4 x 1..4):");
    for val i in 1..4 {
        for val j in 1..4 {
            var product = i * j;
            println("  $i x $j = $product");
        }
    }
    println("");

    println("=== Range Demo Complete ===");
}
