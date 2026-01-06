package main;

fn main() {
    println("Test: While with 100");

    var value = 2;
    while value < 100 {
        println("value = " + value);
        value = value + 10;
    }

    println("Test completed!");
}
