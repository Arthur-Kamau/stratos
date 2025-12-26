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

class Promise<T> {
    var state: PromiseState;
    var value: T?;
    var error: Error?;
    var thenCallbacks: Array<Function<T, any>>;
    var catchCallbacks: Array<Function<Error, any>>;
    var finallyCallbacks: Array<Function>;

    fn then(onFulfilled: Function<T, R>) Promise<R>;
    fn catch(onRejected: Function<Error, T>) Promise<T>;
    fn finally(onFinally: Function) Promise<T>;
}

class Task<T> {
    var promise: Promise<T>;
    var cancel: Function;
    var running: bool;

    fn await() T;
    fn cancel() void;
    fn isRunning() bool;
}

// ============================================================================
// Native function declarations
// ============================================================================

// Promise creation
fn newPromise<T>(executor: Function<Function<T>, Function<Error>>) Promise<T>;
fn resolve<T>(value: T) Promise<T>;
fn reject<T>(error: Error) Promise<T>;

// Promise combinators
fn all<T>(promises: Array<Promise<T>>) Promise<Array<T>>;
fn race<T>(promises: Array<Promise<T>>) Promise<T>;
fn allSettled<T>(promises: Array<Promise<T>>) Promise<Array<PromiseResult<T>>>;
fn any<T>(promises: Array<Promise<T>>) Promise<T>;

class PromiseResult<T> {
    var status: string;  // "fulfilled" or "rejected"
    var value: T?;
    var reason: Error?;
}

// Async task creation
fn async<T>(fn: Function<T>) Task<T>;
fn await<T>(promise: Promise<T>) T;

// Delay and timeout
fn delay(ms: int) Promise<void>;
fn timeout<T>(promise: Promise<T>, ms: int) Promise<T>;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Create resolved promise
fn resolveWith<T>(value: T) Promise<T> {
    return resolve(value);
}

// Create rejected promise
fn rejectWith(error: Error) Promise<void> {
    return reject(error);
}

// Chain multiple promises
fn chain<T>(promises: Array<Function<Promise<T>>>) Promise<T> {
    if promises.length() == 0 {
        return resolve(null);
    }

    var current = promises[0]();

    for i in 1..promises.length() {
        val next = promises[i];
        current = current.then((value) => {
            return next();
        });
    }

    return current;
}

// Retry promise with delay
fn retry<T>(fn: Function<Promise<T>>, attempts: int, delayMs: int) Promise<T> {
    return newPromise((resolve, reject) => {
        var attempt = 0;

        fn attemptFn() {
            fn().then((value) => {
                resolve(value);
            }).catch((error) => {
                attempt = attempt + 1;
                if attempt >= attempts {
                    reject(error);
                } else {
                    delay(delayMs).then(() => {
                        attemptFn();
                    });
                }
            });
        }

        attemptFn();
    });
}

// Map over array with async function
fn map<T, R>(items: Array<T>, fn: Function<T, Promise<R>>) Promise<Array<R>> {
    val promises = collections.newList<Promise<R>>();

    for val item in items {
        promises.add(fn(item));
    }

    return all(promises.items);
}

// Filter array with async predicate
fn filter<T>(items: Array<T>, predicate: Function<T, Promise<bool>>) Promise<Array<T>> {
    val promises = collections.newList<Promise<bool>>();

    for val item in items {
        promises.add(predicate(item));
    }

    return all(promises.items).then((results) => {
        var filtered = Array<T>();
        for i in 0..results.length() {
            if results[i] {
                filtered.add(items[i]);
            }
        }
        return resolve(filtered);
    });
}

// Sequential forEach
fn forEach<T>(items: Array<T>, fn: Function<T, Promise<void>>) Promise<void> {
    var promise = resolve(null);

    for val item in items {
        promise = promise.then(() => {
            return fn(item);
        });
    }

    return promise;
}

// Parallel forEach
fn forEachParallel<T>(items: Array<T>, fn: Function<T, Promise<void>>) Promise<void> {
    val promises = collections.newList<Promise<void>>();

    for val item in items {
        promises.add(fn(item));
    }

    return all(promises.items).then(() => {
        return resolve(null);
    });
}

// Reduce array with async function
fn reduce<T, R>(items: Array<T>, initial: R, fn: Function<R, T, Promise<R>>) Promise<R> {
    var promise = resolve(initial);

    for val item in items {
        promise = promise.then((acc) => {
            return fn(acc, item);
        });
    }

    return promise;
}

// Wait for first successful promise
fn firstSuccess<T>(promises: Array<Promise<T>>) Promise<T> {
    return newPromise((resolve, reject) => {
        var rejected = 0;
        val errors = collections.newList<Error>();

        for val promise in promises {
            promise.then((value) => {
                resolve(value);
            }).catch((error) => {
                rejected = rejected + 1;
                errors.add(error);

                if rejected == promises.length() {
                    reject(Error("All promises failed"));
                }
            });
        }
    });
}

// Throttle async function calls
class AsyncThrottle<T, R> {
    var fn: Function<T, Promise<R>>;
    var maxConcurrent: int;
    var running: int;
    var queue: Array<Pair<T, Promise<R>>>;
    var mutex: concurrent.Mutex;

    constructor(asyncFn: Function<T, Promise<R>>, max: int) {
        this.fn = asyncFn;
        this.maxConcurrent = max;
        this.running = 0;
        this.queue = Array<Pair<T, Promise<R>>>();
        this.mutex = concurrent.newMutex();
    }

    fn execute(arg: T) Promise<R> {
        this.mutex.lock();

        if this.running < this.maxConcurrent {
            this.running = this.running + 1;
            this.mutex.unlock();

            return this.fn(arg).then((result) => {
                this.mutex.lock();
                this.running = this.running - 1;
                this.processQueue();
                this.mutex.unlock();
                return resolve(result);
            }).catch((error) => {
                this.mutex.lock();
                this.running = this.running - 1;
                this.processQueue();
                this.mutex.unlock();
                return reject(error);
            });
        } else {
            val promise = newPromise((resolve, reject) => {
                // Promise will be resolved when dequeued
            });
            this.queue.add(Pair(arg, promise));
            this.mutex.unlock();
            return promise;
        }
    }

    fn processQueue() void {
        if this.queue.length() > 0 && this.running < this.maxConcurrent {
            val item = this.queue.removeAt(0);
            this.running = this.running + 1;

            this.fn(item.first).then((result) => {
                item.second.resolve(result);
                this.mutex.lock();
                this.running = this.running - 1;
                this.processQueue();
                this.mutex.unlock();
            });
        }
    }
}

fn throttle<T, R>(fn: Function<T, Promise<R>>, maxConcurrent: int) AsyncThrottle<T, R> {
    return AsyncThrottle<T, R>(fn, maxConcurrent);
}

// Debounce async function
class AsyncDebounce<T, R> {
    var fn: Function<T, Promise<R>>;
    var delayMs: int;
    var timer: time.Timer?;
    var lastArgs: T?;
    var lastPromise: Promise<R>?;

    constructor(asyncFn: Function<T, Promise<R>>, delay: int) {
        this.fn = asyncFn;
        this.delayMs = delay;
        this.timer = null;
        this.lastArgs = null;
        this.lastPromise = null;
    }

    fn execute(arg: T) Promise<R> {
        this.lastArgs = arg;

        if this.timer != null {
            this.timer.stop();
        }

        return newPromise((resolve, reject) => {
            this.timer = time.newTimer(time.milliseconds(this.delayMs));

            time.after(time.milliseconds(this.delayMs)).then(() => {
                this.fn(this.lastArgs).then((result) => {
                    resolve(result);
                }).catch((error) => {
                    reject(error);
                });
            });
        });
    }
}

fn debounce<T, R>(fn: Function<T, Promise<R>>, delayMs: int) AsyncDebounce<T, R> {
    return AsyncDebounce<T, R>(fn, delayMs);
}

// Sleep utility
fn sleep(ms: int) Promise<void> {
    return delay(ms);
}

// Wait until condition is true
fn waitUntil(predicate: Function<bool>, checkInterval: int, maxWait: int) Promise<bool> {
    val startTime = time.now();

    return newPromise((resolve, reject) => {
        fn check() {
            if predicate() {
                resolve(true);
            } else if time.now().sub(startTime).milliseconds() >= maxWait {
                reject(Error("Wait timeout exceeded"));
            } else {
                delay(checkInterval).then(() => {
                    check();
                });
            }
        }

        check();
    });
}

// Wrap callback-based function to promise
fn promisify<T>(fn: Function<Function<T>, Function<Error>>) Promise<T> {
    return newPromise((resolve, reject) => {
        fn(
            (result) => { resolve(result); },
            (error) => { reject(error); }
        );
    });
}

// Convert promise to callback style
fn callbackify<T>(promise: Promise<T>, callback: Function<Error?, T?>) void {
    promise.then((value) => {
        callback(null, value);
    }).catch((error) => {
        callback(error, null);
    });
}
