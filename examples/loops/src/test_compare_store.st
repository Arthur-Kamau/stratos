package main;

fn main() {
    println("Test: Store comparison result");

    val x = 5;
    val y = 10;
    println("x = " + x);
    println("y = " + y);

    println("\nAbout to compare...");
    val result = x < y;
    println("Comparison done!");
    println("result = " + result);

    println("Test completed!");
}
