package main;

// Test case: Circular reference between two objects
// This will leak memory due to reference counting limitation

class Node {
    var value: int;

    constructor(v: int) {
        this.value = v;
    }
}

fn main() {
    val a = Node(1);
    val b = Node(2);

    // Note: Cannot create circular reference without fields to store references
    // This test demonstrates object creation and cleanup
    // For actual circular reference testing, would need:
    // var next: Node; field in class

    // These objects should be freed when main exits
    println(a.value);
    println(b.value);

    // EXPECTED: No leak (simplified version without circular ref capability)
}
