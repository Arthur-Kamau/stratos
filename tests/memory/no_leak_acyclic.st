package main;

// Test case: Acyclic data structure (tree)
// This should NOT leak memory

class TreeNode {
    var value: int;

    constructor(v: int) {
        this.value = v;
    }
}

fn main() {
    // Create a simple tree with no cycles
    val root = TreeNode(1);
    val left = TreeNode(2);
    val right = TreeNode(3);
    val leftLeft = TreeNode(4);
    val leftRight = TreeNode(5);

    // All nodes should be freed when main exits
    // EXPECTED: No memory leak
    println(root.value);
}
