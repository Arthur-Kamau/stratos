package main;

use math;

fn add(a: int, b: int) int {
    return a + b;
}

fn multiply(a: int, b: int) int {
    return a * b;
}

fn square(x: double) double {
    return x * x;
}

fn main() {
    println("hellp");
    val y = add(5, 5);
    println(y);

    val z = math.random();
    println("random number {} PI {}", z, math.PI);
}