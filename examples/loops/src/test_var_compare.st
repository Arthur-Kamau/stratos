package main;

fn main() {
    println("Test: Compare variables");

    val x = 5;
    val y = 10;

    println("x = " + x);
    println("y = " + y);

    if x < y {
        println("x < y: TRUE (correct!)");
    }

    if y < x {
        println("y < x: TRUE (WRONG!)");
    } else {
        println("y < x: FALSE (correct!)");
    }

    println("Test completed!");
}
