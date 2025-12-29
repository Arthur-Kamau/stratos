package main;

// Test case: Simple object creation and cleanup
// No circular references - should NOT leak

class Counter {
    var count: int;

    constructor(initial: int) {
        this.count = initial;
    }

    fn increment() {
        this.count += 1;
    }

    fn getValue() int {
        return this.count;
    }
}

fn main() {
    // Create multiple objects
    val c1 = Counter(0);
    val c2 = Counter(10);
    val c3 = Counter(20);

    c1.increment();
    c2.increment();
    c3.increment();

    println(c1.getValue());
    println(c2.getValue());
    println(c3.getValue());

    // All objects should be freed when main exits
    // EXPECTED: No memory leak
}
