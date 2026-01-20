// Example: File I/O Operations
// Demonstrates the io module with Result types

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
        log.info("✓ File written successfully");
    } else {
        val err: io.Error = writeResult.err();
        log.error("Failed to write file: " + err.message);
        return;
    }

    // Read from file
    val readResult = io.readFile("test.txt");

    if readResult.ok() {
        log.info("✓ File content:");
        val content: string = readResult.unwrap();
        log.info(content);
    } else {
        val err: io.Error = readResult.err();
        log.error("Failed to read file: " + err.message);
        return;
    }

    // Read lines
    val lines = io.readLines("test.txt");
    log.info("✓ File has " + lines.length() + " lines:");

    for val i in 0..lines.length() {
        log.info("  Line " + i + ": " + lines[i]);
    }

    // Append to file
    val append1 = io.appendLine("test.txt", "Appended line 4");
    val append2 = io.appendLine("test.txt", "Appended line 5");

    if append1.ok() and append2.ok() {
        log.info("✓ Lines appended");
    } else {
        log.error("Failed to append lines");
    }

    // File info
    if io.exists("test.txt") {
        log.info("✓ test.txt exists");
        log.info("  Is file: " + io.isFile("test.txt"));

        val sizeResult = io.fileSize("test.txt");
        if sizeResult.ok() {
            log.info("  File size: " + sizeResult.unwrap() + " bytes");
        }
    }

    // Directory operations
    val mkdirResult = io.mkdir("test_dir");

    if mkdirResult.ok() {
        log.info("✓ Directory created successfully");

        // Copy file to directory
        val copyResult = io.copy("test.txt", "test_dir/copy.txt");

        if copyResult.ok() {
            log.info("✓ File copied successfully");
        } else {
            val err: io.Error = copyResult.err();
            log.error("Failed to copy file: " + err.message);
        }

        // List directory
        val dirResult = io.readDir("test_dir");

        if dirResult.ok() {
            val files = dirResult.unwrap();
            log.info("✓ Files in test_dir:");

            for val file in files {
                log.infoWith("  File", {
                    name: file.fileName,
                    size: file.size,
                    isDir: file.isDir
                });
            }
        } else {
            val err: io.Error = dirResult.err();
            log.error("Failed to read directory: " + err.message);
        }
    } else {
        val err: io.Error = mkdirResult.err();
        log.error("Failed to create directory: " + err.message);
    }

    // Clean up
    log.info("\nCleaning up...");
    io.remove("test.txt");
    io.removeDirAll("test_dir");
    log.info("✓ Cleanup complete");
}
