package main;

// Test case: Circular reference with GC
// The mark-and-sweep GC should break the cycle

class Node {
    var value: int;
    var next: Node;

    constructor(v: int) {
        this.value = v;
    }

    fn setNext(n: Node) {
        this.next = n;
    }
}

fn createCycle() {
    // Create circular reference in local scope
    val a = Node(1);
    val b = Node(2);

    println(a.value);
    println(b.value);

    // Create the cycle: a -> b -> a
    a.setNext(b);
    b.setNext(a);

    println(a.value);  // Verify objects still work after cycle
    println(b.value);

    // When function exits, a and b go out of scope
    // but the circular reference keeps them alive
    // GC should detect this unreachable cycle
}

fn main() {
    createCycle();
    // After createCycle returns, a and b are no longer in scope
    // but they reference each other, forming an unreachable cycle
    // GC should detect and break this cycle
    // EXPECTED: No leak with GC enabled
}
