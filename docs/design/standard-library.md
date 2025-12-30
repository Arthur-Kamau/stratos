# Stratos Standard Library Design

## Overview

The Stratos standard library provides a comprehensive set of modules for common programming tasks, inspired by Go's simplicity, Kotlin's expressiveness, and TypeScript's async model.

## Design Principles

1. **Minimalist API** - Simple, intuitive interfaces (Go-inspired)
2. **Type Safety** - Strong typing with optional types
3. **Concurrency First** - Built-in support for lightweight threads
4. **Async/Await** - Modern async programming (TypeScript-inspired)
5. **Zero Dependencies** - Self-contained standard library
6. **Cross-Platform** - Platform-agnostic where possible

## Module Structure

```
stdlib/
├── io/              # File and stream I/O
├── concurrent/      # Goroutine-style concurrency
├── async/           # Async/await primitives
├── net/             # Networking (sockets, HTTP)
├── time/            # Date and time
├── log/             # Structured logging
├── encoding/        # JSON, Base64, etc.
├── math/            # Extended math functions
├── collections/     # Lists, maps, sets
├── strings/         # String manipulation
├── os/              # Operating system interface
├── crypto/          # Cryptography
└── testing/         # Unit testing framework
```

## Core Modules

### 1. `io` - File and Stream I/O

**Inspired by**: Go's `io` and `os` packages

```stratos
use io;

// File operations
fn readFile() {
    val content = io.readFile("data.txt");
    print(content);
}

fn writeFile() {
    io.writeFile("output.txt", "Hello, World!");
}

// Streaming
fn copyFile() {
    val reader = io.openReader("input.txt");
    val writer = io.openWriter("output.txt");
    io.copy(writer, reader);
    reader.close();
    writer.close();
}

// With defer (Go-style)
fn readWithDefer() {
    val file = io.open("data.txt");
    defer file.close();

    val line = file.readLine();
    print(line);
}
```

**API:**
- `readFile(path: string) -> string`
- `writeFile(path: string, content: string) -> Result<void, Error>`
- `appendFile(path: string, content: string) -> Result<void, Error>`
- `open(path: string, mode: string) -> File`
- `exists(path: string) -> bool`
- `delete(path: string) -> Result<void, Error>`
- `mkdir(path: string) -> Result<void, Error>`
- `readDir(path: string) -> Array<FileInfo>`

### 2. `concurrent` - Lightweight Threads

**Inspired by**: Go's goroutines and channels

```stratos
use concurrent;

// Goroutine-style
fn worker(id: int, ch: Channel<int>) {
    val result = processData(id);
    ch.send(result);
}

fn main() {
    val ch = concurrent.makeChannel<int>(10);

    // Launch workers
    for i in 0..10 {
        concurrent.go(() => worker(i, ch));
    }

    // Collect results
    for i in 0..10 {
        val result = ch.receive();
        print(result);
    }
}

// Mutex for shared state
fn withMutex() {
    val mu = concurrent.Mutex();
    var counter = 0;

    concurrent.go(() => {
        mu.lock();
        defer mu.unlock();
        counter = counter + 1;
    });
}

// WaitGroup
fn parallelWork() {
    val wg = concurrent.WaitGroup();

    for i in 0..5 {
        wg.add(1);
        concurrent.go(() => {
            defer wg.done();
            doWork(i);
        });
    }

    wg.wait();
}
```

**API:**
- `go(fn: Function) -> void` - Launch goroutine
- `makeChannel<T>(size: int) -> Channel<T>`
- `Mutex` - Mutual exclusion lock
- `RWMutex` - Read-write lock
- `WaitGroup` - Wait for goroutines to finish
- `Once` - Execute function once
- `select` - Wait on multiple channels

### 3. `async` - Async/Await

**Inspired by**: TypeScript and JavaScript

```stratos
use async;

// Async functions
async fn fetchData(url: string) -> string {
    val response = await http.get(url);
    return await response.text();
}

// Promise API
fn promiseExample() {
    val promise = async.Promise<string>((resolve, reject) => {
        val data = fetchFromDB();
        if data != None {
            resolve(data);
        } else {
            reject(Error("Not found"));
        }
    });

    promise
        .then((data) => print(data))
        .catch((err) => log.error(err));
}

// Parallel execution
async fn fetchAll() {
    val results = await async.all([
        fetchData("url1"),
        fetchData("url2"),
        fetchData("url3")
    ]);

    return results;
}

// Race condition
async fn fastest() {
    val result = await async.race([
        fetchData("mirror1"),
        fetchData("mirror2")
    ]);

    return result;
}
```

**API:**
- `async fn` - Async function syntax
- `await` - Wait for promise
- `Promise<T>` - Promise type
- `all(promises: Array<Promise<T>>) -> Promise<Array<T>>`
- `race(promises: Array<Promise<T>>) -> Promise<T>`
- `sleep(ms: int) -> Promise<void>`
- `timeout(promise: Promise<T>, ms: int) -> Promise<T>`

### 4. `net` - Networking

**Inspired by**: Go's `net` and `net/http`

```stratos
use net;
use net.http;

// TCP Server
fn tcpServer() {
    val listener = net.listen("tcp", ":8080");
    defer listener.close();

    while true {
        val conn = listener.accept();
        concurrent.go(() => handleConnection(conn));
    }
}

fn handleConnection(conn: Connection) {
    defer conn.close();

    val data = conn.read(1024);
    conn.write("Echo: " + data);
}

// HTTP Server
fn httpServer() {
    val server = http.Server();

    server.get("/", (req, res) => {
        res.json({ message: "Hello, World!" });
    });

    server.post("/data", (req, res) => {
        val body = req.body();
        log.info("Received:", body);
        res.status(201).send("Created");
    });

    server.listen(8080);
}

// HTTP Client
async fn httpClient() {
    val response = await http.get("https://api.example.com/data");
    val data = await response.json();

    val postResponse = await http.post("https://api.example.com/create", {
        headers: { "Content-Type": "application/json" },
        body: json.stringify({ name: "Test" })
    });
}
```

**API:**
- `listen(network: string, address: string) -> Listener`
- `dial(network: string, address: string) -> Connection`
- `http.Server()` - HTTP server
- `http.get/post/put/delete()` - HTTP client
- `websocket.dial(url: string)` - WebSocket client

### 5. `time` - Date and Time

**Inspired by**: Go's `time` and Kotlin's time API

```stratos
use time;

fn timeOperations() {
    // Current time
    val now = time.now();
    print(now.format("2006-01-02 15:04:05"));

    // Duration
    val duration = time.seconds(30);
    val future = now.add(duration);

    // Sleep
    time.sleep(time.milliseconds(100));

    // Parse
    val parsed = time.parse("2024-01-15", "2006-01-02");

    // Comparison
    if future.after(now) {
        print("Future is later");
    }

    // Components
    val year = now.year();
    val month = now.month();
    val day = now.day();
}

// Timers
fn timerExample() {
    val timer = time.after(time.seconds(5));
    print("Waiting 5 seconds...");
    <-timer; // Receive from timer channel
    print("Done!");
}

// Tickers
fn tickerExample() {
    val ticker = time.tick(time.seconds(1));

    for i in 0..10 {
        <-ticker;
        print("Tick ", i);
    }

    ticker.stop();
}
```

**API:**
- `now() -> Time`
- `parse(value: string, layout: string) -> Time`
- `Time.format(layout: string) -> string`
- `Time.add(duration: Duration) -> Time`
- `Time.sub(other: Time) -> Duration`
- `sleep(duration: Duration)`
- `after(duration: Duration) -> Channel`
- `tick(duration: Duration) -> Ticker`

### 6. `log` - Structured Logging

**Inspired by**: Go's slog and Kotlin's logging

```stratos
use log;

fn loggingExample() {
    // Basic logging
    log.info("Application started");
    log.debug("Debug information");
    log.warn("Warning message");
    log.error("Error occurred");

    // Structured logging
    log.info("User logged in", {
        userId: 123,
        username: "alice",
        ip: "192.168.1.1"
    });

    // With context
    val logger = log.with({
        service: "api",
        version: "1.0"
    });

    logger.info("Request received");
    logger.error("Request failed", { error: err });
}

// Custom logger
fn customLogger() {
    val logger = log.Logger({
        level: log.DEBUG,
        format: log.JSON,
        output: io.openWriter("app.log")
    });

    logger.info("Custom log message");
}
```

**API:**
- `info/debug/warn/error(msg: string, fields?: Object)`
- `Logger(config: LogConfig) -> Logger`
- `with(fields: Object) -> Logger`
- `setLevel(level: LogLevel)`

### 7. `encoding` - Data Encoding

**Inspired by**: Go's encoding packages

```stratos
use encoding.json;
use encoding.base64;
use encoding.csv;

// JSON
fn jsonExample() {
    val data = { name: "Alice", age: 30 };
    val encoded = json.stringify(data);
    val decoded = json.parse(encoded);

    // Type-safe decoding
    class User {
        var name: string;
        var age: int;
    }

    val user = json.decode<User>(encoded);
}

// Base64
fn base64Example() {
    val data = "Hello, World!";
    val encoded = base64.encode(data);
    val decoded = base64.decode(encoded);
}

// CSV
fn csvExample() {
    val reader = csv.Reader(io.open("data.csv"));

    while val row = reader.read() {
        print(row[0], row[1]);
    }
}
```

**API:**
- `json.stringify/parse/encode/decode`
- `base64.encode/decode`
- `csv.Reader/Writer`
- `xml.parse/stringify`
- `yaml.parse/stringify`

### 8. `math` - Extended Math

```stratos
use math;

fn mathOperations() {
    val x = math.sqrt(16.0);
    val y = math.pow(2.0, 3.0);
    val z = math.sin(math.PI / 2);

    val max = math.max(10, 20);
    val min = math.min(10, 20);

    val rounded = math.round(3.7);
    val floored = math.floor(3.7);
    val ceiled = math.ceil(3.2);

    val random = math.random();
    val randomInt = math.randomInt(1, 100);
}
```

**API:**
- Trigonometry: `sin/cos/tan/asin/acos/atan`
- Power: `sqrt/pow/exp/log/log10`
- Rounding: `round/floor/ceil/abs`
- Constants: `PI/E`
- Random: `random/randomInt`

### 9. `collections` - Data Structures

**Inspired by**: Kotlin collections

```stratos
use collections;

fn collectionExamples() {
    // List
    val list = collections.List<int>();
    list.add(1);
    list.add(2);
    list.remove(0);

    val filtered = list.filter((x) => x > 10);
    val mapped = list.map((x) => x * 2);
    val sum = list.reduce((acc, x) => acc + x, 0);

    // Map
    val map = collections.Map<string, int>();
    map.set("key", 42);
    val value = map.get("key");
    map.delete("key");

    // Set
    val set = collections.Set<string>();
    set.add("item");
    if set.has("item") {
        print("Found");
    }
}
```

**API:**
- `List<T>` - Dynamic array
- `Map<K, V>` - Hash map
- `Set<T>` - Hash set
- `Queue<T>` - FIFO queue
- `Stack<T>` - LIFO stack
- Higher-order functions: `map/filter/reduce/forEach`

### 10. `strings` - String Manipulation

```stratos
use strings;

fn stringOperations() {
    val s = "Hello, World!";

    val upper = strings.toUpper(s);
    val lower = strings.toLower(s);
    val trimmed = strings.trim("  hello  ");

    val parts = strings.split(s, ", ");
    val joined = strings.join(parts, "-");

    if strings.contains(s, "World") {
        print("Found");
    }

    val replaced = strings.replace(s, "World", "Stratos");
    val repeated = strings.repeat("ha", 3);

    val index = strings.indexOf(s, "World");
    val substr = strings.substring(s, 0, 5);
}
```

**API:**
- Case: `toUpper/toLower/toTitle`
- Trim: `trim/trimLeft/trimRight`
- Split/Join: `split/join`
- Search: `contains/indexOf/lastIndexOf`
- Modify: `replace/replaceAll/repeat`
- Extract: `substring/charAt`

## Implementation Strategy

### Phase 1: Core Modules (Week 1)
1. `math` - Pure functions, easiest to implement
2. `strings` - String utilities
3. `io` - File I/O basics
4. `log` - Basic logging

### Phase 2: Collections & Encoding (Week 2)
1. `collections` - Data structures
2. `encoding.json` - JSON support
3. `encoding.base64` - Base64

### Phase 3: Time & OS (Week 3)
1. `time` - Date/time operations
2. `os` - OS interface

### Phase 4: Concurrency (Week 4)
1. `concurrent` - Goroutines and channels
2. `async` - Async/await

### Phase 5: Networking (Week 5)
1. `net` - TCP/UDP sockets
2. `net.http` - HTTP server/client

### Phase 6: Advanced (Week 6)
1. `crypto` - Hashing and encryption
2. `testing` - Test framework
3. Optimization and documentation

## Native vs Pure Implementation

### Native Functions (C++ implementation)
- File I/O operations
- Network sockets
- Threading primitives
- System calls
- Crypto operations

### Pure Stratos Implementation
- Higher-order functions (map, filter, reduce)
- String manipulation utilities
- Math utilities
- Data structures (where possible)

## Integration with Interpreter

### 1. Native Function Registry

```cpp
// In IRGenerator or new NativeRegistry class
class NativeRegistry {
    std::map<std::string, NativeFunction> functions;

    void registerFunction(const std::string& name, NativeFunction fn);
    bool isNative(const std::string& name);
    NativeFunction get(const std::string& name);
};
```

### 2. Module Loading

```stratos
// Import syntax
use io;                    // Import entire module
use io.{readFile, writeFile};  // Named imports
use io as fileio;         // Aliased import
```

### 3. Standard Library Path

```
$STRATOS_ROOT/stdlib/
  io/init.st
  concurrent/init.st
  ...
```

## Testing Strategy

Each module includes:
1. Unit tests
2. Integration tests
3. Performance benchmarks
4. Documentation examples

## Documentation

Each module includes:
- API reference
- Usage examples
- Best practices
- Performance notes

## Future Enhancements

1. **Database** - SQL and NoSQL drivers
2. **Graphics** - 2D rendering
3. **GUI** - Native GUI toolkit
4. **ML** - Machine learning utilities
5. **Compression** - gzip, zlib support
6. **Regex** - Regular expressions
