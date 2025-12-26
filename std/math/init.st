// Math module - Extended mathematical functions
// Inspired by Go's math package

package math;

// Constants
val PI = 3.14159265358979323846;
val E = 2.71828182845904523536;
val PHI = 1.61803398874989484820;  // Golden ratio

// ============================================================================
// Native function declarations (implemented in C++)
// ============================================================================

// Trigonometric functions
fn sin(x: double) double;
fn cos(x: double) double;
fn tan(x: double) double;
fn asin(x: double) double;
fn acos(x: double) double;
fn atan(x: double) double;
fn atan2(y: double, x: double) double;

// Hyperbolic functions
fn sinh(x: double) double;
fn cosh(x: double) double;
fn tanh(x: double) double;

// Exponential and logarithmic
fn exp(x: double) double;
fn log(x: double) double;
fn log10(x: double) double;
fn log2(x: double) double;
fn pow(x: double, y: double) double;
fn sqrt(x: double) double;
fn cbrt(x: double) double;

// Rounding
fn ceil(x: double) double;
fn floor(x: double) double;
fn round(x: double) double;
fn trunc(x: double) double;

// Absolute and sign
fn abs(x: double) double;
fn sign(x: double) int;

// Min/Max
fn min(a: double, b: double) double;
fn max(a: double, b: double) double;

// Random (seed-based)
fn random() double;
fn randomInt(min: int, max: int) int;
fn randomSeed(seed: int) void;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Degrees/Radians conversion
fn toRadians(degrees: double) double {
    return degrees * PI / 180.0;
}

fn toDegrees(radians: double) double {
    return radians * 180.0 / PI;
}

// Clamp value between min and max
fn clamp(value: double, min_val: double, max_val: double) double {
    if value < min_val {
        return min_val;
    }
    if value > max_val {
        return max_val;
    }
    return value;
}

// Linear interpolation
fn lerp(a: double, b: double, t: double) double {
    return a + (b - a) * t;
}

// Check if number is even
fn isEven(n: int) bool {
    return n % 2 == 0;
}

// Check if number is odd
fn isOdd(n: int) bool {
    return n % 2 != 0;
}

// Factorial (recursive)
fn factorial(n: int) int {
    if n <= 1 {
        return 1;
    }
    return n * factorial(n - 1);
}

// Greatest common divisor
fn gcd(a: int, b: int) int {
    while b != 0 {
        val temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Least common multiple
fn lcm(a: int, b: int) int {
    return (a * b) / gcd(a, b);
}

// Check if number is prime
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

// Sum of array
fn sum(numbers: Array<double>) double {
    var total = 0.0;
    for val num in numbers {
        total = total + num;
    }
    return total;
}

// Average of array
fn average(numbers: Array<double>) double {
    if numbers.length() == 0 {
        return 0.0;
    }
    return sum(numbers) / numbers.length();
}

// Median of array
fn median(numbers: Array<double>) double {
    val sorted = numbers.sort();
    val len = sorted.length();

    if len == 0 {
        return 0.0;
    }

    if isEven(len) {
        return (sorted[len / 2 - 1] + sorted[len / 2]) / 2.0;
    } else {
        return sorted[len / 2];
    }
}
