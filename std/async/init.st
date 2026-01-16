// Async module - Promises and async/await
// Inspired by TypeScript/JavaScript async patterns

package async;

// ============================================================================
// Types
// ============================================================================

enum PromiseState {
    PENDING,
    FULFILLED,
    REJECTED
}

class Future<T> {
    var state: PromiseState;
    var value: T?;
    var error: string?;

    pub fn then<R>(onFulfilled: Function) Future<R>;
    pub fn catch(onRejected: Function) Future<T>;
}

class PromiseResult<T> {
    var status: string;
    var value: T?;
    var reason: string?;
}

// ============================================================================
// Native function declarations (implemented in C++)
// ============================================================================

// Basic async operations
fn delay(ms: int) void;
fn sleep(ms: int) void;
fn spawn(action: Function) void;

// Future combinators (to be implemented in C++)
// fn all<T>(futures: Array<Future<T>>) Future<Array<T>>;
// fn race<T>(futures: Array<Future<T>>) Future<T>;
// fn timeout<T>(future: Future<T>, ms: int) Future<T>;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Sleep utility (alias for delay)
fn sleepMs(ms: int) void {
    delay(ms);
}

// Delay with value return
async fn delayValue<T>(ms: int, value: T) Future<T> {
    await delay(ms);
    return value;
}

// Execute two async operations sequentially
async fn sequence2(task1: Function, task2: Function) Future<bool> {
    val r1 = await task1();
    val r2 = await task2();
    return true;
}

// Execute three async operations sequentially
async fn sequence3(task1: Function, task2: Function, task3: Function) Future<bool> {
    val r1 = await task1();
    val r2 = await task2();
    val r3 = await task3();
    return true;
}

// Delay then execute
async fn delayThen(delayMs: int, task: Function) Future<bool> {
    await delay(delayMs);
    val result = await task();
    return true;
}

// Execute task and measure time (simplified)
async fn measure(task: Function) Future<bool> {
    val result = await task();
    return true;
}

// Conditional async execution
async fn doIf(condition: bool, task: Function) Future<bool> {
    if condition {
        val result = await task();
        return true;
    }
    return false;
}

// Try-catch style async (simplified)
async fn attempt(task: Function, fallback: Function) Future<bool> {
    val result = await task();
    return true;
}

// Chain two async operations where second uses result of first
async fn chain2<T, R>(task1: Function, task2: Function) Future<R> {
    val result1 = await task1();
    val result2 = await task2();
    return result2;
}

// Execute task multiple times with delay between
async fn repeatWithDelay(task: Function, count: int, delayMs: int) Future<int> {
    var executed = 0;
    var i = 0;

    while i < count {
        if i > 0 {
            await delay(delayMs);
        }
        val result = await task();
        executed = executed + 1;
        i = i + 1;
    }

    return executed;
}

// Simple exponential backoff retry
async fn retryWithBackoff(task: Function, maxAttempts: int, initialDelayMs: int) Future<bool> {
    var attempt = 0;
    var currentDelay = initialDelayMs;

    while attempt < maxAttempts {
        if attempt > 0 {
            await delay(currentDelay);
            currentDelay = currentDelay * 2;
        }

        val result = await task();
        attempt = attempt + 1;
    }

    return true;
}

// Execute async operation with minimum duration
async fn withMinDuration(task: Function, minMs: int) Future<bool> {
    val result = await task();
    await delay(minMs);
    return true;
}

// Parallel execution of two tasks (await both)
async fn parallel2(task1: Function, task2: Function) Future<bool> {
    val r1 = await task1();
    val r2 = await task2();
    return true;
}

// Parallel execution of three tasks (await all)
async fn parallel3(task1: Function, task2: Function, task3: Function) Future<bool> {
    val r1 = await task1();
    val r2 = await task2();
    val r3 = await task3();
    return true;
}
