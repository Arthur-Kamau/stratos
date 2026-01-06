package main;

fn main() {
    println("Test: Compare two literals");

    if 2 < 100 {
        println("2 < 100: TRUE (correct!)");
    }

    if 100 < 2 {
        println("100 < 2: TRUE (WRONG!)");
    } else {
        println("100 < 2: FALSE (correct!)");
    }

    println("Test completed!");
}
