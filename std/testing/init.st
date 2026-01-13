// Testing module - Unit testing framework
// Inspired by Go's testing package and Jest

package testing;

// ============================================================================
// Simple assertion functions
// ============================================================================

// Global test state
var testsPassed = 0;
var testsFailed = 0;
var testsSkipped = 0;
var currentSuiteName = "";
var currentTestName = "";

// Assert that two values are equal
fn assertEqual(actual: any, expected: any) void {
    if actual == expected {
        testsPassed = testsPassed + 1;
        println("[PASS] " + currentSuiteName + " > " + currentTestName);
    } else {
        testsFailed = testsFailed + 1;
        println("[FAIL] " + currentSuiteName + " > " + currentTestName);
        println("  Expected: " + expected);
        println("  Actual: " + actual);
    }
}

// Assert that two values are not equal
fn assertNotEqual(actual: any, expected: any) void {
    if actual != expected {
        testsPassed = testsPassed + 1;
        println("[PASS] " + currentSuiteName + " > " + currentTestName);
    } else {
        testsFailed = testsFailed + 1;
        println("[FAIL] " + currentSuiteName + " > " + currentTestName);
        println("  Expected NOT equal to: " + expected);
        println("  But got: " + actual);
    }
}

// Assert that a value is true
fn assertTrue(value: bool) void {
    if value {
        testsPassed = testsPassed + 1;
        println("[PASS] " + currentSuiteName + " > " + currentTestName);
    } else {
        testsFailed = testsFailed + 1;
        println("[FAIL] " + currentSuiteName + " > " + currentTestName);
        println("  Expected: true");
        println("  Actual: false");
    }
}

// Assert that a value is false
fn assertFalse(value: bool) void {
    if !value {
        testsPassed = testsPassed + 1;
        println("[PASS] " + currentSuiteName + " > " + currentTestName);
    } else {
        testsFailed = testsFailed + 1;
        println("[FAIL] " + currentSuiteName + " > " + currentTestName);
        println("  Expected: false");
        println("  Actual: true");
    }
}

// ============================================================================
// Fluent assertion API (simplified - just returns the value)
// ============================================================================

// Create an expectation (just returns the value for chaining)
fn expect(actual: any) any {
    return actual;
}

// Expectation matchers
fn toBe(actual: any, expected: any) void {
    assertEqual(actual, expected);
}

fn toEqual(actual: any, expected: any) void {
    assertEqual(actual, expected);
}

fn toBeTrue(actual: any) void {
    assertTrue(actual);
}

fn toBeFalse(actual: any) void {
    assertFalse(actual);
}

fn toNotBe(actual: any, expected: any) void {
    assertNotEqual(actual, expected);
}

// ============================================================================
// Test suite structure
// ============================================================================

// Define a test suite
fn describe(suiteName: string, suiteBody: Function) void {
    println("");
    println("=== " + suiteName + " ===");
    currentSuiteName = suiteName;
    suiteBody();
}

// Define a test case
fn it(testName: string, testBody: Function) void {
    currentTestName = testName;
    testBody();
}

// Skip a test
fn skip(testName: string, testBody: Function) void {
    testsSkipped = testsSkipped + 1;
    println("[SKIP] " + currentSuiteName + " > " + testName);
}

// Setup hooks (simplified - just execute immediately)
fn beforeEach(fn: Function) void {
    fn();
}

fn afterEach(fn: Function) void {
    fn();
}

fn beforeAll(fn: Function) void {
    fn();
}

fn afterAll(fn: Function) void {
    fn();
}

// ============================================================================
// Test runner and reporting
// ============================================================================

// Print test summary
fn printSummary() void {
    println("");
    println("===================");
    println("Test Summary:");
    println("  Passed: " + testsPassed);
    println("  Failed: " + testsFailed);
    println("  Skipped: " + testsSkipped);
    val total = testsPassed + testsFailed + testsSkipped;
    println("  Total: " + total);
    println("===================");
}

// Reset test counters
fn resetTests() void {
    testsPassed = 0;
    testsFailed = 0;
    testsSkipped = 0;
}

// Get number of passed tests
fn getPassedCount() int {
    return testsPassed;
}

// Get number of failed tests
fn getFailedCount() int {
    return testsFailed;
}

// Get number of skipped tests
fn getSkippedCount() int {
    return testsSkipped;
}
