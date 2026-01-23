// Example: Advanced Concurrent Programming
// Demonstrates worker pools

package main;

use concurrent;
use log;

fn advancedConcurrencyDemo() {
    log.info("\n=== Advanced Concurrency Demo ===");

    // Example 1: Simple goroutine without loop variable capture
    log.info("\n1. Simple Goroutine:");

    val wg = concurrent.newWaitGroup();
    wg.add(1);

    concurrent.go(() => {
        log.info("Goroutine running");
        concurrent.sleep(50);
        log.info("Goroutine done");
        wg.done();
    });

    wg.wait();
    log.info("Goroutine completed!");

    // Example 2: Worker Pool
    log.info("\n2. Worker Pool Example:");

    val pool = concurrent.newWorkerPool(2, (job) => {
        log.info("Processing job: " + job);
        concurrent.sleep(50);
        return job * 2;
    });

    // Submit jobs
    pool.submit(1);
    log.info("Submitted job: 1");
    pool.submit(2);
    log.info("Submitted job: 2");

    // Wait for jobs to complete then close
    concurrent.sleep(200);
    pool.close();

    // Collect results
    log.info("Collecting results:");
    val result1: int = pool.results.receive();
    log.info("  Result: " + result1);
    val result2: int = pool.results.receive();
    log.info("  Result: " + result2);

    log.info("\nAdvanced concurrency demo complete!");
}
