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
    println("hello");
    val y = add(5, 5);
    println(y);


        println("=== Basic Math Example ===");
    println("");

    // Basic arithmetic
    val a = 10;
    val b = 5;
    println("a = 10, b = 5");
    println("a + b =");
    println(a + b);
    println("a - b =");
    println(a - b);
    println("a * b =");
    println(a * b);
    println("");

    // Working with doubles
    val x = 10.5;
    val y2 = 2.0;
    println("x = 10.5, y2 = 2.0");
    println("x + y2 =");
    println(x + y2);
    println("x * y2 =");
    println(x * y2);
    println("");

    // Factorial calculation
    println("Factorial of 5:");
    val n = 5;
    var result = 1;
    var i = 1;
    while i <= n {
        result = result * i;
        i = i + 1;
    }
    println(result);
    

    val z = math.random();
    println("random number {} PI {}", z, math.PI);
}