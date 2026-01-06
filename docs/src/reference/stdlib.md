---
title: Standard Library
description: Complete reference for Stratos standard library modules
---

# Standard Library Reference

Stratos comes with a comprehensive standard library covering common programming needs. All modules are written in Stratos and C++ for optimal performance.

> **Full API Documentation:** For a complete list of all packages and functions, see the [Standard Library API](/stdlib/).

## Module Overview



---
---

## strings

String manipulation and utilities.

### Import

```stratos
use strings;
```

### Case Conversion

```stratos
strings.toUpper(s: string) string
strings.toLower(s: string) string
strings.toTitle(s: string) string
```

### Trimming

```stratos
strings.trim(s: string) string
strings.trimLeft(s: string) string
strings.trimRight(s: string) string
strings.trimPrefix(s: string, prefix: string) string
strings.trimSuffix(s: string, suffix: string) string
```

### Splitting & Joining

```stratos
strings.split(s: string, separator: string) Array<string>
strings.join(parts: Array<string>, separator: string) string
```

### Searching

```stratos
strings.contains(s: string, substr: string) bool
strings.indexOf(s: string, substr: string) int
strings.lastIndexOf(s: string, substr: string) int
strings.startsWith(s: string, prefix: string) bool
strings.endsWith(s: string, suffix: string) bool
strings.count(s: string, substr: string) int
```

### Replacement

```stratos
strings.replace(s: string, old: string, new: string) string
strings.replaceAll(s: string, old: string, new: string) string
```

### Extraction

```stratos
strings.substring(s: string, start: int, end: int) string
strings.charAt(s: string, index: int) string
```

### Formatting

```stratos
strings.repeat(s: string, count: int) string
strings.padLeft(s: string, length: int, pad: string) string
strings.padRight(s: string, length: int, pad: string) string
```

### Utility

```stratos
strings.length(s: string) int
strings.isEmpty(s: string) bool
strings.isBlank(s: string) bool
strings.reverse(s: string) string
strings.compare(a: string, b: string) int
strings.equals(a: string, b: string) bool
strings.equalsIgnoreCase(a: string, b: string) bool
```

### Example

```stratos
use strings;

fn main() {
    val text = "  Hello, Stratos!  ";

    // Trimming
    val cleaned = strings.trim(text);
    print(cleaned);  // "Hello, Stratos!"

    // Case conversion
    val upper = strings.toUpper(cleaned);
    print(upper);  // "HELLO, STRATOS!"

    // Splitting
    val parts = strings.split("apple,banana,orange", ",");
    for (fruit in parts) {
        print(fruit);
    }

    // Searching
    if (strings.contains(cleaned, "Stratos")) {
        val index = strings.indexOf(cleaned, "Stratos");
        print("Found at position: " + index);
    }

    // Formatting
    val padded = strings.padLeft("42", 5, "0");
    print(padded);  // "00042"
}
```

---
---

## io

Input/output operations for files and streams.

### Import

```stratos
use io;
```

### File Operations

```stratos
io.readFile(path: string) string
io.writeFile(path: string, content: string) bool
io.appendFile(path: string, content: string) bool
io.exists(path: string) bool
io.delete(path: string) bool
```

### Example

```stratos
use io;
use log;

fn main() {
    val filename = "data.txt";

    // Write to file
    if (io.writeFile(filename, "Hello, Stratos!")) {
        log.info("File written successfully");
    }

    // Read from file
    if (io.exists(filename)) {
        val content = io.readFile(filename);
        print(content);
    }

    // Append to file
    io.appendFile(filename, "\nNew line");
}
```

---
---

## encoding

### encoding/json

JSON encoding and decoding.

```stratos
use encoding/json;

fn main() {
    // Encode to JSON
    val data = {
        "name": "Alice",
        "age": 30,
        "active": true
    };
    val jsonStr = json.encode(data);
    print(jsonStr);

    // Decode from JSON
    val parsed = json.decode(jsonStr);
    print(parsed.name);
}
```

### encoding/base64

Base64 encoding and decoding.

```stratos
use encoding/base64;

fn main() {
    val original = "Hello, Stratos!";

    // Encode
    val encoded = base64.encode(original);
    print(encoded);

    // Decode
    val decoded = base64.decode(encoded);
    print(decoded);
}
```

### encoding/csv

CSV parsing and generation.

```stratos
use encoding/csv;

fn main() {
    val csvData = "name,age,city\nAlice,30,NYC\nBob,25,LA";

    // Parse CSV
    val rows = csv.parse(csvData);
    for (row in rows) {
        print(row[0]);  // Print names
    }
}
```

---
---

## async

Asynchronous programming primitives for non-blocking operations.

### Import

```stratos
use async;
```

### Async Functions

```stratos
async fn fetchData(url: string) string {
    val response = await http.get(url);
    return await response.text();
}

fn main() async {
    val data = await fetchData("https://api.example.com");
    print(data);
}
```

### Promise

```stratos
async.Promise<T>                           // Promise type
promise.then(callback: fn(T) void) Promise<T>
promise.catch(callback: fn(Error) void) Promise<T>
promise.finally(callback: fn() void) Promise<T>
```

### Concurrent Operations

```stratos
async.all(promises: Array<Promise<T>>) Promise<Array<T>>
async.race(promises: Array<Promise<T>>) Promise<T>
async.sleep(ms: int) Promise<void>
```

### Example

```stratos
use async;
use log;

async fn fetchUser(id: int) User {
    await async.sleep(100);  // Simulate network delay
    return User { id: id, name: "User" + id };
}

fn main() async {
    log.info("Fetching users...");

    // Run multiple async operations concurrently
    val promises = [
        fetchUser(1),
        fetchUser(2),
        fetchUser(3)
    ];

    val users = await async.all(promises);

    for (user in users) {
        log.info("Loaded: " + user.name);
    }
}
```

---
---

## crypto

Cryptographic functions for security operations.

### Import

```stratos
use crypto;
```

### Hashing

```stratos
crypto.sha256(data: string) string
crypto.sha512(data: string) string
crypto.md5(data: string) string
crypto.blake2b(data: string) string
```

### Random

```stratos
crypto.randomBytes(length: int) Array<byte>
crypto.randomInt(min: int, max: int) int
crypto.randomString(length: int) string
crypto.uuid() string                       // Generate UUID v4
```

### Encryption (Symmetric)

```stratos
crypto.aesEncrypt(data: string, key: string) string
crypto.aesDecrypt(encrypted: string, key: string) string
```

### Password Hashing

```stratos
crypto.hashPassword(password: string) string
crypto.verifyPassword(password: string, hash: string) bool
```

### Example

```stratos
use crypto;
use log;

fn main() {
    // Hash data
    val data = "Hello, Stratos!";
    val hash = crypto.sha256(data);
    log.info("SHA-256: " + hash);

    // Generate random values
    val uuid = crypto.uuid();
    log.info("UUID: " + uuid);

    val randomNum = crypto.randomInt(1, 100);
    log.info("Random number: " + randomNum);

    // Password hashing
    val password = "secure_password";
    val hashedPassword = crypto.hashPassword(password);
    log.info("Hashed: " + hashedPassword);

    if (crypto.verifyPassword(password, hashedPassword)) {
        log.info("Password verified!");
    }
}
```

---
---

## os

Operating system interface for file system, processes, and environment.

### Import

```stratos
use os;
```

### File System

```stratos
os.readFile(path: string) string
os.writeFile(path: string, content: string) bool
os.appendFile(path: string, content: string) bool
os.deleteFile(path: string) bool
os.exists(path: string) bool
os.isFile(path: string) bool
os.isDir(path: string) bool
os.mkdir(path: string) bool
os.mkdirAll(path: string) bool              // Create with parents
os.readDir(path: string) Array<string>
os.rename(oldPath: string, newPath: string) bool
os.copy(src: string, dest: string) bool
```

### Process Management

```stratos
os.exec(command: string, args: Array<string>) ProcessResult
os.spawn(command: string, args: Array<string>) Process
os.exit(code: int) void
os.getpid() int
```

### Environment

```stratos
os.getenv(name: string) string?
os.setenv(name: string, value: string) bool
os.environ() Map<string, string>
os.cwd() string                             // Current working directory
os.chdir(path: string) bool                 // Change directory
os.homedir() string
os.tmpdir() string
```

### Platform Info

```stratos
os.platform() string                        // "linux", "darwin", "windows"
os.arch() string                            // "x64", "arm64"
os.hostname() string
```

### Example

```stratos
use os;
use log;

fn main() {
    // File operations
    val filename = "data.txt";
    os.writeFile(filename, "Hello, Stratos!");

    if (os.exists(filename)) {
        val content = os.readFile(filename);
        log.info("Content: " + content);
    }

    // Environment variables
    val home = os.getenv("HOME");
    log.info("Home directory: " + home);

    // Process execution
    val result = os.exec("ls", ["-la"]);
    log.info("Output: " + result.stdout);

    // Platform info
    log.info("Platform: " + os.platform());
    log.info("Architecture: " + os.arch());
    log.info("Hostname: " + os.hostname());
}
```

---
---

## testing

Testing framework for unit tests and assertions.

### Import

```stratos
use testing;
```

### Test Functions

```stratos
testing.describe(name: string, fn() void)
testing.it(name: string, fn() void)
testing.beforeEach(fn() void)
testing.afterEach(fn() void)
testing.beforeAll(fn() void)
testing.afterAll(fn() void)
```

### Assertions

```stratos
testing.expect(value: T) Assertion<T>

// Assertion methods
assertion.toBe(expected: T)
assertion.toEqual(expected: T)
assertion.toBeNull()
assertion.toBeUndefined()
assertion.toBeTruthy()
assertion.toBeFalsy()
assertion.toContain(item: T)
assertion.toThrow()
assertion.toBeGreaterThan(value: number)
assertion.toBeLessThan(value: number)
```

### Mock Functions

```stratos
testing.mock(fn: Function) MockFunction
mock.mockReturnValue(value: any)
mock.mockImplementation(fn: Function)
mock.calls() Array<Array<any>>
mock.reset()
```

### Example

```stratos
use testing;

fn add(a: int, b: int) int {
    return a + b;
}

fn divide(a: int, b: int) int {
    if (b == 0) {
        throw Error("Division by zero");
    }
    return a / b;
}

testing.describe("Math Operations", fn() {
    testing.it("should add two numbers", fn() {
        val result = add(2, 3);
        testing.expect(result).toBe(5);
    });

    testing.it("should handle negative numbers", fn() {
        val result = add(-5, 3);
        testing.expect(result).toBe(-2);
    });

    testing.it("should divide numbers", fn() {
        val result = divide(10, 2);
        testing.expect(result).toBe(5);
    });

    testing.it("should throw on division by zero", fn() {
        testing.expect(fn() {
            divide(10, 0);
        }).toThrow();
    });
});

testing.describe("String Operations", fn() {
    var str: string;

    testing.beforeEach(fn() {
        str = "Hello, World!";
    });

    testing.it("should have correct length", fn() {
        testing.expect(str.length()).toBe(13);
    });

    testing.it("should contain substring", fn() {
        testing.expect(str).toContain("World");
    });
});
```

---

## Module Usage Patterns

### Importing Multiple Modules

```stratos
use math;
use strings;
use log;

fn main() {
    log.info("Starting calculation");

    val result = math.sqrt(16.0);
    val formatted = strings.padLeft(result, 10, " ");

    log.info("Result: " + formatted);
}
```

### Selective Imports (Future Feature)

```stratos
// Future: Import specific functions
use math.{sin, cos, PI};
use strings.{trim, split};
```

## See Also

- [Language Guide](/docs/v1.0.0/language/basics) - Learn Stratos language features
- [Examples](/docs/v1.0.0/examples) - Practical code examples
- [API Reference](/docs/v1.0.0/api) - Complete API documentation
