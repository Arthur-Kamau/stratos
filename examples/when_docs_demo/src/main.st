package main;

use io;

fn classifyNumber(num: int) string {
    return when {
        num < 0 -> "Negative"
        num == 0 -> "Zero"
        num > 0 and num <= 10 -> "Small Positive"
        num > 10 and num <= 100 -> "Medium Positive"
        else -> "Large Positive"
    };
}

fn isPrime(n: int) bool {
    if (n <= 1) return false;
    if (n <= 3) return true;

    var i = 2;
    while (i * i <= n) {
        if (n % i == 0) {
            return false;
        }
        i++;
    }

    return true;
}

fn main() {
    // Test classification
    val numbers = [-5, 0, 7, 42, 150];

    for val num in numbers {
        val classification = classifyNumber(num);
        println(num + " is " + classification);
    }

    // Check for primes
    println("\nPrime numbers from 1 to 20:");
    var i = 1;
    while (i <= 20) {
        if (isPrime(i)) {
            println(i);
        }
        i++;
    }

    // User type access control
    val users = ["admin", "editor", "viewer", "guest"];

    for val user in users {
        val access = when (user) {
        "admin" -> "full"
        "editor" -> "write"
        "viewer" -> "read"
        else -> "none"
    };
        
        println(user + " has " + access + " access");
    }
}
