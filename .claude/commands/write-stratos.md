# Write Stratos Code

Write Stratos (.st) code for a given task or specification.

## Arguments
- $ARGUMENTS: Description of what the code should do (e.g., "fibonacci sequence", "REST API server", "file parser")

## Instructions

Write idiomatic Stratos code based on $ARGUMENTS. Follow these conventions:

### Code Style
- 4 spaces indentation
- Opening braces on same line
- Space after keywords (`if`, `for`, `fn`, `class`, `while`)
- Space around operators (`=`, `+`, `==`, `!=`, `->`)
- Use `val` for immutable bindings (preferred), `var` only when mutation is needed
- Use expression bodies (`=> expr`) for single-expression functions
- Use string interpolation (`"${expr}"`) instead of concatenation
- Always declare package at top of file

### Available Standard Library

```stratos
import "std/math";        // sqrt, abs, sin, cos, pow, random, PI, E
import "std/strings";     // toUpper, toLower, trim, split, contains, replace
import "std/io";          // readFile, writeFile, exists, mkdir
import "std/fs";          // File system operations
import "std/log";         // debug, info, warn, error, fatal
import "std/time";        // now, sleep, Duration
import "std/collections"; // List, Map, Set, Queue, Stack
import "std/async";       // Promise, delay
import "std/concurrent";  // Channel, Mutex, WaitGroup, go
import "std/net";         // HttpServer, HttpClient, Request, Response
import "std/db";          // Database (SQLite, PostgreSQL, MySQL, Redis)
import "std/encoding";    // JSON.parse, JSON.stringify, base64, CSV
import "std/crypto";      // hash, encrypt, decrypt
import "std/regex";       // Regex, match, replace
import "std/testing";     // TestSuite, Test, assertions
import "std/os";          // env, args, exit, exec
import "std/terminal";    // colors, cursor, input
import "std/template";    // Template engine
import "std/websocket";   // WebSocket client/server
import "std/convert";     // Type conversions
import "std/zip";         // Archive compression
```

### Language Features Cheatsheet

```stratos
// Variables
val x = 42;                    // immutable, inferred type
var y: string = "hello";      // mutable, explicit type

// Functions
fn add(a: int, b: int) int { return a + b; }
fn double(x: int) int => x * 2;  // expression body
val square = fn(x: int) int => x * x;  // lambda

// Classes
class MyClass {
    public field: string;
    constructor(field: string) { this.field = field; }
    fn method() string { return this.field; }
}

// Interfaces
interface Printable { fn toString() string; }

// Structs
struct Point { x: float; y: float; }

// Enums
enum Status { Active, Inactive, Pending(reason: string) }

// Control flow
if cond { } else { }
when value { 1 -> x; 2 -> y; else -> z; }
for i in 0..10 { }
for item in list { }
while cond { }
select { case msg = <-ch -> handle(msg); }

// Error handling
val r = expect riskyFn() else { return; };
try { } catch (e: Error) { }

// Async
async fn fetch() string { return await getData(); }

// Concurrency
val ch = Channel<int>();
go fn() { ch.send(42); };

// Defer
defer cleanup();
```

### Process
1. Understand the task from $ARGUMENTS
2. Determine which std modules are needed
3. Write clean, idiomatic Stratos code
4. Include a `stratos.conf` if creating a full project
5. Add comments only where logic isn't self-evident
