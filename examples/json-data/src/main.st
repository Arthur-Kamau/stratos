package main;

use log;
use math;
use convert;

fn main() {
    log.info("Hello from json-data example!");

    val result = math.sqrt(16.0);
    log.info("Square root of 16 is: " + convert.toString(result));
}
