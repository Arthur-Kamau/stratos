// Testing module - Unit testing framework
// Inspired by Go's testing package and Jest

package testing;

// ============================================================================
// Types
// ============================================================================

enum TestStatus {
    PASSED,
    FAILED,
    SKIPPED
}

class TestCase {
    var name: string;
    var fn: Function;
    var status: TestStatus;
    var error: string?;
    var duration: time.Duration;
}

class TestSuite {
    var name: string;
    var tests: Array<TestCase>;
    var beforeEach: Function?;
    var afterEach: Function?;
    var beforeAll: Function?;
    var afterAll: Function?;
}

class TestResult {
    var suite: string;
    var test: string;
    var status: TestStatus;
    var error: string?;
    var duration: time.Duration;
}

class TestRunner {
    var suites: Array<TestSuite>;
    var results: Array<TestResult>;
    var verbose: bool;

    fn run() TestReport;
    fn runSuite(suite: TestSuite) Array<TestResult>;
}

class TestReport {
    var totalTests: int;
    var passed: int;
    var failed: int;
    var skipped: int;
    var duration: time.Duration;
    var results: Array<TestResult>;

    fn print() void;
    fn toJSON() json.JsonValue;
}

class Assertion {
    var context: string;

    fn equals(actual: any, expected: any) void;
    fn notEquals(actual: any, expected: any) void;
    fn isTrue(value: bool) void;
    fn isFalse(value: bool) void;
    fn isNull(value: any?) void;
    fn isNotNull(value: any?) void;
    fn throws(fn: Function) void;
    fn contains(haystack: string, needle: string) void;
    fn startsWith(str: string, prefix: string) void;
    fn endsWith(str: string, suffix: string) void;
}

// ============================================================================
// Global test registry
// ============================================================================

var currentSuite: TestSuite? = null;
var allSuites = Array<TestSuite>();

// ============================================================================
// Native function declarations
// ============================================================================

// Assertion helpers
fn assertEqual(actual: any, expected: any, message: string) void;
fn assertNotEqual(actual: any, expected: any, message: string) void;
fn assertTrue(value: bool, message: string) void;
fn assertFalse(value: bool, message: string) void;

// Test execution
fn runTests(suites: Array<TestSuite>) TestReport;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Define test suite
fn describe(name: string, fn: Function) void {
    val suite = TestSuite {
        name: name,
        tests: Array<TestCase>(),
        beforeEach: null,
        afterEach: null,
        beforeAll: null,
        afterAll: null
    };

    currentSuite = suite;
    fn();  // Run the suite definition
    currentSuite = null;

    allSuites.add(suite);
}

// Define test case
fn it(name: string, fn: Function) void {
    if currentSuite == null {
        panic("it() must be called within describe() block");
    }

    val test = TestCase {
        name: name,
        fn: fn,
        status: TestStatus.PASSED,
        error: null,
        duration: time.milliseconds(0)
    };

    currentSuite.tests.add(test);
}

// Skip test
fn skip(name: string, fn: Function) void {
    if currentSuite == null {
        panic("skip() must be called within describe() block");
    }

    val test = TestCase {
        name: name,
        fn: fn,
        status: TestStatus.SKIPPED,
        error: null,
        duration: time.milliseconds(0)
    };

    currentSuite.tests.add(test);
}

// Setup/teardown hooks
fn beforeEach(fn: Function) void {
    if currentSuite != null {
        currentSuite.beforeEach = fn;
    }
}

fn afterEach(fn: Function) void {
    if currentSuite != null {
        currentSuite.afterEach = fn;
    }
}

fn beforeAll(fn: Function) void {
    if currentSuite != null {
        currentSuite.beforeAll = fn;
    }
}

fn afterAll(fn: Function) void {
    if currentSuite != null {
        currentSuite.afterAll = fn;
    }
}

// Assertion builder
fn expect(value: any) Assertion {
    return Assertion { context: "" };
}

// Matchers
fn toBe(assertion: Assertion, expected: any) void {
    assertion.equals(assertion.context, expected);
}

fn toEqual(assertion: Assertion, expected: any) void {
    assertion.equals(assertion.context, expected);
}

fn toBeTrue(assertion: Assertion) void {
    assertion.isTrue(assertion.context);
}

fn toBeFalse(assertion: Assertion) void {
    assertion.isFalse(assertion.context);
}

fn toBeNull(assertion: Assertion) void {
    assertion.isNull(assertion.context);
}

fn toContain(assertion: Assertion, substring: string) void {
    assertion.contains(assertion.context, substring);
}

// Run all tests
fn runAllTests() TestReport {
    val runner = TestRunner {
        suites: allSuites,
        results: Array<TestResult>(),
        verbose: true
    };

    return runner.run();
}

// Run specific suite
fn runSuite(name: string) TestReport {
    var targetSuite: TestSuite? = null;

    for val suite in allSuites {
        if suite.name == name {
            targetSuite = suite;
            break;
        }
    }

    if targetSuite == null {
        log.error("Test suite not found: " + name);
        return TestReport {
            totalTests: 0,
            passed: 0,
            failed: 0,
            skipped: 0,
            duration: time.milliseconds(0),
            results: Array<TestResult>()
        };
    }

    val runner = TestRunner {
        suites: [targetSuite],
        results: Array<TestResult>(),
        verbose: true
    };

    return runner.run();
}

// Mock function builder
class Mock<T> {
    var calls: Array<Array<any>>;
    var returnValues: Array<T>;
    var implementation: Function?;

    constructor() {
        this.calls = Array<Array<any>>();
        this.returnValues = Array<T>();
        this.implementation = null;
    }

    fn mockReturnValue(value: T) Mock<T> {
        this.returnValues.add(value);
        return this;
    }

    fn mockImplementation(fn: Function) Mock<T> {
        this.implementation = fn;
        return this;
    }

    fn call(args: Array<any>) T {
        this.calls.add(args);

        if this.implementation != null {
            return this.implementation(args);
        }

        if this.returnValues.length() > 0 {
            return this.returnValues[0];
        }

        return null;
    }

    fn getCalls() Array<Array<any>> {
        return this.calls;
    }

    fn getCallCount() int {
        return this.calls.length();
    }

    fn wasCalledWith(args: Array<any>) bool {
        for val call in this.calls {
            var match = true;
            for i in 0..args.length() {
                if call[i] != args[i] {
                    match = false;
                    break;
                }
            }
            if match {
                return true;
            }
        }
        return false;
    }

    fn reset() void {
        this.calls = Array<Array<any>>();
    }
}

fn mock<T>() Mock<T> {
    return Mock<T>();
}

// Spy function wrapper
class Spy<T> {
    var originalFn: Function<T>;
    var calls: Array<Array<any>>;

    constructor(fn: Function<T>) {
        this.originalFn = fn;
        this.calls = Array<Array<any>>();
    }

    fn call(args: Array<any>) T {
        this.calls.add(args);
        return this.originalFn(args);
    }

    fn getCalls() Array<Array<any>> {
        return this.calls;
    }

    fn reset() void {
        this.calls = Array<Array<any>>();
    }
}

fn spy<T>(fn: Function<T>) Spy<T> {
    return Spy<T>(fn);
}

// Benchmark helper
class Benchmark {
    var name: string;
    var iterations: int;
    var results: Array<time.Duration>;

    constructor(testName: string, iters: int) {
        this.name = testName;
        this.iterations = iters;
        this.results = Array<time.Duration>();
    }

    fn run(fn: Function) void {
        for i in 0..this.iterations {
            val start = time.now();
            fn();
            val end = time.now();
            this.results.add(end.sub(start));
        }
    }

    fn report() void {
        val total = this.results.reduce(time.milliseconds(0), (acc, dur) => {
            return acc.add(dur);
        });

        val avg = total.milliseconds() / this.iterations;

        log.infoWith("Benchmark: " + this.name, {
            iterations: this.iterations,
            total_ms: total.milliseconds(),
            avg_ms: avg,
            ops_per_sec: 1000.0 / avg
        });
    }
}

fn benchmark(name: string, iterations: int, fn: Function) void {
    val bench = Benchmark(name, iterations);
    bench.run(fn);
    bench.report();
}

// Snapshot testing
class Snapshot {
    var snapshots: Map<string, string>;
    var snapshotFile: string;

    constructor(file: string) {
        this.snapshotFile = file;
        this.snapshots = collections.newMap<string, string>();
        this.load();
    }

    fn load() void {
        if io.exists(this.snapshotFile) {
            val content = io.readFile(this.snapshotFile);
            if content.ok() {
                val parsed = json.parse(content.unwrap());
                if parsed.ok() {
                    // Load snapshots
                }
            }
        }
    }

    fn save() void {
        val data = json.jsonObject(this.snapshots);
        io.writeFile(this.snapshotFile, json.pretty(data));
    }

    fn match(key: string, value: string) bool {
        if !this.snapshots.containsKey(key) {
            // Create new snapshot
            this.snapshots.put(key, value);
            this.save();
            return true;
        }

        return this.snapshots.get(key) == value;
    }
}

fn snapshot(file: string) Snapshot {
    return Snapshot(file);
}

// Test coverage helper
class Coverage {
    var files: Map<string, FileCoverage>;

    fn add(file: string, line: int) void;
    fn report() void;
}

class FileCoverage {
    var path: string;
    var lines: Set<int>;
    var totalLines: int;

    fn percentage() double {
        return (this.lines.length() * 100.0) / this.totalLines;
    }
}
