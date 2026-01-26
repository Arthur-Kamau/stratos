// Test using a module that has internal method calls

package main;

use log;
use myhttp;

fn main() {
    log.info("=== Method Test 3 ===");
    myhttp.serve(8080, "handler");
    log.info("=== Test Complete ===");
}
