---
title: Standard Library API
description: Comprehensive API reference for Stratos standard library
---

# Stratos Standard Library API

The Stratos Standard Library provides a comprehensive set of packages for common programming tasks.

## Packages

### Core
- [async](async.md) - Asynchronous programming primitives
- [concurrent](concurrent.md) - Concurrency support
- [ffi](ffi.md) - Foreign Function Interface
- [testing](testing.md) - Testing framework

### Data & Structures
- [collections](collections.md) - List, Map, Set and other data structures
- [csv](csv.md) - CSV parsing and generation
- [json](json.md) - JSON parsing and generation
- [base64](base64.md) - Base64 encoding/decoding
- [zip](zip.md) - ZIP archive manipulation

### Input/Output
- [io](io.md) - Core I/O interfaces
- [log](log.md) - Logging facilities
- [terminal](terminal.md) - Terminal manipulation (colors, cursor)

### Networking
- [http](http.md) - HTTP client and server
- [net](net.md) - Low-level network primitives (TCP/UDP)
- [websocket](websocket.md) - WebSocket client and server

### Text Processing
- [strings](strings.md) - String manipulation
- [regex](regex.md) - Regular expressions
- [convert](convert.md) - Type conversion utilities

### Utilities
- [crypto](crypto.md) - Cryptographic functions
- [math](math.md) - Mathematical functions and constants
- [os](os.md) - Operating system interactions
- [time](time.md) - Date and time handling
- [greeting](greeting.md) - (Example/Test package)

## Usage

To use a standard library package, import it at the top of your Stratos file:

```stratos
use io;
use strings;

fn main() {
    io.println(strings.toUpper("hello world"));
}
```