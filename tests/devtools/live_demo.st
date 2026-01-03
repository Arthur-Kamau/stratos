package main;

use log;

fn main() {
    log.info("DevTools Live Demo - Starting");
    log.info("This program will run for 30 seconds");
    log.info("Open http://localhost:8080 in your browser to see logs");
    log.info("========================================");

    val count = 0;
    while (count < 30) {
        val count = count + 1;

        if (count % 3 == 0) {
            log.debug("Debug message at count");
        }

        if (count % 5 == 0) {
            log.warn("Warning at count");
        }

        if (count % 10 == 0) {
            log.error("Error at count");
        } else {
            log.info("Info message at count");
        }

        // Sleep for ~1 second (busy wait)
        val sleep_count = 0;
        while (sleep_count < 100000000) {
            val sleep_count = sleep_count + 1;
        }
    }

    log.info("========================================");
    log.info("Demo complete - program will exit in 5 seconds");
    log.info("This gives you time to see the final logs");

    // Final sleep
    val final_sleep = 0;
    while (final_sleep < 500000000) {
        val final_sleep = final_sleep + 1;
    }
}
