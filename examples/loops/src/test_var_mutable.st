package main;

fn main() {
    println("Test: Mutable variables");

    var x = 5;
    var y = 10;
    println("x = " + x);
    println("y = " + y);

    println("\nComparing x < y");
    if x < y {
        println("x < y: TRUE");
    }

    println("Test completed!");
}
