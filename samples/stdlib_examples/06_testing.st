// Example: Unit Testing
// Demonstrates the testing module

package main;

use testing;
use math;
use strings;
use collections;

// Test suite for math functions
fn testMath() {
    testing.describe("Math functions", () => {
        testing.it("should calculate factorial correctly", () => {
            val result = math.factorial(5);
            testing.expect(result).toBe(120);
        });

        testing.it("should identify prime numbers", () => {
            testing.expect(math.isPrime(17)).toBeTrue();
            testing.expect(math.isPrime(18)).toBeFalse();
            testing.expect(math.isPrime(2)).toBeTrue();
        });

        testing.it("should calculate GCD", () => {
            val result = math.gcd(48, 18);
            testing.expect(result).toBe(6);
        });

        testing.it("should clamp values correctly", () => {
            testing.expect(math.clamp(15.0, 0.0, 10.0)).toBe(10.0);
            testing.expect(math.clamp(-5.0, 0.0, 10.0)).toBe(0.0);
            testing.expect(math.clamp(5.0, 0.0, 10.0)).toBe(5.0);
        });
    });
}

// Test suite for string functions
fn testStrings() {
    testing.describe("String functions", () => {
        testing.beforeEach(() => {
            // Setup before each test
        });

        testing.afterEach(() => {
            // Cleanup after each test
        });

        testing.it("should convert to uppercase", () => {
            val result = strings.toUpper("hello");
            testing.expect(result).toBe("HELLO");
        });

        testing.it("should capitalize first letter", () => {
            val result = strings.capitalize("hello world");
            testing.expect(result).toBe("Hello world");
        });

        testing.it("should convert to camelCase", () => {
            val result = strings.toCamelCase("hello world");
            testing.expect(result).toBe("helloWorld");
        });

        testing.it("should convert to snake_case", () => {
            val result = strings.toSnakeCase("hello world");
            testing.expect(result).toBe("hello_world");
        });

        testing.it("should count words", () => {
            val result = strings.wordCount("The quick brown fox");
            testing.expect(result).toBe(4);
        });

        testing.it("should truncate strings", () => {
            val result = strings.truncate("Hello, World!", 8, "...");
            testing.expect(result).toBe("Hello...");
        });

        testing.skip("this test is skipped", () => {
            // This test will be skipped
            testing.expect(true).toBeFalse();
        });
    });
}

// Test suite for collections
fn testCollections() {
    testing.describe("Collections", () => {
        testing.it("should create and manipulate lists", () => {
            val list = collections.newList<int>();
            list.add(1);
            list.add(2);
            list.add(3);

            testing.expect(list.length()).toBe(3);
            testing.expect(list.get(1)).toBe(2);
        });

        testing.it("should filter lists", () => {
            val list = collections.listOf([1, 2, 3, 4, 5, 6]);
            val evens = list.filter((n) => math.isEven(n));

            testing.expect(evens.length()).toBe(3);
        });

        testing.it("should map over lists", () => {
            val list = collections.listOf([1, 2, 3]);
            val doubled = list.map((n) => n * 2);

            testing.expect(doubled.get(0)).toBe(2);
            testing.expect(doubled.get(1)).toBe(4);
            testing.expect(doubled.get(2)).toBe(6);
        });

        testing.it("should reduce lists", () => {
            val list = collections.listOf([1, 2, 3, 4, 5]);
            val sum = list.reduce(0, (acc, n) => acc + n);

            testing.expect(sum).toBe(15);
        });
    });
}

// Mock and spy examples
fn testMocksAndSpies() {
    testing.describe("Mocks and Spies", () => {
        testing.it("should mock function calls", () => {
            val mockFn = testing.mock<int>();
            mockFn.mockReturnValue(42);

            val result = mockFn.call([]);

            testing.expect(result).toBe(42);
            testing.expect(mockFn.getCallCount()).toBe(1);
        });

        testing.it("should spy on function calls", () => {
            val originalFn = (x: int) => x * 2;
            val spyFn = testing.spy(originalFn);

            val result = spyFn.call([5]);

            testing.expect(result).toBe(10);
            testing.expect(spyFn.getCalls().length()).toBe(1);
            testing.expect(spyFn.wasCalledWith([5])).toBeTrue();
        });
    });
}

// Benchmark example
fn benchmarkExample() {
    testing.benchmark("Factorial calculation", 1000, () => {
        math.factorial(10);
    });

    testing.benchmark("String concatenation", 1000, () => {
        var result = "";
        for i in 0..100 {
            result = result + "a";
        }
    });
}

fn main() {
    log.info("=== Testing Framework Example ===\n");

    // Run tests
    testMath();
    testStrings();
    testCollections();
    testMocksAndSpies();

    // Run all tests and get report
    val report = testing.runAllTests();

    log.info("\n=== Test Results ===");
    report.print();

    log.infoWith("Summary", {
        total: report.totalTests,
        passed: report.passed,
        failed: report.failed,
        skipped: report.skipped,
        duration_ms: report.duration.milliseconds()
    });

    // Run benchmarks
    log.info("\n=== Benchmarks ===");
    benchmarkExample();

    // Export results as JSON
    val jsonReport = report.toJSON();
    io.writeFile("test-results.json", json.pretty(jsonReport));
    log.info("\nTest results exported to test-results.json");
}
