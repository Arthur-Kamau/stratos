package main;

fn main() {
    println("Test: Mixed literal and variable");

    val x = 5;
    println("x = " + x);

    println("\nTest 1: x < 10 (variable < literal)");
    if x < 10 {
        println("x < 10: TRUE");
    }

    println("\nTest 2: 3 < x (literal < variable)");
    if 3 < x {
        println("3 < x: TRUE");
    }

    println("Test completed!");
}
