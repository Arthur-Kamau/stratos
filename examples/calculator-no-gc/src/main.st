package main;

// Simple calculator example demonstrating Stratos without GC
// This project has garbage collection disabled in stratos.conf

fn add(a: int, b: int): int {
    return a + b;
}

fn subtract(a: int, b: int): int {
    return a - b;
}

fn multiply(a: int, b: int): int {
    return a * b;
}

fn divide(a: int, b: int): int {
    if b == 0 {
        println("Error: Division by zero!");
        return 0;
    }
    return a / b;
}

fn modulo(a: int, b: int): int {
    if b == 0 {
        println("Error: Modulo by zero!");
        return 0;
    }
    return a % b;
}

fn power(base: int, exp: int): int {
    if exp == 0 {
        return 1;
    }
    var result = 1;
    for i in 0..exp {
        result = result * base;
    }
    return result;
}

fn factorial(n: int): int {
    if n <= 1 {
        return 1;
    }
    var result = 1;
    for i in 2..(n + 1) {
        result = result * i;
    }
    return result;
}

fn printResult(operation: string, result: int) {
    println("${operation} = ${result}");
}

fn main() {
    println("=== Calculator Demo (No GC) ===");
    println("");

    // Basic arithmetic
    val a = 15;
    val b = 4;

    println("Operating on a=${a}, b=${b}:");
    println("");

    printResult("${a} + ${b}", add(a, b));
    printResult("${a} - ${b}", subtract(a, b));
    printResult("${a} * ${b}", multiply(a, b));
    printResult("${a} / ${b}", divide(a, b));
    printResult("${a} % ${b}", modulo(a, b));

    println("");
    println("Power operations:");
    printResult("2^10", power(2, 10));
    printResult("3^5", power(3, 5));

    println("");
    println("Factorials:");
    printResult("5!", factorial(5));
    printResult("7!", factorial(7));
    printResult("10!", factorial(10));

    println("");
    println("=== Done ===");
}
