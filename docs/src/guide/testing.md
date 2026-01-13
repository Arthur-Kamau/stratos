# Testing

Stratos includes a built-in testing framework inspired by Jest and Go's testing package. Write tests to ensure your code works correctly and catch bugs early.

## Quick Start

Create a test file and import the testing module:

```stratos
package main;

use testing;
use math;

fn testMath() {
    testing.describe("Math functions", () => {
        testing.it("should calculate factorial", () => {
            val result = math.factorial(5);
            testing.toBe(testing.expect(result), 120);
        });
    });
}

fn main() {
    testMath();
    testing.printSummary();
}
```

Run your tests:

```bash
stratos run your_test.st
```

## Test Structure

### Describe Blocks

Use `describe()` to group related tests into a test suite:

```stratos
testing.describe("Math operations", () => {
    // Your tests go here
    testing.it("should add numbers", () => {
        // Test implementation
    });

    testing.it("should subtract numbers", () => {
        // Test implementation
    });
});
```

### Test Cases

Use `it()` to define individual test cases:

```stratos
testing.it("should multiply numbers correctly", () => {
    val result = 6 * 7;
    testing.toBe(testing.expect(result), 42);
});
```

### Skipping Tests

Skip tests that aren't ready or are temporarily disabled:

```stratos
testing.skip("this test is not ready yet", () => {
    // This test will be skipped
    testing.toBe(testing.expect(1), 2);
});
```

## Assertions

### Basic Assertions

Direct assertion functions:

```stratos
// Assert equality
testing.assertEqual(5, 5);
testing.assertEqual("hello", "hello");

// Assert inequality
testing.assertNotEqual(5, 10);
testing.assertNotEqual("hello", "world");

// Assert boolean values
testing.assertTrue(true);
testing.assertTrue(5 > 3);
testing.assertFalse(false);
testing.assertFalse(2 > 10);
```

### Fluent Assertions

Chain assertions with `expect()` for a more readable style:

```stratos
testing.describe("Fluent assertions", () => {
    testing.it("should work with toBe", () => {
        testing.toBe(testing.expect(42), 42);
        testing.toBe(testing.expect("hello"), "hello");
    });

    testing.it("should work with toEqual", () => {
        testing.toEqual(testing.expect(3.14), 3.14);
    });

    testing.it("should work with boolean matchers", () => {
        testing.toBeTrue(testing.expect(true));
        testing.toBeFalse(testing.expect(false));
    });

    testing.it("should work with toNotBe", () => {
        testing.toNotBe(testing.expect(42), 100);
    });
});
```

## Available Matchers

| Matcher | Description | Example |
|---------|-------------|---------|
| `toBe(actual, expected)` | Checks strict equality | `testing.toBe(testing.expect(5), 5)` |
| `toEqual(actual, expected)` | Checks value equality | `testing.toEqual(testing.expect(x), y)` |
| `toNotBe(actual, expected)` | Checks inequality | `testing.toNotBe(testing.expect(5), 10)` |
| `toBeTrue(actual)` | Checks if value is true | `testing.toBeTrue(testing.expect(x > 5))` |
| `toBeFalse(actual)` | Checks if value is false | `testing.toBeFalse(testing.expect(x < 0))` |
| `assertEqual(actual, expected)` | Direct equality check | `testing.assertEqual(5, 5)` |
| `assertNotEqual(actual, expected)` | Direct inequality check | `testing.assertNotEqual(5, 10)` |
| `assertTrue(value)` | Direct true check | `testing.assertTrue(true)` |
| `assertFalse(value)` | Direct false check | `testing.assertFalse(false)` |

## Test Results

### Printing Summary

Get a summary of your test results:

```stratos
fn main() {
    // Run your tests
    testMath();
    testStrings();
    testLogic();

    // Print summary
    testing.printSummary();
}
```

Output:

```
===================
Test Summary:
  Passed: 25
  Failed: 2
  Skipped: 1
  Total: 28
===================
```

### Checking Results

Check if tests passed or failed:

```stratos
fn main() {
    runAllTests();
    testing.printSummary();

    if testing.getFailedCount() == 0 {
        println("✓ All tests passed!");
    } else {
        println("✗ Some tests failed!");
    }
}
```

### Resetting Counters

Reset test counters between test runs:

```stratos
testing.resetTests();
```

## Setup and Teardown

Run code before or after each test:

```stratos
testing.describe("Database tests", () => {
    testing.beforeEach(() => {
        println("Setting up test...");
        // Setup code here
    });

    testing.afterEach(() => {
        println("Cleaning up...");
        // Cleanup code here
    });

    testing.it("should insert data", () => {
        // Test code
    });

    testing.it("should query data", () => {
        // Test code
    });
});
```

::: tip
`beforeEach()` and `afterEach()` run for every test in the suite. Use them to set up test fixtures or clean up resources.
:::

## Complete Example

Here's a complete test file:

```stratos
package main;

use testing;
use math;

fn testArithmetic() {
    testing.describe("Arithmetic Operations", () => {
        testing.it("should add numbers", () => {
            testing.toBe(testing.expect(5 + 3), 8);
        });

        testing.it("should subtract numbers", () => {
            testing.toBe(testing.expect(10 - 4), 6);
        });

        testing.it("should multiply numbers", () => {
            testing.toBe(testing.expect(6 * 7), 42);
        });

        testing.it("should divide numbers", () => {
            testing.toBe(testing.expect(20 / 5), 4);
        });
    });
}

fn testMath() {
    testing.describe("Math Library", () => {
        testing.it("should calculate factorial", () => {
            testing.toBe(testing.expect(math.factorial(5)), 120);
            testing.toBe(testing.expect(math.factorial(0)), 1);
        });

        testing.it("should identify prime numbers", () => {
            testing.toBeTrue(testing.expect(math.isPrime(17)));
            testing.toBeFalse(testing.expect(math.isPrime(18)));
            testing.toBeTrue(testing.expect(math.isPrime(2)));
        });

        testing.it("should calculate GCD", () => {
            testing.toBe(testing.expect(math.gcd(48, 18)), 6);
            testing.toBe(testing.expect(math.gcd(100, 50)), 50);
        });
    });
}

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
    println("=== Running Tests ===\n");

    // Run all test suites
    testArithmetic();
    testMath();
    testStrings();

    // Print results
    testing.printSummary();

    // Exit with appropriate status
    if testing.getFailedCount() == 0 {
        println("\n✓ All tests passed!");
    } else {
        println("\n✗ " + testing.getFailedCount() + " test(s) failed!");
    }
}
```

## Best Practices

::: tip Organize Your Tests
Group related tests using `describe()` blocks. This makes your test output more readable and helps you understand what's being tested.
:::

::: tip Test One Thing at a Time
Each `it()` block should test a single behavior or functionality. This makes it easier to identify what broke when a test fails.
:::

::: tip Use Descriptive Names
Write clear, descriptive names for your test cases that explain what they're testing:

```stratos
// Good
testing.it("should return empty array when input is empty", () => { ... });

// Bad
testing.it("test 1", () => { ... });
```
:::

::: tip Test Edge Cases
Don't just test the happy path. Test edge cases, error conditions, and boundary values:

```stratos
testing.describe("Division", () => {
    testing.it("should divide positive numbers", () => {
        testing.toBe(testing.expect(10 / 2), 5);
    });

    testing.it("should handle division by zero", () => {
        // Test error handling
    });

    testing.it("should handle negative numbers", () => {
        testing.toBe(testing.expect(-10 / 2), -5);
    });
});
```
:::

::: tip Keep Tests Independent
Each test should be able to run independently. Don't rely on the order of test execution or state from previous tests.
:::

## API Reference

### Test Structure Functions

- `describe(name: string, fn: Function)` - Create a test suite
- `it(name: string, fn: Function)` - Create a test case
- `skip(name: string, fn: Function)` - Skip a test

### Setup/Teardown Hooks

- `beforeEach(fn: Function)` - Run before each test
- `afterEach(fn: Function)` - Run after each test
- `beforeAll(fn: Function)` - Run before all tests in suite
- `afterAll(fn: Function)` - Run after all tests in suite

### Assertion Functions

- `assertEqual(actual, expected)` - Assert equality
- `assertNotEqual(actual, expected)` - Assert inequality
- `assertTrue(value)` - Assert value is true
- `assertFalse(value)` - Assert value is false

### Fluent Assertions

- `expect(value)` - Create an expectation
- `toBe(actual, expected)` - Check equality
- `toEqual(actual, expected)` - Check value equality
- `toNotBe(actual, expected)` - Check inequality
- `toBeTrue(actual)` - Check if true
- `toBeFalse(actual)` - Check if false

### Test Results

- `printSummary()` - Print test results summary
- `getPassedCount()` - Get number of passed tests
- `getFailedCount()` - Get number of failed tests
- `getSkippedCount()` - Get number of skipped tests
- `resetTests()` - Reset test counters

## Next Steps

- Learn about [Error Handling](/guide/error-handling) to test error conditions
- Explore [Async/Await](/guide/async) for testing async code
- Check out the [Standard Library](/reference/stdlib) for more testing utilities
