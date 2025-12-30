// Test native function calls
package main;

use math;
use log;
use strings;
use csv;
use crypto;

fn main() {
    // Test math functions
    log.info("Testing math.sqrt(16.0)...");
    val sqrtResult = math.sqrt(16.0);
    log.info("Result: " + sqrtResult);

    // Test math.sin
    log.info("Testing math.sin(0.0)...");
    val sinResult = math.sin(0.0);
    log.info("Result: " + sinResult);

    // Test strings functions
    log.info("Testing strings.toUpper('hello')...");
    val upperResult = strings.toUpper("hello");
    log.info("Result: " + upperResult);

    // Test strings.length
    log.info("Testing strings.length('test')...");
    val lenResult = strings.length("test");
    log.info("Result: " + lenResult);

    // Test CSV parsing
    log.info("Testing csv.parse...");
    val csvData = "name,age\nAlice,30\nBob,25";
    val records = csv.parse(csvData);
    log.info("Parsed CSV successfully");

    // Test crypto.randomHex
    log.info("Testing crypto.randomHex(16)...");
    val hexResult = crypto.randomHex(16);
    log.info("Random hex: " + hexResult);

    log.info("All native function tests completed!");
}
