// Example: Advanced Concurrent Programming
// Demonstrates goroutines, channels, worker pools, and pipelines

package main;

use concurrent;
use log;

fn advancedConcurrencyDemo() {
    log.info("\n=== Advanced Concurrency Demo ===");

    // Example 1: Goroutines with WaitGroup
    log.info("\n1. Goroutines Example:");

    val wg = concurrent.newWaitGroup();

    for val i in 0..3 {
        wg.add(1);
        val num = i;

        concurrent.go(() => {
            log.info("Goroutine " + num + " running");
            concurrent.sleep(50);
            log.info("Goroutine " + num + " done");
            wg.done();
        });
    }

    wg.wait();
    log.info("All goroutines completed!");

    // Example 2: Simple channel communication
    log.info("\n2. Channel Example:");

    val ch = concurrent.newChannel(5);
    val wg2 = concurrent.newWaitGroup();
    wg2.add(1);

    // Producer goroutine
    concurrent.go(() => {
        for val i in 1..4 {
            log.info("Sending: " + i);
            ch.send(i);
        }
        ch.close();
        log.info("Producer finished");
        wg2.done();
    });

    // Wait for producer to finish
    wg2.wait();

    // Now receive all values
    for val i in 0..3 {
        val value: int = ch.receive();
        log.info("Received: " + value);
    }

    // Example 3: Worker Pool
    log.info("\n3. Worker Pool Example:");

    val pool = concurrent.newWorkerPool(2, (job) => {
        log.info("Processing job: " + job);
        concurrent.sleep(50);
        return job * 2;
    });

    // Submit jobs
    for val i in 1..4 {
        pool.submit(i);
        log.info("Submitted job: " + i);
    }

    // Wait for jobs to complete then close
    concurrent.sleep(300);
    pool.close();

    // Collect results
    log.info("Collecting results:");
    for val i in 0..3 {
        val result: int = pool.results.receive();
        log.info("  Result: " + result);
    }

    // Example 4: Mutex with goroutines
    log.info("\n4. Mutex with Goroutines:");

    var counter = 0;
    val mutex = concurrent.newMutex();
    val wg3 = concurrent.newWaitGroup();

    for val i in 0..5 {
        wg3.add(1);

        concurrent.go(() => {
            for val j in 0..10 {
                mutex.lock();
                counter = counter + 1;
                mutex.unlock();
            }
            wg3.done();
        });
    }

    wg3.wait();
    log.info("Final counter: " + counter + " (expected: 50)");

    log.info("\nAdvanced concurrency demo complete!");
}
