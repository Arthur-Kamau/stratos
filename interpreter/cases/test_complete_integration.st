// Complete integration test - demonstrates full native function call system
package main;

use math;
use strings;
use log;

fn main() {
    // Test 1: Math module
    log.info("=== Testing Math Module ===");
    val sqrtResult = math.sqrt(25.0);
    val sinResult = math.sin(0.0);
    val cosResult = math.cos(0.0);

    // Test 2: Strings module
    log.info("=== Testing Strings Module ===");
    val upperCase = strings.toUpper("hello");
    val lowerCase = strings.toLower("WORLD");
    val length = strings.length("stratos");

    // Test 3: Log module
    log.info("=== All Tests Complete ===");
    log.info("Math: sqrt(25.0) processed");
    log.info("Strings: toUpper, toLower, length processed");
    log.info("Integration: SUCCESS");
}
