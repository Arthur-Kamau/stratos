package main;

use log;

fn countdown(n: int) {
    val i = n;
    while i > 0 {
        log.info("Countdown");
        log.debug("Debug message");

        if i == 5 {
            log.warn("Halfway there!");
        }

        val i = i - 1;
    }
}

fn main() {
    log.info("DevTools Demo Starting");
    log.debug("Initializing application");

    countdown(10);

    log.info("Processing complete");
    log.warn("This is a warning");
    log.error("This is an error (not critical)");

    log.info("DevTools Demo Completed");
}
