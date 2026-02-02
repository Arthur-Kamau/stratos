---
title: Concurrency
description: Goroutines, channels, mutexes, and concurrent programming in Stratos
---

# Concurrency

Stratos provides powerful concurrency primitives inspired by Go, including lightweight goroutines, typed channels, and synchronization primitives.

## Goroutines

Goroutines are lightweight threads managed by the Stratos runtime. They're ideal for concurrent tasks that don't need the overhead of OS threads.

### Launching Goroutines

```stratos
package main;

use concurrent;

fn main() {
    // Launch a goroutine with concurrent.go()
    concurrent.go(|| {
        print("Hello from goroutine!");
    });

    // Main continues immediately
    print("Main function");

    // Wait for goroutine to complete
    concurrent.sleep(100);
}
```

### Goroutines with Parameters

```stratos
package main;

use concurrent;

fn worker(id: int) {
    print("Worker " + id + " starting");
    concurrent.sleep(100);
    print("Worker " + id + " done");
}

fn main() {
    // Launch multiple workers
    for val i in 1..6 {
        concurrent.go(|| {
            worker(i);
        });
    }

    // Wait for all workers
    concurrent.sleep(500);
    print("All workers completed");
}
```

::: tip Goroutine Characteristics
- Extremely lightweight (small stack, grows as needed)
- Multiplexed onto OS threads by the runtime
- Communication via channels (recommended) or shared memory with synchronization
:::

## Channels

Channels are typed conduits for communication between goroutines. They ensure safe data transfer without explicit locking.

### Creating Channels

```stratos
package main;

use concurrent;

fn main() {
    // Unbuffered channel (blocks until received)
    val unbuffered = concurrent.newChannel<int>();

    // Buffered channel (capacity of 5)
    val buffered = concurrent.newChannel<int>(5);
}
```

### Sending and Receiving

```stratos
package main;

use concurrent;

fn main() {
    val ch = concurrent.newChannel<int>(1);

    // Send a value
    ch.send(42);

    // Receive a value
    val value = ch.receive();
    print("Received: " + value);
}
```

### Channel Communication Pattern

```stratos
package main;

use concurrent;

fn producer(ch: Channel<int>) {
    for val i in 1..6 {
        print("Sending: " + i);
        ch.send(i);
    }
    ch.close();
}

fn consumer(ch: Channel<int>) {
    while true {
        val value = ch.receive();
        if (value == null) {
            break;  // Channel closed
        }
        print("Received: " + value);
    }
}

fn main() {
    val ch = concurrent.newChannel<int>(2);

    concurrent.go(|| producer(ch));
    concurrent.go(|| consumer(ch));

    concurrent.sleep(500);
}
```

### Channel Operators

Stratos supports Go-style channel operators:

```stratos
package main;

use concurrent;

fn main() {
    val ch = concurrent.newChannel<string>(1);

    // Send with arrow operator
    ch <- "hello";

    // Receive with arrow operator
    val msg = <-ch;
    print(msg);
}
```

## WaitGroup

WaitGroup waits for a collection of goroutines to finish.

```stratos
package main;

use concurrent;

fn worker(id: int, wg: WaitGroup) {
    print("Worker " + id + " starting");
    concurrent.sleep(100);
    print("Worker " + id + " done");
    wg.done();
}

fn main() {
    val wg = concurrent.newWaitGroup();

    for val i in 1..6 {
        wg.add(1);
        concurrent.go(|| {
            worker(i, wg);
        });
    }

    wg.wait();  // Block until all workers done
    print("All workers completed");
}
```

### WaitGroup Pattern with Defer

```stratos
package main;

use concurrent;

fn worker(id: int, wg: WaitGroup) {
    defer wg.done();  // Ensure done() is called even on error

    print("Worker " + id + " processing");
    // Do work...
}

fn main() {
    val wg = concurrent.newWaitGroup();

    for val i in 1..4 {
        wg.add(1);
        concurrent.go(|| worker(i, wg));
    }

    wg.wait();
    print("Complete");
}
```

## Mutex

Mutex provides mutual exclusion for protecting shared state.

### Basic Mutex Usage

```stratos
package main;

use concurrent;

var counter = 0;

fn increment(mutex: Mutex, times: int) {
    for val _ in 0..times {
        mutex.lock();
        counter = counter + 1;
        mutex.unlock();
    }
}

fn main() {
    val mutex = concurrent.newMutex();

    // Launch concurrent incrementers
    concurrent.go(|| increment(mutex, 1000));
    concurrent.go(|| increment(mutex, 1000));
    concurrent.go(|| increment(mutex, 1000));

    concurrent.sleep(500);
    print("Counter: " + counter);  // Should be 3000
}
```

### Mutex with Defer

```stratos
package main;

use concurrent;

fn safeUpdate(mutex: Mutex, data: SharedData) {
    mutex.lock();
    defer mutex.unlock();  // Always releases lock

    // Safe to modify data here
    data.value = data.value + 1;
    data.lastUpdated = time.now();
}
```

::: warning Mutex Best Practices
- Always unlock in a defer statement to prevent deadlocks
- Keep critical sections as short as possible
- Prefer channels for communication when possible
:::

## Worker Pool Pattern

A common pattern for processing work concurrently:

```stratos
package main;

use concurrent;

fn worker(id: int, jobs: Channel<int>, results: Channel<int>) {
    while true {
        val job = jobs.receive();
        if (job == null) {
            break;  // No more jobs
        }

        print("Worker " + id + " processing job " + job);
        concurrent.sleep(50);

        val result = job * 2;
        results.send(result);
    }
}

fn main() {
    val numJobs = 10;
    val numWorkers = 3;

    val jobs = concurrent.newChannel<int>(numJobs);
    val results = concurrent.newChannel<int>(numJobs);

    // Start workers
    for val w in 1..numWorkers + 1 {
        concurrent.go(|| worker(w, jobs, results));
    }

    // Send jobs
    for val j in 1..numJobs + 1 {
        jobs.send(j);
    }
    jobs.close();

    // Collect results
    for val _ in 1..numJobs + 1 {
        val result = results.receive();
        print("Result: " + result);
    }
}
```

## Pipeline Pattern

Chain stages of processing using channels:

```stratos
package main;

use concurrent;

fn generate(out: Channel<int>) {
    for val i in 1..11 {
        out.send(i);
    }
    out.close();
}

fn square(input: Channel<int>, output: Channel<int>) {
    while true {
        val n = input.receive();
        if (n == null) break;
        output.send(n * n);
    }
    output.close();
}

fn print_values(input: Channel<int>) {
    while true {
        val n = input.receive();
        if (n == null) break;
        print("Value: " + n);
    }
}

fn main() {
    val naturals = concurrent.newChannel<int>(10);
    val squares = concurrent.newChannel<int>(10);

    concurrent.go(|| generate(naturals));
    concurrent.go(|| square(naturals, squares));

    print_values(squares);
}
```

## Select Statement

Select lets you wait on multiple channel operations:

```stratos
package main;

use concurrent;

fn main() {
    val ch1 = concurrent.newChannel<int>(1);
    val ch2 = concurrent.newChannel<string>(1);

    concurrent.go(|| {
        concurrent.sleep(100);
        ch1.send(42);
    });

    concurrent.go(|| {
        concurrent.sleep(50);
        ch2.send("hello");
    });

    // Wait for first available
    select {
        case receive ch1:
            val v = ch1.receive();
            print("Received int: " + v)
        case receive ch2:
            val s = ch2.receive();
            print("Received string: " + s)
        default:
            print("No channel ready")
    }
}
```

### Select with Timeout

```stratos
package main;

use concurrent;
use time;

fn main() {
    val ch = concurrent.newChannel<int>(1);
    val timeout = time.after(1000);  // 1 second timeout

    select {
        case receive ch:
            val v = ch.receive();
            print("Received: " + v)
        case receive timeout:
            print("Timed out!")
    }
}
```

## Thread Utilities

```stratos
package main;

use concurrent;

fn main() {
    // Get current thread ID
    val threadId = concurrent.getThreadId();
    print("Thread ID: " + threadId);

    // Sleep for milliseconds
    concurrent.sleep(100);

    // Yield to other threads
    concurrent.yield();
}
```

## Concurrent Data Structures

### Thread-Safe Counter

```stratos
package main;

use concurrent;

class SafeCounter {
    private var count: int;
    private var mutex: Mutex;

    constructor() {
        this.count = 0;
        this.mutex = concurrent.newMutex();
    }

    pub fn increment() {
        this.mutex.lock();
        defer this.mutex.unlock();
        this.count = this.count + 1;
    }

    pub fn get() int {
        this.mutex.lock();
        defer this.mutex.unlock();
        return this.count;
    }
}
```

### Thread-Safe Map

```stratos
package main;

use concurrent;
use maps;

class SafeMap {
    private var data: Map<string, int>;
    private var mutex: Mutex;

    constructor() {
        this.data = maps.create();
        this.mutex = concurrent.newMutex();
    }

    pub fn put(key: string, value: int) {
        this.mutex.lock();
        defer this.mutex.unlock();
        this.data = this.data.put(key, value);
    }

    pub fn get(key: string) int {
        this.mutex.lock();
        defer this.mutex.unlock();
        return this.data.get(key);
    }
}
```

## Best Practices

::: tip Prefer Channels Over Shared Memory
"Don't communicate by sharing memory; share memory by communicating."
Channels make data flow explicit and prevent race conditions.
:::

::: tip Keep Critical Sections Small
When using mutexes, minimize the code between lock and unlock.
This reduces contention and improves performance.
:::

::: tip Always Release Locks
Use `defer mutex.unlock()` immediately after locking to ensure locks are always released, even on errors or early returns.
:::

::: warning Avoid Deadlocks
- Always acquire locks in a consistent order
- Use timeouts when possible
- Consider using channels instead of multiple locks
:::

::: tip Size Buffered Channels Appropriately
- Unbuffered: Synchronous, blocking communication
- Buffered: Decouples sender and receiver timing
- Too large: Memory waste
- Too small: Potential bottlenecks
:::

## Complete Example: Parallel Web Scraper

```stratos
package main;

use concurrent;
use http;
use log;

struct FetchResult {
    url: string;
    status: int;
    size: int;
}

fn fetcher(urls: Channel<string>, results: Channel<FetchResult>, wg: WaitGroup) {
    defer wg.done();

    while true {
        val url = urls.receive();
        if (url == null) break;

        log.info("Fetching: " + url);

        val response = http.get(url);
        results.send(FetchResult {
            url: url,
            status: response.status,
            size: response.body.length()
        });
    }
}

fn main() {
    val urlList = [
        "https://example.com",
        "https://google.com",
        "https://github.com"
    ];

    val urls = concurrent.newChannel<string>(10);
    val results = concurrent.newChannel<FetchResult>(10);
    val wg = concurrent.newWaitGroup();

    // Start worker pool
    val numWorkers = 3;
    for val i in 0..numWorkers {
        wg.add(1);
        concurrent.go(|| fetcher(urls, results, wg));
    }

    // Send URLs
    for val url in urlList {
        urls.send(url);
    }
    urls.close();

    // Collect results in separate goroutine
    concurrent.go(|| {
        wg.wait();
        results.close();
    });

    // Process results
    while true {
        val result = results.receive();
        if (result == null) break;

        log.info("$result.url: status=$result.status, size=$result.size bytes");
    }

    log.info("All fetches complete");
}
```

## Quick Reference

| Primitive | Purpose | Example |
|-----------|---------|---------|
| `concurrent.go()` | Launch goroutine | `concurrent.go(\|\| work())` |
| `newChannel<T>()` | Create channel | `val ch = concurrent.newChannel<int>(5)` |
| `ch.send(v)` | Send to channel | `ch.send(42)` |
| `ch.receive()` | Receive from channel | `val v = ch.receive()` |
| `ch.close()` | Close channel | `ch.close()` |
| `newWaitGroup()` | Create wait group | `val wg = concurrent.newWaitGroup()` |
| `wg.add(n)` | Add to wait count | `wg.add(1)` |
| `wg.done()` | Decrement count | `wg.done()` |
| `wg.wait()` | Wait for completion | `wg.wait()` |
| `newMutex()` | Create mutex | `val m = concurrent.newMutex()` |
| `m.lock()` | Acquire lock | `m.lock()` |
| `m.unlock()` | Release lock | `m.unlock()` |
| `sleep(ms)` | Sleep milliseconds | `concurrent.sleep(100)` |

## Next Steps

- [Async Programming](/guide/async) - Async/await patterns
- [Advanced Features](/guide/advanced-features) - Defer, channels operators
- [Error Handling](/guide/error-handling) - Handling errors in concurrent code
