// Example: Async/Await and Promises
// Demonstrates the async module (TypeScript-style)

package main;

use async;
use log;
use time;
use http;

fn main() {
    log.info("=== Async/Await Example ===");

    // Example 1: Basic Promise
    log.info("\n1. Basic Promise:");

    val promise = async.newPromise((resolve, reject) => {
        log.info("Promise executing...");
        time.sleep(time.milliseconds(1000));

        if math.random() > 0.5 {
            resolve("Success!");
        } else {
            reject(io.Error("Random failure"));
        }
    });

    promise.then((value) => {
        log.info("Promise resolved: " + value);
    }).catch((error) => {
        log.error("Promise rejected: " + error.message);
    }).finally(() => {
        log.info("Promise completed");
    });

    // Example 2: Promise chaining
    log.info("\n2. Promise Chaining:");

    async.resolve(5)
        .then((n) => {
            log.info("Step 1: " + n);
            return async.resolve(n * 2);
        })
        .then((n) => {
            log.info("Step 2: " + n);
            return async.resolve(n + 10);
        })
        .then((n) => {
            log.info("Step 3: " + n);
            log.info("Final result: " + n);
        });

    // Example 3: Promise.all
    log.info("\n3. Promise.all:");

    val promises = [
        async.delay(100).then(() => async.resolve("Task 1 done")),
        async.delay(200).then(() => async.resolve("Task 2 done")),
        async.delay(150).then(() => async.resolve("Task 3 done"))
    ];

    async.all(promises).then((results) => {
        log.info("All tasks completed:");
        for val result in results {
            log.info("  - " + result);
        }
    });

    // Example 4: Promise.race
    log.info("\n4. Promise.race:");

    val racers = [
        async.delay(300).then(() => async.resolve("Slow task")),
        async.delay(100).then(() => async.resolve("Fast task")),
        async.delay(200).then(() => async.resolve("Medium task"))
    ];

    async.race(racers).then((winner) => {
        log.info("Winner: " + winner);
    });

    // Example 5: Async map
    log.info("\n5. Async Map:");

    val numbers = [1, 2, 3, 4, 5];

    async.map(numbers, (n) => {
        return async.delay(n * 100).then(() => {
            log.info("Processing: " + n);
            return async.resolve(n * n);
        });
    }).then((squares) => {
        log.info("Squares: " + squares);
    });

    // Example 6: Async retry
    log.info("\n6. Async Retry:");

    var attempts = 0;

    val unreliableTask = () => {
        attempts = attempts + 1;
        log.info("Attempt " + attempts);

        return async.newPromise((resolve, reject) => {
            if attempts >= 3 {
                resolve("Success on attempt " + attempts);
            } else {
                reject(io.Error("Failed attempt " + attempts));
            }
        });
    };

    async.retry(unreliableTask, 5, 500).then((result) => {
        log.info("Retry succeeded: " + result);
    }).catch((error) => {
        log.error("Retry failed: " + error.message);
    });

    // Example 7: Async HTTP requests
    log.info("\n7. Async HTTP Requests:");

    val fetchUser = (id: int) => {
        log.info("Fetching user " + id + "...");

        return async.delay(500).then(() => {
            return async.resolve(json.jsonObject({
                id: json.jsonNumber(id),
                name: json.jsonString("User " + id),
                email: json.jsonString("user" + id + "@example.com")
            }));
        });
    };

    // Fetch multiple users in parallel
    val userPromises = [
        fetchUser(1),
        fetchUser(2),
        fetchUser(3)
    ];

    async.all(userPromises).then((users) => {
        log.info("Fetched " + users.length() + " users:");
        for val user in users {
            log.info("  - " + json.getStringPath(user, "name") + " (" + json.getStringPath(user, "email") + ")");
        }
    });

    // Example 8: Throttle
    log.info("\n8. Async Throttle:");

    val expensiveTask = (n: int) => {
        log.info("Running expensive task " + n);
        return async.delay(500).then(() => {
            return async.resolve(n * 2);
        });
    };

    val throttled = async.throttle(expensiveTask, 2); // Max 2 concurrent

    for i in 1..6 {
        throttled.execute(i).then((result) => {
            log.info("Throttled result: " + result);
        });
    }

    // Example 9: Wait until
    log.info("\n9. Wait Until:");

    var ready = false;

    // Set ready after 2 seconds
    concurrent.go(() => {
        time.sleep(time.seconds(2));
        ready = true;
        log.info("System is now ready!");
    });

    async.waitUntil(() => ready, 100, 5000).then(() => {
        log.info("Condition met, proceeding...");
    }).catch((error) => {
        log.error("Timeout waiting for condition");
    });

    // Example 10: Sequential forEach
    log.info("\n10. Sequential forEach:");

    val items = ["item1", "item2", "item3"];

    async.forEach(items, (item) => {
        log.info("Processing " + item);
        return async.delay(300).then(() => {
            log.info("Completed " + item);
        });
    }).then(() => {
        log.info("All items processed sequentially");
    });

    log.info("\nAsync examples started! Watch the logs for results.");
}
