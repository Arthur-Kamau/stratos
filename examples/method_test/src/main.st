// Test: Method calls on module-defined classes

package main;

use mymodule;
use log;

fn main() {
    log.info("=== Method Resolution Test ===");

    // Test 1: Create object from module and call methods
    log.info("Test 1: Basic method call");
    val counter = mymodule.newCounter();
    counter.increment();
    counter.increment();
    val value = counter.getValue();
    log.info("Counter value: " + value);

    // Test 2: Method chaining
    log.info("Test 2: Method chaining");
    val builder = mymodule.newBuilder();
    val result = builder.append("Hello").append(" World").build();
    log.info("Built string: " + result);

    log.info("=== Tests Complete ===");
}
