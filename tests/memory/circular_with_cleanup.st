package main;

// Test case: Large object allocation stress test
// Creates many objects to test memory cleanup

class Data {
    var value: int;

    constructor(v: int) {
        this.value = v;
    }
}

fn main() {
    // Allocate many objects
    val d1 = Data(1);
    val d2 = Data(2);
    val d3 = Data(3);
    val d4 = Data(4);
    val d5 = Data(5);
    val d6 = Data(6);
    val d7 = Data(7);
    val d8 = Data(8);
    val d9 = Data(9);
    val d10 = Data(10);

    println(d1.value + d10.value);

    // All objects should be freed when main exits
    // EXPECTED: No memory leak
}
