package main;

fn main() {
    println("Test: Boolean literal");

    if true {
        println("TRUE branch works!");
    }

    if false {
        println("FALSE branch - should NOT print");
    } else {
        println("ELSE branch works!");
    }

    println("Test completed!");
}
