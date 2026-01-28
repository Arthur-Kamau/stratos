// Defer Statement Example
// Demonstrates Go-style defer for cleanup operations

package main;

fn cleanup() {
    println("Cleanup executed!");
}

fn processData() {
    println("Starting data processing...");
    defer cleanup();
    println("Processing data...");
    println("Data processing complete.");
    return;
}

fn multipleDefers() {
    println("Testing multiple defers (LIFO order):");
    defer println("First defer (executed last)");
    defer println("Second defer (executed second)");
    defer println("Third defer (executed first)");
    println("Function body complete.");
    return;
}

fn deferWithValue() {
    val resource = "Database Connection";
    println("Opening: $resource");
    defer println("Closing: $resource");
    println("Using: $resource");
    return;
}

fn main() {
    println("=== Defer Statement Demo ===");
    println("");

    println("1. Basic defer with cleanup:");
    processData();
    println("");

    println("2. Multiple defers (LIFO order):");
    multipleDefers();
    println("");

    println("3. Defer with captured value:");
    deferWithValue();
    println("");

    println("=== Defer Demo Complete ===");
}
