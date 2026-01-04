package main;

use io;


fn checkPathType(path: string) void {
    if io.exists(path) {
        println("✓ " + path + " exists");
    } else {
        println("✗ " + path + " not found");
    }
}



fn main() {
    println("=== Stratos Error Handling Example ===");
    println("Demonstrating safe file operations\n");

    // Example 1: File exists check before reading
    println("1. Safe file reading:");
    val path1 = "examples/expect/src/main.st";

    if io.exists(path1) {
        println("✓ File exists: " + path1);
        val content = io.readFile(path1);
        val lines = content.split("\n");
        println("  Lines: " + lines.length());
        println("  First line: " + lines[0]);
    } else {
        println("✗ File not found: " + path1);
    }

    // Example 2: Handling non-existent files
    println("\n2. Attempting to read non-existent file:");
    val path2 = "nonexistent.txt";

    if io.exists(path2) {
        val content = io.readFile(path2);
        println("Content: " + content);
    } else {
        println("✗ File not found: " + path2);
        println("  Safely handled without crash!");
    }

    // Example 3: Safe file writing
    println("\n3. Writing and reading a file:");
    val testFile = "test_output.txt";
    val testContent = "Hello from Stratos!\nThis is a test file.\nError handling works!";

    io.writeFile(testFile, testContent);
    println("✓ Wrote to " + testFile);

    if io.exists(testFile) {
        val readContent = io.readFile(testFile);
        println("✓ Read back from file:");
        println(readContent);

        // Clean up
        io.remove(testFile);
        println("✓ Cleaned up test file");
    }

    // Example 4: Directory checks
    println("\n4. Checking directories:");
    checkPathType(".");
    checkPathType("examples");
    checkPathType("src");
    checkPathType("nonexistent_dir");

    // Example 5: File vs Directory
    println("\n5. Distinguishing files and directories:");
    checkFileType("examples/expect/stratos.conf");
    checkFileType("examples/expect/src");
    checkFileType("README.md");

    println("\n=== All checks completed successfully ===");
}
fn checkFileType(path: string) void {
    if !io.exists(path) {
        println("✗ " + path + " - not found");
        return;
    }

    if io.isDirectory(path) {
        println("✓ " + path + " - directory");
    } else if io.isFile(path) {
        println("✓ " + path + " - file");
    } else {
        println("? " + path + " - unknown type");
    }
}
