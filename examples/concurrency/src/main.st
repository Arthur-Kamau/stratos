// Example: Basic Concurrency Primitives
// Demonstrates WaitGroup and Mutex usage

package main;

use concurrent;
use log;
use time;

fn main() {
    log.info("=== Concurrency Example ===");

    // Example 1: WaitGroup
    log.info("\n1. WaitGroup Example:");

    val wg = concurrent.newWaitGroup();
    log.info("Created WaitGroup");

    wg.add(3);
    log.info("Added 3 to WaitGroup");

    wg.done();
    log.info("Called done (1/3)");

    wg.done();
    log.info("Called done (2/3)");

    wg.done();
    log.info("Called done (3/3)");

    wg.wait();
    log.info("WaitGroup completed!");

    // Example 2: Mutex
    log.info("\n2. Mutex Example:");

    val mutex = concurrent.newMutex();
    log.info("Created Mutex");

    mutex.lock();
    log.info("Mutex locked");

    val tryResult = mutex.tryLock();
    log.info("tryLock returned: " + tryResult);

    mutex.unlock();
    log.info("Mutex unlocked");

    val tryResult2 = mutex.tryLock();
    log.info("tryLock after unlock: " + tryResult2);

    mutex.unlock();

    // Example 3: Channel
    log.info("\n3. Channel Example:");

    val ch = concurrent.newChannel(5);
    log.info("Created Channel with capacity 5");

    ch.send(42);
    log.info("Sent value to channel");

    ch.close();
    log.info("Channel closed");

    log.info("isClosed: " + ch.isClosed());

    // Example 4: Thread ID
    log.info("\n4. Thread Info:");
    val threadId = concurrent.getThreadId();
    log.info("Current thread ID: " + threadId);

    // Example 5: Sleep
    log.info("\n5. Sleep Example:");
    log.info("Sleeping for 100ms...");
    concurrent.sleep(100);
    log.info("Woke up!");

    log.info("\nBasic concurrency examples complete!");

    // Run advanced demo
    advancedConcurrencyDemo();
}
