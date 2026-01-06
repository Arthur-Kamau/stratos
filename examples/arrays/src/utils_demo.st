package main;

use io;

fn printHeader(title: string) void {
    println("\n" + "=".repeat(50));
    println(title);
    println("=".repeat(50));
}

fn printSection(num: int, title: string) void {
    println("\n" + num + ". " + title);
    println("-".repeat(40));
}

fn main() {
    printHeader("Array Utility Functions Demo");

    // =================================================================
    // 1. isEmpty, first, last
    // =================================================================
    printSection(1, "isEmpty(), first(), and last()");

    val fruits = "apple,banana,cherry,date,elderberry".split(",");

    println("Fruits array: " + fruits.length() + " items");
    println("  isEmpty(): " + fruits.isEmpty());
    println("  first(): " + fruits.first());
    println("  last(): " + fruits.last());

    val empty = "".split(",");
    empty = empty.clear();  // Create truly empty array
    println("\nEmpty array:");
    println("  isEmpty(): " + empty.isEmpty());
    println("  first(): '" + empty.first() + "' (returns empty string)");
    println("  last(): '" + empty.last() + "' (returns empty string)");

    // =================================================================
    // 2. contains and indexOf
    // =================================================================
    printSection(2, "contains() and indexOf()");

    val colors = "red,green,blue,yellow,purple".split(",");

    println("Colors: " + colors.join(", "));
    println("\nSearching for values:");
    println("  contains('blue'): " + colors.contains("blue"));
    println("  indexOf('blue'): " + colors.indexOf("blue"));
    println("  contains('orange'): " + colors.contains("orange"));
    println("  indexOf('orange'): " + colors.indexOf("orange") + " (returns -1 if not found)");

    // =================================================================
    // 3. reverse
    // =================================================================
    printSection(3, "reverse()");

    val numbers = "1,2,3,4,5".split(",");
    val reversed = numbers.reverse();

    println("Original: " + numbers.join(" -> "));
    println("Reversed: " + reversed.join(" -> "));

    val words = "Hello,World,from,Stratos".split(",");
    println("\nOriginal words: " + words.join(" "));
    println("Reversed words: " + words.reverse().join(" "));

    // =================================================================
    // 4. join with different separators
    // =================================================================
    printSection(4, "join() with custom separators");

    val items = "laptop,phone,tablet,mouse,keyboard".split(",");

    println("Default join (comma): " + items.join(","));
    println("Space separated: " + items.join(" "));
    println("Pipe separated: " + items.join(" | "));
    println("Arrow separated: " + items.join(" -> "));
    println("Newline separated:\n" + items.join("\n"));

    // =================================================================
    // 5. clear
    // =================================================================
    printSection(5, "clear()");

    val data = "a,b,c,d,e".split(",");
    println("Before clear:");
    println("  Size: " + data.length());
    println("  First: " + data.first());

    val cleared = data.clear();
    println("\nAfter clear:");
    println("  Size: " + cleared.length());
    println("  isEmpty: " + cleared.isEmpty());

    // =================================================================
    // 6. Chaining methods
    // =================================================================
    printSection(6, "Method Chaining");

    val text = "the,quick,brown,fox,jumps".split(",");
    val processed = text.reverse();
    val result = processed.join(" ");

    println("Original: " + text.join(" "));
    println("Process: reverse -> join with space");
    println("Result: " + result);

    // =================================================================
    // 7. Practical: Array utilities for data processing
    // =================================================================
    printSection(7, "Practical Example: Log Processing");

    val logLines = "INFO,WARNING,ERROR,INFO,DEBUG,ERROR,INFO".split(",");

    println("Total log entries: " + logLines.length());
    println("First log level: " + logLines.first());
    println("Last log level: " + logLines.last());

    println("\nSearching logs:");
    println("  Contains ERROR: " + logLines.contains("ERROR"));
    println("  First ERROR at index: " + logLines.indexOf("ERROR"));
    println("  Contains FATAL: " + logLines.contains("FATAL"));

    println("\nAll log levels: " + logLines.join(" -> "));

    // =================================================================
    // 8. Practical: File path manipulation
    // =================================================================
    printSection(8, "Practical Example: Path Manipulation");

    val path = "/usr/local/bin/stratos".split("/");

    println("Path segments: " + path.length());
    println("  First segment: '" + path.first() + "' (empty for leading /)");
    println("  Last segment (filename): " + path.last());
    println("  Contains 'local': " + path.contains("local"));

    val reversePath = path.reverse();
    println("\nReverse path: " + reversePath.join("/"));

    // =================================================================
    // Summary
    // =================================================================
    printHeader("Array Utilities Summary");

    println("\n✓ isEmpty() - Check if array is empty");
    println("✓ first() - Get first element");
    println("✓ last() - Get last element");
    println("✓ clear() - Return empty array");
    println("✓ contains(value) - Check if value exists");
    println("✓ indexOf(value) - Find index of value (-1 if not found)");
    println("✓ reverse() - Reverse array order");
    println("✓ join(separator) - Join elements into string");

    println("\n" + "=".repeat(50));
    println("All array utility functions work perfectly!");
    println("=".repeat(50));
}
