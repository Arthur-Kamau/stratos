package main;

fn main() {
    println("=== Debug Test ===");

    println("Step 1: Create variables");
    val x = 5;
    val y = 10;
    println("x = " + x);
    println("y = " + y);

    println("\nStep 2: Test arithmetic (uses similar code path)");
    val sum = x + y;
    println("x + y = " + sum);

    println("\nStep 3: Test comparison with literal");
    if 5 < 10 {
        println("5 < 10: TRUE");
    }

    println("\nStep 4: Test comparison with one variable");
    if x < 10 {
        println("x < 10: TRUE");
    } else {
        println("x < 10: FALSE");
    }

    println("\nStep 5: Test comparison with both variables");
    if x < y {
        println("x < y: TRUE");
    } else {
        println("x < y: FALSE");
    }

    println("\n=== Test Complete ===");
}
