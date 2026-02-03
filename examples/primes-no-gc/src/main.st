package main;

// Prime numbers example demonstrating Stratos without GC
// This project has garbage collection disabled in stratos.conf

// Check if a number is prime
fn isPrime(n: int): bool {
    if n < 2 {
        return false;
    }
    if n == 2 {
        return true;
    }
    if n % 2 == 0 {
        return false;
    }

    var i = 3;
    while i * i <= n {
        if n % i == 0 {
            return false;
        }
        i = i + 2;
    }
    return true;
}

// Find the nth prime number
fn nthPrime(n: int): int {
    if n <= 0 {
        return 0;
    }

    var count = 0;
    var num = 2;

    while count < n {
        if isPrime(num) {
            count = count + 1;
            if count == n {
                return num;
            }
        }
        num = num + 1;
    }

    return num;
}

// Count primes up to n (prime counting function)
fn countPrimes(n: int): int {
    var count = 0;
    for i in 2..(n + 1) {
        if isPrime(i) {
            count = count + 1;
        }
    }
    return count;
}

// Sum of primes up to n
fn sumPrimes(n: int): int {
    var sum = 0;
    for i in 2..(n + 1) {
        if isPrime(i) {
            sum = sum + i;
        }
    }
    return sum;
}

// Find next prime after n
fn nextPrime(n: int): int {
    var candidate = n + 1;
    while !isPrime(candidate) {
        candidate = candidate + 1;
    }
    return candidate;
}

// Find previous prime before n
fn prevPrime(n: int): int {
    if n <= 2 {
        return 0;
    }
    var candidate = n - 1;
    while candidate > 1 && !isPrime(candidate) {
        candidate = candidate - 1;
    }
    if candidate < 2 {
        return 0;
    }
    return candidate;
}

// Check if two numbers are coprime (GCD = 1)
fn gcd(a: int, b: int): int {
    var x = a;
    var y = b;
    while y != 0 {
        val temp = y;
        y = x % y;
        x = temp;
    }
    return x;
}

fn areCoprime(a: int, b: int): bool {
    return gcd(a, b) == 1;
}

// Find prime factors of a number
fn printPrimeFactors(n: int) {
    print("Prime factors of ${n}: ");
    var num = n;
    var first = true;

    // Check for 2
    while num % 2 == 0 {
        if first {
            print("2");
            first = false;
        } else {
            print(" x 2");
        }
        num = num / 2;
    }

    // Check for odd factors
    var i = 3;
    while i * i <= num {
        while num % i == 0 {
            if first {
                print("${i}");
                first = false;
            } else {
                print(" x ${i}");
            }
            num = num / i;
        }
        i = i + 2;
    }

    // If num is still greater than 1, it's a prime factor
    if num > 1 {
        if first {
            print("${num}");
        } else {
            print(" x ${num}");
        }
    }

    println("");
}

// Print primes in a range
fn printPrimesInRange(start: int, end: int) {
    println("Primes from ${start} to ${end}:");
    var count = 0;
    for i in start..(end + 1) {
        if isPrime(i) {
            print("${i} ");
            count = count + 1;
            if count % 10 == 0 {
                println("");
            }
        }
    }
    if count % 10 != 0 {
        println("");
    }
    println("Total: ${count} primes");
}

fn main() {
    println("===========================================");
    println("   Prime Numbers Demo (No GC)");
    println("===========================================");
    println("");

    // Test primality
    println("=== Primality Testing ===");
    val testNumbers = [1, 2, 3, 4, 5, 17, 18, 19, 97, 100, 101];
    for n in testNumbers {
        val result = isPrime(n);
        if result {
            println("${n} is PRIME");
        } else {
            println("${n} is NOT prime");
        }
    }

    println("");
    println("=== Nth Prime Numbers ===");
    for i in [1, 5, 10, 25, 50, 100] {
        val prime = nthPrime(i);
        println("Prime #${i} = ${prime}");
    }

    println("");
    println("=== Prime Counting ===");
    for n in [10, 50, 100, 500, 1000] {
        val count = countPrimes(n);
        println("Primes up to ${n}: ${count}");
    }

    println("");
    println("=== Sum of Primes ===");
    for n in [10, 50, 100] {
        val sum = sumPrimes(n);
        println("Sum of primes up to ${n}: ${sum}");
    }

    println("");
    println("=== Next/Previous Prime ===");
    for n in [10, 20, 50, 100] {
        val next = nextPrime(n);
        val prev = prevPrime(n);
        println("For ${n}: prev=${prev}, next=${next}");
    }

    println("");
    println("=== Prime Factorization ===");
    for n in [12, 28, 60, 97, 100, 360] {
        printPrimeFactors(n);
    }

    println("");
    println("=== Coprimality Testing ===");
    val pairs = [[8, 15], [12, 18], [14, 25], [21, 35]];
    for pair in pairs {
        val a = pair[0];
        val b = pair[1];
        val coprime = areCoprime(a, b);
        val gcdVal = gcd(a, b);
        if coprime {
            println("${a} and ${b} are COPRIME (gcd=${gcdVal})");
        } else {
            println("${a} and ${b} are NOT coprime (gcd=${gcdVal})");
        }
    }

    println("");
    printPrimesInRange(1, 100);

    println("");
    println("===========================================");
    println("   Demo Complete!");
    println("===========================================");
}
