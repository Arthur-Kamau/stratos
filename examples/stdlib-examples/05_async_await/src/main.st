// Example: Async/Await
// Demonstrates async functions and await keyword

package main;

use async;

// Simple async function that delays and returns a value
async fn delayedMessage(message: string, delayMs: int) Future<string> {
    println("Starting: " + message);
    await async.delay(delayMs);
    println("Completed: " + message);
    return message;
}

// Async function that processes data
async fn processData(data: string) Future<string> {
    println("Processing: " + data);
    await async.delay(100);
    return "Processed-" + data;
}

// Async function that fetches data (simulated)
async fn fetchData(id: int) Future<string> {
    println("Fetching data for ID: " + id);
    await async.delay(200);
    return "Data-" + id;
}

// Chain multiple async operations
async fn fetchAndProcess(id: int) Future<string> {
    val data = await fetchData(id);
    val result = await processData(data);
    return result;
}

// Example: Sequential async operations
async fn sequentialExample() Future<void> {
    println("\n=== Sequential Operations ===");

    val msg1 = await delayedMessage("First task", 300);
    println("Got: " + msg1);

    val msg2 = await delayedMessage("Second task", 200);
    println("Got: " + msg2);

    val msg3 = await delayedMessage("Third task", 100);
    println("Got: " + msg3);

    println("All sequential tasks completed!");
}

// Example: Using delayValue utility
async fn delayValueExample() Future<void> {
    println("\n=== DelayValue Example ===");

    val result = await async.delayValue(500, 42);
    println("Got delayed value: " + result);
}

// Example: Retry with backoff
async fn retryExample() Future<void> {
    println("\n=== Retry with Backoff ===");

    val task = async () => {
        println("Executing task...");
        await async.delay(100);
    };

    val success = await async.retryWithBackoff(task, 3, 100);
    if success {
        println("Retry completed successfully");
    }
}

// Example: Repeat operation multiple times
async fn repeatExample() Future<void> {
    println("\n=== Repeat with Delay ===");

    var counter = 0;

    val task = async () => {
        counter = counter + 1;
        println("Execution #" + counter);
        await async.delay(50);
    };

    val count = await async.repeatWithDelay(task, 3, 150);
    println("Completed " + count + " executions");
}

// Example: Conditional execution
async fn conditionalExample() Future<void> {
    println("\n=== Conditional Async ===");

    val shouldRun = true;

    val task = async () => {
        println("Task is running!");
        await async.delay(100);
    };

    val didRun = await async.doIf(shouldRun, task);
    if didRun {
        println("Task was executed");
    }
}

// Example: Sequential task execution
async fn sequenceExample() Future<void> {
    println("\n=== Sequence Tasks ===");

    val task1 = async () => {
        println("Task 1 executing");
        await async.delay(100);
    };

    val task2 = async () => {
        println("Task 2 executing");
        await async.delay(100);
    };

    val task3 = async () => {
        println("Task 3 executing");
        await async.delay(100);
    };

    await async.sequence2(task1, task2);
    println("First two tasks completed");

    await async.sequence3(task1, task2, task3);
    println("All three tasks completed");
}

// Example: Chaining async operations
async fn chainExample() Future<void> {
    println("\n=== Chain Operations ===");

    val result = await fetchAndProcess(123);
    println("Final result: " + result);
}

// Main async function
async fn main() {
    println("=== Async/Await Examples ===");

    // Example 1: Basic async/await
    println("\n--- Example 1: Basic Async ---");
    val data = await fetchData(1);
    println("Received: " + data);

    // Example 2: Chaining
    println("\n--- Example 2: Chaining ---");
    await chainExample();

    // Example 3: Sequential operations
    println("\n--- Example 3: Sequential ---");
    await sequentialExample();

    // Example 4: DelayValue
    println("\n--- Example 4: DelayValue ---");
    await delayValueExample();

    // Example 5: Retry
    println("\n--- Example 5: Retry ---");
    await retryExample();

    // Example 6: Repeat
    println("\n--- Example 6: Repeat ---");
    await repeatExample();

    // Example 7: Conditional
    println("\n--- Example 7: Conditional ---");
    await conditionalExample();

    // Example 8: Sequence
    println("\n--- Example 8: Sequence ---");
    await sequenceExample();

    println("\n=== All Examples Completed ===");
}
