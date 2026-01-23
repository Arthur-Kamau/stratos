// Example: Advanced Concurrency Primitives
// Demonstrates channels and synchronization (synchronous operations)

package main;

use concurrent;
use log;

fn advancedConcurrencyDemo() {
    log.info("\n=== Advanced Concurrency Demo ===");

    // Example 1: Channel send/receive
    log.info("\n1. Channel Send/Receive:");

    val ch = concurrent.newChannel(5);

    // Send multiple values
    ch.send(10);
    ch.send(20);
    ch.send(30);
    log.info("Sent 3 values to channel");

    // Receive values
    val v1: int = ch.receive();
    val v2: int = ch.receive();
    val v3: int = ch.receive();
    log.info("Received: " + v1 + ", " + v2 + ", " + v3);

    // Example 2: Mutex protecting shared state
    log.info("\n2. Mutex Protected Counter:");

    val mutex = concurrent.newMutex();
    var counter = 0;

    // Simulate protected increment
    for val i in 0..10 {
        mutex.lock();
        counter = counter + 1;
        mutex.unlock();
    }
    log.info("Counter after 10 increments: " + counter);

    // Example 3: WaitGroup tracking
    log.info("\n3. WaitGroup Task Tracking:");

    val wg = concurrent.newWaitGroup();

    // Add tasks
    wg.add(3);
    log.info("Added 3 tasks");

    // Complete tasks
    log.info("Completing task 1...");
    wg.done();

    log.info("Completing task 2...");
    wg.done();

    log.info("Completing task 3...");
    wg.done();

    // Wait should return immediately since all tasks done
    wg.wait();
    log.info("All tasks completed!");

    // Example 4: Channel with close
    log.info("\n4. Channel Close:");

    val ch2 = concurrent.newChannel(3);
    ch2.send(100);
    ch2.send(200);

    log.info("Channel open, isClosed: " + ch2.isClosed());
    ch2.close();
    log.info("Channel closed, isClosed: " + ch2.isClosed());

    log.info("\nAdvanced demo complete!");
}
