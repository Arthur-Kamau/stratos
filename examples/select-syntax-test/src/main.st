// Select Syntax Test
// Tests the select statement syntax (Go-style channel multiplexing)

package main;

// Create a channel-like class that the runtime recognizes
class Channel {
    var __queue: array<any> = [];
    var closed: bool = false;
}

fn main() {
    println("=== Select Syntax Test ===");
    println("");

    // Create channels
    val ch1 = Channel {};
    val ch2 = Channel {};

    println("Testing select syntax parsing...");
    println("");

    // The select statement with cases
    // Note: The current runtime implementation checks channels non-blocking
    select {
        case msg <- ch1:
            println("Received from ch1: $msg");
        case msg <- ch2:
            println("Received from ch2: $msg");
        else:
            println("Default case executed (no channels ready)");
    }

    println("");
    println("=== Select Syntax Test Complete ===");
}
