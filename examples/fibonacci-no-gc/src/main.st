package main;

// Fibonacci sequence example demonstrating Stratos without GC
// This project has garbage collection disabled in stratos.conf

// Recursive Fibonacci (simple but slow for large n)
fn fibRecursive(n: int): int {
    if n <= 1 {
        return n;
    }
    return fibRecursive(n - 1) + fibRecursive(n - 2);
}

// Iterative Fibonacci (efficient)
fn fibIterative(n: int): int {
    if n <= 1 {
        return n;
    }

    var prev = 0;
    var curr = 1;

    for i in 2..(n + 1) {
        val next = prev + curr;
        prev = curr;
        curr = next;
    }

    return curr;
}

// Print first n Fibonacci numbers
fn printFibSequence(count: int) {
    println("First ${count} Fibonacci numbers:");
    var prev = 0;
    var curr = 1;

    for i in 0..count {
        if i == 0 {
            print("${prev}");
        } else if i == 1 {
            print(", ${curr}");
        } else {
            val next = prev + curr;
            prev = curr;
            curr = next;
            print(", ${curr}");
        }
    }
    println("");
}

// Check if a number is a Fibonacci number
fn isFibonacci(num: int): bool {
    if num < 0 {
        return false;
    }
    if num <= 1 {
        return true;
    }

    var prev = 0;
    var curr = 1;

    while curr < num {
        val next = prev + curr;
        prev = curr;
        curr = next;
    }

    return curr == num;
}

// Find the index of a Fibonacci number
fn fibIndex(num: int): int {
    if num < 0 {
        return -1;
    }
    if num == 0 {
        return 0;
    }
    if num == 1 {
        return 1;
    }

    var prev = 0;
    var curr = 1;
    var index = 1;

    while curr < num {
        val next = prev + curr;
        prev = curr;
        curr = next;
        index = index + 1;
    }

    if curr == num {
        return index;
    }
    return -1;
}

// Sum of first n Fibonacci numbers
fn fibSum(n: int): int {
    var sum = 0;
    var prev = 0;
    var curr = 1;

    for i in 0..n {
        if i == 0 {
            sum = sum + prev;
        } else if i == 1 {
            sum = sum + curr;
        } else {
            val next = prev + curr;
            prev = curr;
            curr = next;
            sum = sum + curr;
        }
    }

    return sum;
}

fn main() {
    println("=== Fibonacci Demo (No GC) ===");
    println("");

    // Print sequence
    printFibSequence(15);

    println("");
    println("Individual Fibonacci calculations:");

    // Calculate specific Fibonacci numbers
    for i in [5, 10, 15, 20] {
        val result = fibIterative(i);
        println("F(${i}) = ${result}");
    }

    println("");
    println("Recursive vs Iterative (small values):");
    for i in [10, 12, 15] {
        val recursive = fibRecursive(i);
        val iterative = fibIterative(i);
        println("F(${i}): recursive=${recursive}, iterative=${iterative}");
    }

    println("");
    println("Checking Fibonacci membership:");
    val testNumbers = [1, 2, 3, 4, 5, 8, 13, 20, 21, 34, 50, 55];
    for num in testNumbers {
        val isFib = isFibonacci(num);
        if isFib {
            val idx = fibIndex(num);
            println("${num} is Fibonacci (index: ${idx})");
        } else {
            println("${num} is NOT Fibonacci");
        }
    }

    println("");
    println("Sum of Fibonacci sequences:");
    for n in [5, 10, 15] {
        val sum = fibSum(n);
        println("Sum of first ${n} Fibonacci numbers: ${sum}");
    }

    println("");
    println("=== Done ===");
}
