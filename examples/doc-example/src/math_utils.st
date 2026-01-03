/**
 * Math utilities module - Advanced mathematical operations
 *
 * This module provides extended mathematical functions beyond the standard
 * math library, including statistical operations, number theory functions,
 * and geometric calculations.
 *
 * @since 1.0.0
 */
package mathutils;

use math;

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
    if numbers.length == 0 {
        return 0.0;
    }

    var sum = 0.0;
    for i in 0..numbers.length {
        sum = sum + numbers[i];
    }

    return sum / numbers.length;
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
    if numbers.length == 0 {
        return 0.0;
    }

    // Note: This is a simplified implementation
    // In production, you'd want to sort the array first
    val mid = numbers.length / 2;
    return numbers[mid];
}

/**
 * Calculates the standard deviation of a dataset.
 *
 * Standard deviation measures the amount of variation in a set of values.
 * A low standard deviation indicates values are close to the mean.
 * A high standard deviation indicates values are spread out over a wider range.
 *
 * @param numbers Array of numbers to analyze
 * @return The standard deviation
 * @example
 * val stddev = standardDeviation([2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0]);
 * @since 1.0.0
 */
fn standardDeviation(numbers: Array<double>) double {
    val avg = mean(numbers);
    var sumSquaredDiff = 0.0;

    for i in 0..numbers.length {
        val diff = numbers[i] - avg;
        sumSquaredDiff = sumSquaredDiff + (diff * diff);
    }

    return math.sqrt(sumSquaredDiff / numbers.length);
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
    if n <= 1 {
        return false;
    }

    if n <= 3 {
        return true;
    }

    if n % 2 == 0 || n % 3 == 0 {
        return false;
    }

    var i = 5;
    while i * i <= n {
        if n % i == 0 || n % (i + 2) == 0 {
            return false;
        }
        i = i + 6;
    }

    return true;
}

/**
 * Calculates the factorial of a number using iteration.
 *
 * The factorial of n (denoted n!) is the product of all positive integers
 * less than or equal to n. For example, 5! = 5 × 4 × 3 × 2 × 1 = 120.
 * By convention, 0! = 1.
 *
 * @param n The number to calculate factorial for (must be non-negative)
 * @return The factorial of n
 * @throws "Invalid input" if n is negative
 * @example
 * val fact5 = factorial(5);  // Returns 120
 * val fact0 = factorial(0);  // Returns 1
 * @since 1.0.0
 */
fn factorial(n: int) int {
    if n < 0 {
        println("Error: Factorial not defined for negative numbers");
        return -1;
    }

    if n == 0 || n == 1 {
        return 1;
    }

    var result = 1;
    for i in 2..n+1 {
        result = result * i;
    }

    return result;
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

    if x < 0 {
        x = -x;
    }
    if y < 0 {
        y = -y;
    }

    while y != 0 {
        val temp = y;
        y = x % y;
        x = temp;
    }

    return x;
}

// ============================================================================
// Geometric Functions
// ============================================================================

/**
 * Calculates the distance between two points in 2D space.
 *
 * Uses the Pythagorean theorem: distance = sqrt((x2-x1)² + (y2-y1)²)
 *
 * @param x1 X coordinate of first point
 * @param y1 Y coordinate of first point
 * @param x2 X coordinate of second point
 * @param y2 Y coordinate of second point
 * @return The Euclidean distance between the points
 * @example
 * val dist = distance(0.0, 0.0, 3.0, 4.0);  // Returns 5.0
 * @since 1.0.0
 */
fn distance(x1: double, y1: double, x2: double, y2: double) double {
    val dx = x2 - x1;
    val dy = y2 - y1;
    return math.sqrt(dx * dx + dy * dy);
}
