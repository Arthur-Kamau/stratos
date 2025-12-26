// Example: File I/O Operations
// Demonstrates the io module

package main;

use io;
use log;
use strings;

fn main() {
    log.info("=== File I/O Example ===");

    // Write to file
    val content = "Hello, Stratos!\nThis is a test file.\nLine 3";
    val writeResult = io.writeFile("test.txt", content);

    if writeResult.ok() {
        log.info("File written successfully");
    } else {
        log.error("Failed to write file: " + writeResult.err().message);
        return;
    }

    // Read from file
    val readResult = io.readFile("test.txt");

    if readResult.ok() {
        log.info("File content:");
        log.info(readResult.unwrap());
    } else {
        log.error("Failed to read file: " + readResult.err().message);
        return;
    }

    // Read lines
    val linesResult = io.readLines("test.txt");

    if linesResult.ok() {
        val lines = linesResult.unwrap();
        log.info("File has " + lines.length() + " lines:");

        for i in 0..lines.length() {
            log.info("Line " + i + ": " + lines[i]);
        }
    }

    // Append to file
    io.appendLine("test.txt", "Appended line 4");
    io.appendLine("test.txt", "Appended line 5");

    // File info
    if io.exists("test.txt") {
        log.info("test.txt exists");
        log.info("Is file: " + io.isFile("test.txt"));

        val sizeResult = io.fileSize("test.txt");
        if sizeResult.ok() {
            log.info("File size: " + sizeResult.unwrap() + " bytes");
        }
    }

    // Directory operations
    val mkdirResult = io.mkdir("test_dir");

    if mkdirResult.ok() {
        log.info("Directory created successfully");

        // Copy file to directory
        val copyResult = io.copy("test.txt", "test_dir/copy.txt");

        if copyResult.ok() {
            log.info("File copied successfully");
        }

        // List directory
        val dirResult = io.readDir("test_dir");

        if dirResult.ok() {
            val files = dirResult.unwrap();
            log.info("Files in test_dir:");

            for val file in files {
                log.infoWith("File info", {
                    name: file.name,
                    size: file.size,
                    isDir: file.isDirectory
                });
            }
        }
    }

    // Clean up
    io.remove("test.txt");
    io.removeDirAll("test_dir");
    log.info("Cleanup complete");
}
