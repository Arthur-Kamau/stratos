package main;

use io;

fn printUtilsHeader(title: string) {
    println("\n" + "=" . repeat(50));
    println(title);
    println("=" . repeat(50));
}

fn printUtilsSection(num: int, title: string) {
    println("\n" + num + ". " + title);
    println("-" . repeat(40));
}

fn demoUtils() {
    printUtilsHeader("Array Utility Functions Demo");
    printUtilsSection(1, "isEmpty(), first(), and last()");
    val fruits = "apple,banana,cherry,date,elderberry" . split(",");
    println("Fruits array: " + fruits . length() + " items");
    println("  isEmpty(): " + fruits . isEmpty());
    println("  first(): " + fruits . first());
    println("  last(): " + fruits . last());
    val empty = "" . split(",");
    empty = empty . clear();
    println("\nEmpty array:");
    println("  isEmpty(): " + empty . isEmpty());
    println("  first(): '" + empty . first() + "' (returns empty string)");
    println("  last(): '" + empty . last() + "' (returns empty string)");
    printUtilsSection(2, "contains() and indexOf()");
    val colors = "red,green,blue,yellow,purple" . split(",");
    println("Colors: " + colors . join(", "));
    println("\nSearching for values:");
    println("  contains('blue'): " + colors . contains("blue"));
    println("  indexOf('blue'): " + colors . indexOf("blue"));
    println("  contains('orange'): " + colors . contains("orange"));
    println("  indexOf('orange'): " + colors . indexOf("orange") + " (returns -1 if not found)");
    printUtilsSection(3, "reverse()");
    val numbers = "1,2,3,4,5" . split(",");
    val reversed = numbers . reverse();
    println("Original: " + numbers . join(" -> "));
    println("Reversed: " + reversed . join(" -> "));
    val words = "Hello,World,from,Stratos" . split(",");
    println("\nOriginal words: " + words . join(" "));
    println("Reversed words: " + words . reverse() . join(" "));
    printUtilsSection(4, "join() with custom separators");
    val items = "laptop,phone,tablet,mouse,keyboard" . split(",");
    println("Default join (comma): " + items . join(","));
    println("Space separated: " + items . join(" "));
    println("Pipe separated: " + items . join(" | "));
    println("Arrow separated: " + items . join(" -> "));
    println("Newline separated:\n" + items . join("\n"));
    printUtilsSection(5, "clear()");
    val data = "a,b,c,d,e" . split(",");
    println("Before clear:");
    println("  Size: " + data . length());
    println("  First: " + data . first());
    val cleared = data . clear();
    println("\nAfter clear:");
    println("  Size: " + cleared . length());
    println("  isEmpty: " + cleared . isEmpty());
    printUtilsSection(6, "Method Chaining");
    val text = "the,quick,brown,fox,jumps" . split(",");
    val processed = text . reverse();
    val result = processed . join(" ");
    println("Original: " + text . join(" "));
    println("Process: reverse -> join with space");
    println("Result: " + result);
    printUtilsSection(7, "Practical Example: Log Processing");
    val logLines = "INFO,WARNING,ERROR,INFO,DEBUG,ERROR,INFO" . split(",");
    println("Total log entries: " + logLines . length());
    println("First log level: " + logLines . first());
    println("Last log level: " + logLines . last());
    println("\nSearching logs:");
    println("  Contains ERROR: " + logLines . contains("ERROR"));
    println("  First ERROR at index: " + logLines . indexOf("ERROR"));
    println("  Contains FATAL: " + logLines . contains("FATAL"));
    println("\nAll log levels: " + logLines . join(" -> "));
    printUtilsSection(8, "Practical Example: Path Manipulation");
    val path = "/usr/local/bin/stratos" . split("/");
    println("Path segments: " + path . length());
    println("  First segment: '" + path . first() + "' (empty for leading /)");
    println("  Last segment (filename): " + path . last());
    println("  Contains 'local': " + path . contains("local"));
    val reversePath = path . reverse();
    println("\nReverse path: " + reversePath . join("/"));
    printUtilsHeader("Array Utilities Summary");
    println("\n✓ isEmpty() - Check if array is empty");
    println("✓ first() - Get first element");
    println("✓ last() - Get last element");
    println("✓ clear() - Return empty array");
    println("✓ contains(value) - Check if value exists");
    println("✓ indexOf(value) - Find index of value (-1 if not found)");
    println("✓ reverse() - Reverse array order");
    println("✓ join(separator) - Join elements into string");
    println("\n" + "=" . repeat(50));
    println("All array utility functions work perfectly!");
    println("=" . repeat(50));
}
