// Example: File I/O Operations
// Demonstrates the io module

package main;

use io;
use log;
use strings;
use async;

fn main() {
    log.info("STARTING MAIN");
    log.info("=== File I/O Example ===");

    // Write to file
    val content = "Hello, Stratos!\nThis is a test file.\nLine 3";
    log.info("About to write file...");
    val writeSuccess = io.writeFile("test.txt", content);

    if writeSuccess {
        log.info("File written successfully");
    } else {
        log.error("Failed to write file");
        return;
    }

    // Read from file
    log.info("About to read file...");
    val fileContent = io.readFile("test.txt");
    log.info("File content:");
    log.info(fileContent);

    // Read lines
    log.info("About to read lines...");
    val lines = io.readLines("test.txt");
    log.info("File has " + lines.length() + " lines:");

    for val i in 0..lines.length() {
        log.info("Line " + i + ": " + lines[i]);
    }

    // Append to file
    log.info("About to append lines...");
    io.appendLine("test.txt", "Appended line 4");
    io.appendLine("test.txt", "Appended line 5");
    log.info("Lines appended");

    // File info
    if io.exists("test.txt") {
        log.info("test.txt exists");
        log.info("Is file: " + io.isFile("test.txt"));

        val size = io.fileSize("test.txt");
        log.info("File size: " + size + " bytes");
    }

    // Directory operations
    log.info("About to create directory...");
    val mkdirSuccess = io.mkdir("test_dir");

    if mkdirSuccess {
        log.info("Directory created successfully");

        // Copy file to directory
        val copySuccess = io.copy("test.txt", "test_dir/copy.txt");

        if copySuccess {
            log.info("File copied successfully");
        } else {
            log.error("Failed to copy file");
        }

        // List directory
        val files = io.readDir("test_dir");
        log.info("Files in test_dir:");

        for val file in files {
            log.infoWith("File info", {
                name: file.fileName,
                size: file.size,
                isDir: file.isDir
            });
        }
    } else {
        log.error("Failed to create directory");
    }

    // Clean up
    await async.delay(100);
    log.info("About to clean up...");
    io.remove("test.txt");
    io.removeDirAll("test_dir");
    log.info("Cleanup complete");
}
