package main;

use io;
use collections;

fn printHeader(title: string) void {
    println("\n" + "=".repeat(50));
    println(title);
    println("=".repeat(50));
}

fn printSection(num: int, title: string) void {
    println("\n" + num + ". " + title);
    println("-".repeat(40));
}

// =================================================================
// FOR LOOPS
// =================================================================

// Iterate over array elements
fn forLoopOverArray() void {
    val fruits = "apple,banana,cherry,date,elderberry".split(",");

    println("Fruits list:");
    for val fruit in fruits {
        println("  - " + fruit);
    }
}

// Count elements in array using for loop
fn forLoopWithCounter() void {
    val colors = "red,green,blue,yellow,purple".split(",");
    var count = 0;

    for val color in colors {
        count = count + 1;
        println(count + ". " + color);
    }

    println("Total colors: " + count);
}

// Process array elements with accumulation
fn forLoopProcessing() void {
    val items = "apple,banana,cherry,date".split(",");
    var count = 0;
    var combined = "";

    println("Processing items:");
    for val item in items {
        count = count + 1;
        combined = combined + item + " ";
        println("  Item " + count + ": " + item);
    }

    println("Total items: " + count);
    println("Combined: " + combined);
}

// Search for element in array
fn forLoopSearch() void {
    val animals = "cat,dog,bird,fish,hamster".split(",");
    val searchFor = "bird";
    var found = false;

    println("Searching for: " + searchFor);
    for val animal in animals {
        println("  Checking: " + animal);
        if animal == searchFor {
            found = true;
            println("  ✓ Found it!");
        }
    }

    if !found {
        println("  ✗ Not found");
    }
}

// Nested for loops - process multiple arrays
fn nestedForLoops() void {
    val row1 = "A1,A2,A3".split(",");
    val row2 = "B1,B2,B3".split(",");
    val row3 = "C1,C2,C3".split(",");

    println("Grid (3x3):");

    println("Row 1:");
    for val cell in row1 {
        println("  " + cell);
    }

    println("Row 2:");
    for val cell in row2 {
        println("  " + cell);
    }

    println("Row 3:");
    for val cell in row3 {
        println("  " + cell);
    }
}

// For loop over map keys
fn forLoopOverMap() void {
    val config = collections.newMap<string, string>();
    config.put("host", "localhost");
    config.put("port", "8080");
    config.put("database", "mydb");
    config.put("timeout", "30");

    println("Configuration:");
    for val entry in config.entries() {
        val key = entry.first;
        val value = entry.second;
        println("  " + key + " = " + value);
    }
}

// For loop with file processing
fn forLoopFileProcessing() void {
    val content = "line 1\nline 2\nline 3\nline 4\nline 5";
    io.writeFile("test_lines.txt", content);

    val fileContent = io.readFile("test_lines.txt").unwrap();
    val lines = fileContent.split("\n");

    println("File contents:");
    var lineNum = 0;
    for val line in lines {
        lineNum = lineNum + 1;
        println("  " + lineNum + ": " + line);
    }

    io.remove("test_lines.txt");
    println("✓ Cleaned up test file");
}

// =================================================================
// WHILE LOOPS (for comparison)
// =================================================================

// Simple while loop counting
fn whileLoopCounter() void {
    var i = 1;
    println("Counting with while:");
    while i <= 5 {
        println("  " + i);
        i = i + 1;
    }
}

// While loop with condition
fn whileLoopUntilCondition() void {
    var value = 2;
    var iterations = 0;

    println("Doubling until >= 100:");
    while value < 3 {
        value = value * 2;
        iterations = iterations + 1;
        println("  Iteration " + iterations + ": " + value);
    }

    println("Reached " + value + " in " + iterations + " iterations");
}

// While loop for factorial
fn whileLoopFactorial() void {
    var n = 5;
    var result = 1;
    var i = 1;

    println("Calculating " + n + "! (factorial):");
    while i <= n {
        result = result * i;
        println("  " + i + "! = " + result);
        i = i + 1;
    }

    println("Final: " + n + "! = " + result);
}

// =================================================================
// COMPARISON: For vs While
// =================================================================

fn forVsWhileComparison() void {
    val numbers = "10,20,30,40,50".split(",");

    println("Using FOR loop:");
    for val num in numbers {
        println("  " + num);
    }

    println("\nUsing WHILE loop (equivalent):");
    var i = 0;
    while i < numbers.length() {
        val num = numbers[i];
        println("  " + num);
        i = i + 1;
    }
}

// =================================================================
// MAIN FUNCTION
// =================================================================

fn main() void {
    println("DEBUG: main() started");

    printHeader("Stratos Loop Examples");

    // FOR LOOPS
    printHeader("FOR LOOPS - Iterate over collections");

    printSection(1, "For Loop - Array Iteration");
    forLoopOverArray();

    printSection(2, "For Loop - With Counter");
    forLoopWithCounter();

    printSection(3, "For Loop - Processing & Accumulation");
    forLoopProcessing();

    printSection(4, "For Loop - Search Pattern");
    forLoopSearch();

    printSection(5, "Nested For Loops");
    nestedForLoops();

    printSection(6, "For Loop - Over Map Keys");
    forLoopOverMap();

    printSection(7, "For Loop - File Processing");
    forLoopFileProcessing();

    // WHILE LOOPS
    printHeader("WHILE LOOPS - Condition-based iteration");

    printSection(8, "While Loop - Simple Counter");
    whileLoopCounter();

    printSection(9, "While Loop - Until Condition");
    whileLoopUntilCondition();

    printSection(10, "While Loop - Factorial Calculation");
    whileLoopFactorial();

    // COMPARISON
    printHeader("FOR vs WHILE - When to use each");

    printSection(11, "Comparison: Same Task, Different Approaches");
    forVsWhileComparison();

    println("\n" + "=".repeat(50));
    println("Key Takeaways:");
    println("=".repeat(50));
    println("✓ FOR loops: Use when iterating over collections");
    println("  - Cleaner syntax for arrays and maps");
    println("  - Automatic element access");
    println("  - No index management needed");
    println("");
    println("✓ WHILE loops: Use for condition-based iteration");
    println("  - More control over loop logic");
    println("  - Better for complex conditions");
    println("  - Useful when iteration count unknown");
    println("=".repeat(50));
}
