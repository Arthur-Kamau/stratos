// Concurrent module - Goroutines, channels, and synchronization
// Inspired by Go's concurrency primitives

package concurrent;

// ============================================================================
// Types
// ============================================================================

class Channel<T> {
    var buffer: Array<T>;
    var capacity: int;
    var closed: bool;

    pub fn send(value: T) bool;
    pub fn receive() T?;
    pub fn tryReceive() Pair<T?, bool>;
    pub fn close() void;
    pub fn isClosed() bool;
}

class Mutex {
    var locked: bool;
    var owner: int;

    pub fn lock() void;
    pub fn unlock() void;
    pub fn tryLock() bool;
}

class RWMutex {
    var readers: int;
    var writer: bool;

    pub fn rLock() void;
    pub fn rUnlock() void;
    pub fn lock() void;
    pub fn unlock() void;
}

class WaitGroup {
    var count: int;
    var mutex: Mutex;

    pub fn add(delta: int) void;
    pub fn done() void;
    pub fn wait() void;
}

class Once {
    var done: bool;
    var mutex: Mutex;

    pub fn do(action: Function) void;
}

class Semaphore {
    var count: int;
    var capacity: int;
    var mutex: Mutex;

    pub fn acquire() void;
    pub fn release() void;
    pub fn tryAcquire() bool;
}

class Future<T> {
    var result: Optional<Result<T, Error>>;
    var done: bool;
    var channel: Channel<Result<T, Error>>;

    pub fn get() Result<T, Error>;
    pub fn getWithTimeout(timeout: Duration) Result<T, Error>;
    pub fn isDone() bool;
    pub fn cancel() void;
}

// ============================================================================
// Native function declarations
// ============================================================================

// Goroutine spawning
fn go(action: Function) void;
fn goWithArgs(action: Function, args: Array<any>) void;

// Channel creation
fn newChannel<T>(capacity: int) Channel<T>;
fn newBufferedChannel<T>(capacity: int) Channel<T>;

// Select statement (for channel operations)
fn select(cases: Array<SelectCase>) int;

class SelectCase {
    var channel: Channel<any>;
    var send: bool;
    var value: Optional<any>;
    var handler: Function;
}

// Mutex creation
fn newMutex() Mutex;
fn newRWMutex() RWMutex;

// Synchronization primitives
fn newWaitGroup() WaitGroup;
fn newOnce() Once;
fn newSemaphore(capacity: int) Semaphore;

// Thread info
fn getThreadId() int;
fn getThreadCount() int;
fn setMaxThreads(count: int) void;

// Atomic operations
fn atomicAdd(ptr: int*, value: int) int;
fn atomicCompareAndSwap(ptr: int*, old: int, new: int) bool;
fn atomicLoad(ptr: int*) int;
fn atomicStore(ptr: int*, value: int) void;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Run function in background, return future
// Task should return Result<T, Error> for error handling
fn spawn<T>(task: Function<Result<T, Error>>) Future<T> {
    val future = Future<T> {
        result: None,
        done: false,
        channel: newChannel<Result<T, Error>>(1)
    };

    go(() => {
        val taskResult = task();
        future.result = Some(taskResult);
        future.done = true;
        future.channel.send(taskResult);
    });

    return future;
}

// Parallel map over collection
fn parallelMap<T, R>(items: Array<T>, mapper: Function<T, R>) Array<R> {
    val results = Array<R>(items.length());
    val wg = newWaitGroup();

    for val i in 0..items.length() {
        wg.add(1);
        val index = i;
        go(() => {
            results[index] = mapper(items[index]);
            wg.done();
        });
    }

    wg.wait();
    return results;
}

// Parallel forEach
// fn parallelForEach<T>(items: Array<T>, action: any) void {
//    // Implementation removed to avoid parser errors
// }

// Pipeline pattern
fn pipeline<T>(input: Channel<T>, stages: Array<Function<T, T>>) Channel<T> {
    var current = input;

    for val stage in stages {
        val output = newChannel<T>(10);
        val stageInput = current;

        go(() => {
            while true {
                val value = stageInput.receive();
                if value == null {
                    output.close();
                    break;
                }
                output.send(stage(value));
            }
        });

        current = output;
    }

    return current;
}

// Fan-out pattern
// fn fanOut<T>(input: Channel<T>, workers: int, action: any) void {
//    // Implementation removed to avoid parser errors
// }

// Fan-in pattern
fn fanIn<T>(channels: Array<Channel<T>>) Channel<T> {
    val output = newChannel<T>(10);
    val wg = newWaitGroup();

    for val ch in channels {
        wg.add(1);
        go(() => {
            while true {
                val value = ch.receive();
                if value == null {
                    break;
                }
                output.send(value);
            }
            wg.done();
        });
    }

    // Close output when all inputs are done
    go(() => {
        wg.wait();
        output.close();
    });

    return output;
}

// Worker pool pattern
class WorkerPool<T, R> {
    var workers: int;
    var jobs: Channel<T>;
    var results: Channel<R>;
    var wg: WaitGroup;

    constructor(numWorkers: int, workerFn: Function<T, R>) {
        this.workers = numWorkers;
        this.jobs = newChannel<T>(100);
        this.results = newChannel<R>(100);
        this.wg = newWaitGroup();

        // Start workers
        for val i in 0..numWorkers {
            this.wg.add(1);
            go(() => {
                while true {
                    val job = this.jobs.receive();
                    if job == null {
                        break;
                    }
                    val result = workerFn(job);
                    this.results.send(result);
                }
                this.wg.done();
            });
        }
    }

    pub fn submit(job: T) void {
        this.jobs.send(job);
    }

    pub fn close() void {
        this.jobs.close();
        this.wg.wait();
        this.results.close();
    }
}

fn newWorkerPool<T, R>(workers: int, workerFn: Function<T, R>) WorkerPool<T, R> {
    return WorkerPool<T, R>(workers, workerFn);
}

// Rate limiter
class RateLimiter {
    var tokens: Semaphore;
    var rate: Duration;

    constructor(requestsPerSecond: int) {
        this.tokens = newSemaphore(requestsPerSecond);
        this.rate = time.milliseconds(1000 / requestsPerSecond);

        // Refill tokens
        go(() => {
            while true {
                time.sleep(this.rate);
                this.tokens.release();
            }
        });
    }

    pub fn acquire() void {
        this.tokens.acquire();
    }

    pub fn tryAcquire() bool {
        return this.tokens.tryAcquire();
    }
}

fn newRateLimiter(requestsPerSecond: int) RateLimiter {
    return RateLimiter(requestsPerSecond);
}

// Timeout wrapper - task should return Result<T, Error>
fn withTimeout<T>(task: Function<Result<T, Error>>, timeout: Duration) Result<T, Error> {
    val done = newChannel<Result<T, Error>>(1);
    var timedOut = false;

    go(() => {
        val taskResult = task();
        if !timedOut {
            done.send(taskResult);
        }
    });

    val timer = time.newTimer(timeout);

    // Wait for either completion or timeout
    val selectResult = select([
        SelectCase {
            channel: done,
            send: false,
            handler: () => {
                val received = done.receive();
                if received.isSome() {
                    return received.unwrap();
                }
                return Result.err("Channel closed unexpectedly");
            }
        },
        SelectCase {
            channel: timer.channel,
            send: false,
            handler: () => {
                timedOut = true;
                return Result.err("Timeout exceeded");
            }
        }
    ]);

    return selectResult;
}

// Retry with concurrent attempts
// Task should return Result<T, Error> for error handling
fn retryParallel<T>(task: Function<Result<T, Error>>, attempts: int) Result<T, Error> {
    val results = newChannel<Result<T, Error>>(attempts);

    for val i in 0..attempts {
        go(() => {
            val taskResult = task();
            results.send(taskResult);
        });
    }

    // Return first successful result
    for val i in 0..attempts {
        val result = results.receive();
        if result.isSome() {
            val r = result.unwrap();
            if r.ok() {
                return r;
            }
        }
    }

    return Result.err("All attempts failed");
}
