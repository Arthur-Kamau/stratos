// Example: Unit Testing
// Demonstrates the testing module

package main;

use testing;
use math;

// Test suite for math functions
fn testMath() {
    testing.describe("Math functions", () => {
        testing.it("should calculate factorial correctly", () => {
            val result = math.factorial(5);
            testing.toBe(testing.expect(result), 120);
        });

        testing.it("should identify prime numbers", () => {
            testing.toBeTrue(testing.expect(math.isPrime(17)));
            testing.toBeFalse(testing.expect(math.isPrime(18)));
            testing.toBeTrue(testing.expect(math.isPrime(2)));
        });

        testing.it("should calculate GCD", () => {
            val result = math.gcd(48, 18);
            testing.toBe(testing.expect(result), 6);
        });

        testing.it("should clamp values correctly", () => {
            testing.toBe(testing.expect(math.clamp(15.0, 0.0, 10.0)), 10.0);
            testing.toBe(testing.expect(math.clamp(-5.0, 0.0, 10.0)), 0.0);
            testing.toBe(testing.expect(math.clamp(5.0, 0.0, 10.0)), 5.0);
        });

        testing.it("should calculate absolute values", () => {
            testing.toBe(testing.expect(math.abs(-5.0)), 5.0);
            testing.toBe(testing.expect(math.abs(5.0)), 5.0);
        });

        testing.it("should find min and max", () => {
            testing.toBe(testing.expect(math.min(3.0, 7.0)), 3.0);
            testing.toBe(testing.expect(math.max(3.0, 7.0)), 7.0);
        });
    });
}

// Test suite for arithmetic operations
fn testArithmetic() {
    testing.describe("Arithmetic Operations", () => {
        testing.it("should add numbers correctly", () => {
            val result = 5 + 3;
            testing.toBe(testing.expect(result), 8);
        });

        testing.it("should subtract numbers correctly", () => {
            val result = 10 - 4;
            testing.toBe(testing.expect(result), 6);
        });

        testing.it("should multiply numbers correctly", () => {
            val result = 6 * 7;
            testing.toBe(testing.expect(result), 42);
        });

        testing.it("should divide numbers correctly", () => {
            val result = 20 / 5;
            testing.toBe(testing.expect(result), 4);
        });

        testing.skip("this test is skipped", () => {
            // This test will be skipped
            testing.toBeFalse(testing.expect(true));
        });
    });
}

// Test suite for boolean logic
fn testBooleans() {
    testing.describe("Boolean Logic", () => {
        testing.it("should handle AND operations", () => {
            testing.toBeTrue(testing.expect(true && true));
            testing.toBeFalse(testing.expect(true && false));
        });

        testing.it("should handle OR operations", () => {
            testing.toBeTrue(testing.expect(true || false));
            testing.toBeFalse(testing.expect(false || false));
        });

        testing.it("should handle NOT operations", () => {
            testing.toBeTrue(testing.expect(!false));
            testing.toBeFalse(testing.expect(!true));
        });
    });
}

// Test suite for basic assertions
fn testAssertions() {
    testing.describe("Basic Assertions", () => {
        testing.it("should assert equality", () => {
            testing.assertEqual(5, 5);
            testing.assertEqual("hello", "hello");
            testing.assertEqual(3.14, 3.14);
        });

        testing.it("should assert inequality", () => {
            testing.assertNotEqual(5, 10);
            testing.assertNotEqual("hello", "world");
        });

        testing.it("should assert boolean values", () => {
            testing.assertTrue(true);
            testing.assertTrue(5 > 3);
            testing.assertFalse(false);
            testing.assertFalse(2 > 10);
        });

        testing.it("should use fluent assertions", () => {
            testing.toBe(testing.expect(42), 42);
            testing.toNotBe(testing.expect(42), 100);
            testing.toBeTrue(testing.expect(true));
            testing.toBeFalse(testing.expect(false));
        });
    });
}

// Test suite for string operations
fn testStrings() {
    testing.describe("String Operations", () => {
        testing.it("should concatenate strings", () => {
            val result = "Hello" + " " + "World";
            testing.toBe(testing.expect(result), "Hello World");
        });

        testing.it("should compare strings", () => {
            testing.toBeTrue(testing.expect("hello" == "hello"));
            testing.toBeFalse(testing.expect("hello" == "world"));
        });
    });
}

fn main() {
    println("=== Testing Framework Example ===");
    println("");

    // Run test suites
    testMath();
    testArithmetic();
    testBooleans();
    testAssertions();
    testStrings();

    // Print summary
    testing.printSummary();

    // Check if all tests passed
    if testing.getFailedCount() == 0 {
        println("");
        println("✓ All tests passed!");
    } else {
        println("");
        println("✗ Some tests failed!");
    }
}
