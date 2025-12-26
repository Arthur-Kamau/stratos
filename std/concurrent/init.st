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

    fn send(value: T) bool;
    fn receive() T?;
    fn tryReceive() Pair<T?, bool>;
    fn close() void;
    fn isClosed() bool;
}

class Mutex {
    var locked: bool;
    var owner: int;

    fn lock() void;
    fn unlock() void;
    fn tryLock() bool;
}

class RWMutex {
    var readers: int;
    var writer: bool;

    fn rLock() void;
    fn rUnlock() void;
    fn lock() void;
    fn unlock() void;
}

class WaitGroup {
    var count: int;
    var mutex: Mutex;

    fn add(delta: int) void;
    fn done() void;
    fn wait() void;
}

class Once {
    var done: bool;
    var mutex: Mutex;

    fn do(fn: Function) void;
}

class Semaphore {
    var count: int;
    var capacity: int;
    var mutex: Mutex;

    fn acquire() void;
    fn release() void;
    fn tryAcquire() bool;
}

class Future<T> {
    var value: T?;
    var error: Error?;
    var done: bool;
    var channel: Channel<T>;

    fn get() T;
    fn getWithTimeout(timeout: Duration) Result<T, Error>;
    fn isDone() bool;
    fn cancel() void;
}

// ============================================================================
// Native function declarations
// ============================================================================

// Goroutine spawning
fn go(fn: Function) void;
fn goWithArgs(fn: Function, args: Array<any>) void;

// Channel creation
fn newChannel<T>(capacity: int) Channel<T>;
fn newBufferedChannel<T>(capacity: int) Channel<T>;

// Select statement (for channel operations)
fn select(cases: Array<SelectCase>) int;

class SelectCase {
    var channel: Channel<any>;
    var send: bool;
    var value: any?;
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
fn spawn<T>(fn: Function<T>) Future<T> {
    val future = Future<T> {
        value: null,
        error: null,
        done: false,
        channel: newChannel<T>(1)
    };

    go(() => {
        try {
            val result = fn();
            future.value = result;
            future.done = true;
            future.channel.send(result);
        } catch (e: Error) {
            future.error = e;
            future.done = true;
        }
    });

    return future;
}

// Parallel map over collection
fn parallelMap<T, R>(items: Array<T>, fn: Function<T, R>) Array<R> {
    val results = Array<R>(items.length());
    val wg = newWaitGroup();

    for i in 0..items.length() {
        wg.add(1);
        val index = i;
        go(() => {
            results[index] = fn(items[index]);
            wg.done();
        });
    }

    wg.wait();
    return results;
}

// Parallel forEach
fn parallelForEach<T>(items: Array<T>, fn: Function<T, void>) void {
    val wg = newWaitGroup();

    for val item in items {
        wg.add(1);
        go(() => {
            fn(item);
            wg.done();
        });
    }

    wg.wait();
}

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
fn fanOut<T>(input: Channel<T>, workers: int, fn: Function<T, void>) void {
    val wg = newWaitGroup();

    for i in 0..workers {
        wg.add(1);
        go(() => {
            while true {
                val value = input.receive();
                if value == null {
                    break;
                }
                fn(value);
            }
            wg.done();
        });
    }

    wg.wait();
}

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

    constructor(numWorkers: int, fn: Function<T, R>) {
        this.workers = numWorkers;
        this.jobs = newChannel<T>(100);
        this.results = newChannel<R>(100);
        this.wg = newWaitGroup();

        // Start workers
        for i in 0..numWorkers {
            this.wg.add(1);
            go(() => {
                while true {
                    val job = this.jobs.receive();
                    if job == null {
                        break;
                    }
                    val result = fn(job);
                    this.results.send(result);
                }
                this.wg.done();
            });
        }
    }

    fn submit(job: T) void {
        this.jobs.send(job);
    }

    fn close() void {
        this.jobs.close();
        this.wg.wait();
        this.results.close();
    }
}

fn newWorkerPool<T, R>(workers: int, fn: Function<T, R>) WorkerPool<T, R> {
    return WorkerPool<T, R>(workers, fn);
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

    fn acquire() void {
        this.tokens.acquire();
    }

    fn tryAcquire() bool {
        return this.tokens.tryAcquire();
    }
}

fn newRateLimiter(requestsPerSecond: int) RateLimiter {
    return RateLimiter(requestsPerSecond);
}

// Timeout wrapper
fn withTimeout<T>(fn: Function<T>, timeout: Duration) Result<T, Error> {
    val done = newChannel<T>(1);
    val timedOut = false;

    go(() => {
        val result = fn();
        if !timedOut {
            done.send(result);
        }
    });

    val timer = time.newTimer(timeout);

    // Wait for either completion or timeout
    val result = select([
        SelectCase {
            channel: done,
            send: false,
            value: null,
            handler: () => {
                return Result<T, Error> {
                    value: done.receive(),
                    error: null,
                    isOk: true
                };
            }
        },
        SelectCase {
            channel: timer.channel,
            send: false,
            value: null,
            handler: () => {
                timedOut = true;
                return Result<T, Error> {
                    value: null,
                    error: Error("Timeout exceeded"),
                    isOk: false
                };
            }
        }
    ]);

    return result;
}

// Retry with concurrent attempts
fn retryParallel<T>(fn: Function<T>, attempts: int) Result<T, Error> {
    val results = newChannel<Result<T, Error>>(attempts);

    for i in 0..attempts {
        go(() => {
            try {
                val result = fn();
                results.send(Result<T, Error> {
                    value: result,
                    error: null,
                    isOk: true
                });
            } catch (e: Error) {
                results.send(Result<T, Error> {
                    value: null,
                    error: e,
                    isOk: false
                });
            }
        });
    }

    // Return first successful result
    for i in 0..attempts {
        val result = results.receive();
        if result.ok() {
            return result;
        }
    }

    return Result<T, Error> {
        value: null,
        error: Error("All attempts failed"),
        isOk: false
    };
}
