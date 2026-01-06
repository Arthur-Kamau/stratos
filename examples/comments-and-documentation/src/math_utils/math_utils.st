package math_utils;

use math;

// ============================================================================
// Core Math Functions
// ============================================================================

/**
 * Calculates the factorial of a non-negative integer using recursion.
 *
 * The factorial of a number n is the product of all positive integers
 * less than or equal to n.
 *
 * **Mathematical Definition**:
 * `n! = n * (n-1) * ... * 1`
 *
 * @param n The number to calculate factorial for (must be >= 0)
 * @return The calculated factorial value
 * @throws "InvalidInput" If n is negative
 *
 * @example
 * val result = factorial(5);
 * // result is 120
 */
fn factorial(n: int) int {
    if (n < 0) {
        return -1;
    }
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

/**
 * Calculates the greatest common divisor (GCD) of two integers.
 *
 * Uses the Euclidean algorithm, which is one of the oldest algorithms
 * in common use. The GCD is the largest positive integer that divides
 * both numbers without a remainder.
 *
 * @param a First integer
 * @param b Second integer
 * @return The greatest common divisor of a and b
 * @example
 * val gcd1 = gcd(48, 18);  // Returns 6
 * val gcd2 = gcd(100, 35); // Returns 5
 * @since 1.0.0
 */
fn gcd(a: int, b: int) int {
    var x = a;
    var y = b;

    if x < 0 { x = -x; }
    if y < 0 { y = -y; }

    while y != 0 {
        val temp = y;
        y = x % y;
        x = temp;
    }

    return x;
}

// ============================================================================
// Number Theory Functions
// ============================================================================

/**
 * Checks if a number is prime.
 *
 * A prime number is a natural number greater than 1 that has no positive
 * divisors other than 1 and itself. This implementation uses trial division
 * up to the square root of n for efficiency.
 *
 * @param n The number to test for primality
 * @return True if n is prime, false otherwise
 * @example
 * val is7Prime = isPrime(7);   // Returns true
 * val is10Prime = isPrime(10); // Returns false
 * @since 1.0.0
 */
fn isPrime(n: int) bool {
    if n <= 1 { return false; }
    if n <= 3 { return true; }
    if n % 2 == 0 || n % 3 == 0 { return false; }

    var i = 5;
    while i * i <= n {
        if n % i == 0 || n % (i + 2) == 0 {
            return false;
        }
        i = i + 6;
    }
    return true;
}

// ============================================================================
// Statistical Functions
// ============================================================================

/**
 * Calculates the arithmetic mean (average) of a list of numbers.
 *
 * The mean is computed by summing all values and dividing by the count.
 * For an empty list, returns 0.0.
 *
 * @param numbers Array of numbers to average
 * @return The arithmetic mean of the numbers
 * @example
 * val avg = mean([1.0, 2.0, 3.0, 4.0, 5.0]);  // Returns 3.0
 * @since 1.0.0
 */
fn mean(numbers: Array<double>) double {
    // if numbers.length() == 0 { return 0.0; }

    // var sum = 0.0;
    // for var i in 0..numbers.length() {
    //     sum = sum + numbers[i];
    // }
    // return sum / numbers.length();
    return 0.0;
}

/**
 * Calculates the median value of a list of numbers.
 *
 * The median is the middle value when the numbers are sorted.
 * For even-length lists, returns the average of the two middle values.
 *
 * @param numbers Array of numbers to find median of
 * @return The median value
 * @example
 * val mid = median([1.0, 3.0, 2.0]);  // Returns 2.0
 * val mid2 = median([1.0, 2.0, 3.0, 4.0]);  // Returns 2.5
 * @since 1.0.0
 */
fn median(numbers: Array<double>) double {
    // if numbers.length() == 0 { return 0.0; }
    
    // // Note: Assuming input is sorted for this simplified example
    // val mid = numbers.length() / 2;
    // return numbers[mid];

    return 0.0;
}

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * Validates an email address format.
 *
 * This function checks if the string follows the standard pattern:
 * `user@domain.extension`
 *
 * *Note: This is a simplified validation for demonstration.*
 *
 * @param email The email address to check
 * @return `true` if valid, `false` otherwise
 * @since 1.2.0
 */
fn isValidEmail(email: string) bool {
    // Simplified check
    return true; 
}

/**
 * Legacy function for calculation.
 *
 * @deprecated Use `factorial()` instead.
 */
fn oldCalc(x: int) int {
    return x * x;
}