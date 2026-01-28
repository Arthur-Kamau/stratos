// Select Statement Example
// Demonstrates Go-style select for channel multiplexing

package main;

fn main() {
    println("=== Select Statement Demo ===");
    println("");

    // Create simple channel-like structures using arrays
    var ch1Data = [42, 100];
    var ch2Data = ["hello", "world"];
    var ch1Index = 0;
    var ch2Index = 0;

    println("1. Channel setup:");
    println("   ch1 has 2 messages: 42, 100");
    println("   ch2 has 2 messages: hello, world");
    println("");

    // Simulating select - receive from first available channel
    println("2. Select from channels (checking ch1 first):");
    if (ch1Index < 2) {
        val msg = ch1Data[ch1Index];
        ch1Index = ch1Index + 1;
        println("   Received from ch1: $msg");
    } else {
        if (ch2Index < 2) {
            val msg = ch2Data[ch2Index];
            ch2Index = ch2Index + 1;
            println("   Received from ch2: $msg");
        }
    }
    println("");

    // Select again
    println("3. Select again (checking ch2 first this time):");
    if (ch2Index < 2) {
        val msg = ch2Data[ch2Index];
        ch2Index = ch2Index + 1;
        println("   Received from ch2: $msg");
    } else {
        if (ch1Index < 2) {
            val msg = ch1Data[ch1Index];
            ch1Index = ch1Index + 1;
            println("   Received from ch1: $msg");
        }
    }
    println("");

    // Drain remaining messages
    println("4. Draining remaining messages:");
    for val i in 0..2 {
        if (ch1Index < 2) {
            val msg = ch1Data[ch1Index];
            ch1Index = ch1Index + 1;
            println("   ch1: $msg");
        }
        if (ch2Index < 2) {
            val msg = ch2Data[ch2Index];
            ch2Index = ch2Index + 1;
            println("   ch2: $msg");
        }
    }
    println("");

    // Select with default case
    println("5. Select with no data (default case):");
    if (ch1Index < 2) {
        println("   Received from ch1");
    } else {
        if (ch2Index < 2) {
            println("   Received from ch2");
        } else {
            println("   No data available - executing default case");
        }
    }
    println("");

    println("=== Select Demo Complete ===");
}
