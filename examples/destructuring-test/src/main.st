// Destructuring Declaration Example
// Demonstrates Kotlin-style destructuring syntax

package main;

fn main() {
    println("=== Destructuring Declaration Demo ===");
    println("");

    // Basic array destructuring
    println("1. Basic array destructuring:");
    val numbers = [10, 20, 30];
    val (a, b, c) = numbers;
    println("  Array: [10, 20, 30]");
    println("  a = $a");
    println("  b = $b");
    println("  c = $c");
    println("");

    // Destructuring from range
    println("2. Destructuring from range:");
    val (first, second, third) = 1..4;
    println("  Range: 1..4");
    println("  first = $first");
    println("  second = $second");
    println("  third = $third");
    println("");

    // Partial destructuring (more variables than elements)
    println("3. Destructuring with fewer elements:");
    val small = [100, 200];
    val (x, y, z) = small;
    println("  Array: [100, 200]");
    println("  x = $x");
    println("  y = $y");
    println("  z = $z (default: 0)");
    println("");

    // Using destructured values
    println("4. Using destructured values in calculation:");
    val coords = [3, 4];
    val (dx, dy) = coords;
    var distance = dx * dx + dy * dy;
    println("  Coordinates: [3, 4]");
    println("  dx^2 + dy^2 = $distance");
    println("");

    println("=== Destructuring Demo Complete ===");
}
