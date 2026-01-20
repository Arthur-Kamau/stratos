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

    when (writeResult) {
        Result.Ok(_) -> {
            log.info("✓ File written successfully");
        }
        Result.Err(error) -> {
            log.error("Failed to write file: " + error.message);
            return;
        }
    }

    // Read from file
    val readResult = io.readFile("test.txt");

    when (readResult) {
        Result.Ok(content) -> {
            log.info("✓ File content:");
            log.info(content);
        }
        Result.Err(error) -> {
            log.error("Failed to read file: " + error.message);
            return;
        }
    }

    // Read lines
    val linesResult = io.readLines("test.txt");

    when (linesResult) {
        Result.Ok(lines) -> {
            log.info("✓ File has " + lines.length() + " lines:");

            for val i in 0..lines.length() {
                log.info("  Line " + i + ": " + lines[i]);
            }
        }
        Result.Err(error) -> {
            log.error("Failed to read lines: " + error.message);
        }
    }

    // Append to file
    val append1 = io.appendLine("test.txt", "Appended line 4");
    val append2 = io.appendLine("test.txt", "Appended line 5");

    when (append1) {
        Result.Ok(_) -> when (append2) {
            Result.Ok(_) -> log.info("✓ Lines appended")
            Result.Err(error) -> log.error("Failed to append: " + error.message)
        }
        Result.Err(error) -> log.error("Failed to append: " + error.message)
    }

    // File info
    if io.exists("test.txt") {
        log.info("✓ test.txt exists");
        log.info("  Is file: " + io.isFile("test.txt"));

        val sizeResult = io.fileSize("test.txt");
        when (sizeResult) {
            Result.Ok(size) -> {
                log.info("  File size: " + size + " bytes");
            }
            Result.Err(error) -> {
                log.error("Failed to get file size: " + error.message);
            }
        }
    }

    // Directory operations
    val mkdirResult = io.mkdir("test_dir");

    when (mkdirResult) {
        Result.Ok(_) -> {
            log.info("✓ Directory created successfully");

            // Copy file to directory
            val copyResult = io.copy("test.txt", "test_dir/copy.txt");

            when (copyResult) {
                Result.Ok(_) -> {
                    log.info("✓ File copied successfully");
                }
                Result.Err(error) -> {
                    log.error("Failed to copy file: " + error.message);
                }
            }

            // List directory
            val dirResult = io.readDir("test_dir");

            when (dirResult) {
                Result.Ok(files) -> {
                    log.info("✓ Files in test_dir:");

                    for val file in files {
                        log.infoWith("  File", {
                            name: file.fileName,
                            size: file.size,
                            isDir: file.isDir
                        });
                    }
                }
                Result.Err(error) -> {
                    log.error("Failed to read directory: " + error.message);
                }
            }
        }
        Result.Err(error) -> {
            log.error("Failed to create directory: " + error.message);
        }
    }

    // Clean up
    log.info("\nCleaning up...");
    io.remove("test.txt");
    io.removeDirAll("test_dir");
    log.info("✓ Cleanup complete");
}
