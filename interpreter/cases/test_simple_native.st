// Simple native function test
package main;

use math;
use log;

fn main() {
    log.info("Testing native math.sqrt...");
    val result = math.sqrt(16.0);
    log.info("Result should be 4.0");
}
