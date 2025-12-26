// Example: Basic Math Operations
// Demonstrates the math module

package main;

use math;
use log;

fn main() {
    log.info("=== Math Module Example ===");

    // Constants
    log.info("Pi = " + math.PI);
    log.info("E = " + math.E);
    log.info("Golden Ratio = " + math.PHI);

    // Trigonometry
    val angle = 45.0;
    val radians = math.toRadians(angle);
    log.infoWith("Trigonometry", {
        angle: angle,
        radians: radians,
        sin: math.sin(radians),
        cos: math.cos(radians),
        tan: math.tan(radians)
    });

    // Powers and roots
    log.infoWith("Powers and Roots", {
        sqrt_16: math.sqrt(16.0),
        cbrt_27: math.cbrt(27.0),
        pow_2_10: math.pow(2.0, 10.0)
    });

    // Number theory
    log.info("Is 17 prime? " + math.isPrime(17));
    log.info("Is 15 prime? " + math.isPrime(15));
    log.info("GCD of 48 and 18: " + math.gcd(48, 18));
    log.info("LCM of 12 and 18: " + math.lcm(12, 18));
    log.info("Factorial of 5: " + math.factorial(5));

    // Statistics
    val numbers = [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0];
    log.infoWith("Statistics", {
        sum: math.sum(numbers),
        average: math.average(numbers),
        median: math.median(numbers)
    });

    // Utilities
    log.info("Clamp 15 to [0, 10]: " + math.clamp(15.0, 0.0, 10.0));
    log.info("Lerp between 0 and 100 at 0.5: " + math.lerp(0.0, 100.0, 0.5));
}
