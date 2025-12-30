// Example: Concurrent Programming
// Demonstrates goroutines, channels, and synchronization

package main;

use concurrent;
use log;
use time;

fn main() {
    log.info("=== Concurrency Example ===");

    // Example 1: Simple goroutines
    log.info("\n1. Simple Goroutines:");

    val wg = concurrent.newWaitGroup();

    for i in 0..5 {
        wg.add(1);
        val num = i;

        concurrent.go(() => {
            log.info("Goroutine " + num + " running");
            time.sleep(time.milliseconds(100));
            log.info("Goroutine " + num + " done");
            wg.done();
        });
    }

    wg.wait();
    log.info("All goroutines completed");

    // Example 2: Channels
    log.info("\n2. Channel Communication:");

    val ch = concurrent.newChannel<int>(5);

    // Producer
    concurrent.go(() => {
        for i in 0..5 {
            log.info("Sending: " + i);
            ch.send(i);
            time.sleep(time.milliseconds(100));
        }
        ch.close();
    });

    // Consumer
    while true {
        val value = ch.receive();
        if value == null {
            break;
        }
        log.info("Received: " + value);
    }

    // Example 3: Worker Pool
    log.info("\n3. Worker Pool:");

    val pool = concurrent.newWorkerPool<int, int>(3, (num) => {
        log.info("Processing job: " + num);
        time.sleep(time.milliseconds(200));
        return num * 2;
    });

    // Submit jobs
    concurrent.go(() => {
        for i in 1..11 {
            pool.submit(i);
        }
        pool.close();
    });

    // Collect results
    while true {
        val result = pool.results.receive();
        if result == null {
            break;
        }
        log.info("Result: " + result);
    }

    // Example 4: Mutex for shared state
    log.info("\n4. Mutex Example:");

    var counter = 0;
    val mutex = concurrent.newMutex();
    val wg2 = concurrent.newWaitGroup();

    for i in 0..10 {
        wg2.add(1);

        concurrent.go(() => {
            for j in 0..100 {
                mutex.lock();
                counter = counter + 1;
                mutex.unlock();
            }
            wg2.done();
        });
    }

    wg2.wait();
    log.info("Final counter value: " + counter);
    log.info("Expected: 1000");

    // Example 5: Pipeline
    log.info("\n5. Pipeline Pattern:");

    val input = concurrent.newChannel<int>(10);

    // Stage 1: Double the number
    val stage1 = (n) => {
        log.info("Stage 1: " + n + " -> " + (n * 2));
        return n * 2;
    };

    // Stage 2: Add 10
    val stage2 = (n) => {
        log.info("Stage 2: " + n + " -> " + (n + 10));
        return n + 10;
    };

    val output = concurrent.pipeline(input, [stage1, stage2]);

    // Feed input
    concurrent.go(() => {
        for i in 1..6 {
            input.send(i);
            time.sleep(time.milliseconds(50));
        }
        input.close();
    });

    // Collect output
    while true {
        val result = output.receive();
        if result == null {
            break;
        }
        log.info("Pipeline result: " + result);
    }

    log.info("\nConcurrency examples complete!");
}
