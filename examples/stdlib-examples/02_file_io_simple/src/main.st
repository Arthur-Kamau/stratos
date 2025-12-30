// Example: Simple File I/O Operations
// Demonstrates basic io module functions

package main;

use io;
use log;

fn main() {
    log.info("=== Simple File I/O Example ===");

    // Write to file
    val content = "Hello, Stratos!";
    io.writeFile("test.txt", content);
    log.info("File written successfully");

    // Read from file
    val fileContent = io.readFile("test.txt");
    log.info("File content:");
    log.info(fileContent);

    // Check if file exists
    if (io.exists("test.txt")) {
        log.info("test.txt exists");
        log.info("Is file: " + io.isFile("test.txt"));
    }

    // Clean up
    io.remove("test.txt");
    log.info("Cleanup complete");
}
