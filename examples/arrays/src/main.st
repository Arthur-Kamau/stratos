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
    printHeader("Stratos Array Operations Demo");

    // =================================================================
    // 1. Creating Arrays from Strings
    // =================================================================
    printSection(1, "Creating Arrays from String Split");

    val text = "apple,banana,cherry,date,elderberry";
    val fruits = text.split(",");

    println("Original text: " + text);
    println("Split by comma into array");
    println("Array length: " + fruits.length());
    println("First fruit: " + fruits[0]);
    println("Last fruit: " + fruits[4]);

    // =================================================================
    // 2. Array Indexing
    // =================================================================
    printSection(2, "Array Indexing and Access");

    val colors = "red,green,blue,yellow,purple".split(",");

    println("Colors array has " + colors.length() + " elements");
    println("");
    println("Accessing each element:");
    println("  [0] = " + colors[0]);
    println("  [1] = " + colors[1]);
    println("  [2] = " + colors[2]);
    println("  [3] = " + colors[3]);
    println("  [4] = " + colors[4]);

    // =================================================================
    // 3. Working with Different Delimiters
    // =================================================================
    printSection(3, "Different Split Delimiters");

    // Split by spaces
    val sentence = "The quick brown fox jumps";
    val words = sentence.split(" ");
    println("Sentence: " + sentence);
    println("Words count: " + words.length());
    println("First word: " + words[0]);
    println("Last word: " + words[4]);

    // Split by newlines
    val multiline = "Line 1\nLine 2\nLine 3";
    val lines = multiline.split("\n");
    println("\nMultiline text has " + lines.length() + " lines");
    println("First line: " + lines[0]);

    // Split by pipes
    val data = "name|age|city|country";
    val fields = data.split("|");
    println("\nData fields: " + fields.length());
    println("Field 0: " + fields[0]);
    println("Field 1: " + fields[1]);

    // =================================================================
    // 4. Empty and Single Element Arrays
    // =================================================================
    printSection(4, "Edge Cases: Empty and Single Elements");

    val single = "only-one-item".split(",");
    println("Single item (no delimiter found):");
    println("  Length: " + single.length());
    println("  Item: " + single[0]);

    val withEmpty = "a,,b".split(",");
    println("\nArray with empty elements (a,,b):");
    println("  Length: " + withEmpty.length());
    println("  [0]: '" + withEmpty[0] + "'");
    println("  [1]: '" + withEmpty[1] + "' (empty)");
    println("  [2]: '" + withEmpty[2] + "'");

    // =================================================================
    // 5. Array Length Method
    // =================================================================
    printSection(5, "Array Length Method");

    val small = "a,b".split(",");
    val medium = "1,2,3,4,5".split(",");
    val large = "a,b,c,d,e,f,g,h,i,j".split(",");

    println("Small array length: " + small.length());
    println("Medium array length: " + medium.length());
    println("Large array length: " + large.length());

    // =================================================================
    // 6. Practical Example: CSV Processing
    // =================================================================
    printSection(6, "Practical Example: CSV Row Processing");

    val csvHeader = "Name,Age,City,Occupation";
    val csvRow1 = "Alice,30,NYC,Engineer";
    val csvRow2 = "Bob,25,LA,Designer";
    val csvRow3 = "Charlie,35,SF,Manager";

    val headers = csvHeader.split(",");
    val row1 = csvRow1.split(",");
    val row2 = csvRow2.split(",");
    val row3 = csvRow3.split(",");

    println("CSV Data Processing:");
    println("");
    println("Headers (" + headers.length() + " columns):");
    println("  " + headers[0] + " | " + headers[1] + " | " + headers[2] + " | " + headers[3]);
    println("");
    println("Row 1:");
    println("  " + row1[0] + " | " + row1[1] + " | " + row1[2] + " | " + row1[3]);
    println("Row 2:");
    println("  " + row2[0] + " | " + row2[1] + " | " + row2[2] + " | " + row2[3]);
    println("Row 3:");
    println("  " + row3[0] + " | " + row3[1] + " | " + row3[2] + " | " + row3[3]);

    // =================================================================
    // 7. Working with File Data
    // =================================================================
    printSection(7, "Processing File Contents as Arrays");

    // Create a test file with multiple lines
    val testData = "Monday\nTuesday\nWednesday\nThursday\nFriday\nSaturday\nSunday";
    io.writeFile("days.txt", testData);
    println("✓ Created days.txt with 7 days");

    // Read and process
    val fileContent = io.readFile("days.txt");
    val days = fileContent.split("\n");

    println("Days in the file: " + days.length());
    println("First day: " + days[0]);
    println("Midweek: " + days[2]);
    println("Weekend starts: " + days[5]);

    // Cleanup
    io.remove("days.txt");
    println("✓ Cleaned up days.txt");

    // =================================================================
    // 8. Array Use Cases
    // =================================================================
    printSection(8, "Common Array Use Cases");

    // Parsing command output
    val path = "/usr/local/bin:/usr/bin:/bin";
    val directories = path.split(":");
    println("PATH parsing:");
    println("  Total directories: " + directories.length());
    println("  First directory: " + directories[0]);

    // Processing tags
    val tags = "stratos,programming,arrays,tutorial,demo".split(",");
    println("\nBlog post tags: " + tags.length() + " tags");
    println("  Primary tag: " + tags[0]);
    println("  Last tag: " + tags[4]);

    // Parsing version strings
    val version = "1.2.3".split(".");
    println("\nVersion parsing:");
    println("  Major: " + version[0]);
    println("  Minor: " + version[1]);
    println("  Patch: " + version[2]);

    // =================================================================
    // Summary
    // =================================================================
    printHeader("Array Operations Summary");

    println("\n✓ Array Creation: Using string.split(delimiter)");
    println("✓ Array Access: Using array[index] syntax");
    println("✓ Array Length: Using array.length() method");
    println("✓ Array Types: Currently supporting string arrays");
    println("✓ Use Cases: CSV parsing, file processing, data splitting");

    println("\n" + "=".repeat(50));
    println("All array operations completed successfully!");
    println("=".repeat(50));
}
