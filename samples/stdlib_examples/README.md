# Stratos Standard Library Examples

This directory contains example programs demonstrating the Stratos standard library modules.

## Examples

### 01_basic_math.st
Demonstrates the `math` module:
- Mathematical constants (PI, E, PHI)
- Trigonometric functions
- Powers and roots
- Number theory (primes, GCD, LCM, factorial)
- Statistics (sum, average, median)
- Utility functions (clamp, lerp)

### 02_file_io.st
Demonstrates the `io` module:
- Reading and writing files
- Line-based file operations
- Appending to files
- File information and metadata
- Directory operations (create, list, remove)
- File copying

### 03_concurrency.st
Demonstrates the `concurrent` module:
- Goroutines for concurrent execution
- Channels for communication
- Worker pools
- Mutexes for synchronization
- WaitGroups for coordination
- Pipeline pattern

### 04_http_server.st
Demonstrates the `net/http` module:
- Creating HTTP servers
- Routing (GET, POST, DELETE)
- Middleware (logging, CORS, rate limiting)
- JSON request/response handling
- RESTful API design
- In-memory data store

### 05_async_await.st
Demonstrates the `async` module:
- Creating and chaining Promises
- Promise.all and Promise.race
- Async map, filter, reduce
- Retry with exponential backoff
- Throttling and debouncing
- Sequential and parallel execution
- Wait until condition

### 06_testing.st
Demonstrates the `testing` module:
- Defining test suites with `describe()`
- Writing test cases with `it()`
- Assertions and expectations
- Setup/teardown hooks (beforeEach, afterEach)
- Mocking and spying
- Benchmarking
- Test reporting and JSON export

## Running the Examples

To run an example, use the Stratos interpreter:

```bash
# From the interpreter build directory
cd interpreter/C++/build

# Run a specific example
./stratos ../../../samples/stdlib_examples/01_basic_math.st

# Or from the examples directory
cd samples/stdlib_examples
../../interpreter/C++/build/stratos 01_basic_math.st
```

## Standard Library Modules Used

- **math** - Mathematical functions and constants
- **strings** - String manipulation utilities
- **io** - File and stream I/O operations
- **log** - Structured logging
- **time** - Date and time manipulation
- **collections** - Enhanced data structures (List, Map, Set, Queue, Stack)
- **encoding/json** - JSON parsing and serialization
- **encoding/base64** - Base64 encoding/decoding
- **encoding/csv** - CSV parsing and writing
- **concurrent** - Goroutines, channels, and synchronization
- **async** - Promises and async/await
- **net** - TCP/UDP sockets
- **net/http** - HTTP server and client
- **os** - Operating system interface
- **crypto** - Cryptographic functions
- **testing** - Unit testing framework

## Next Steps

1. Explore the standard library source code in `std/`
2. Read the comprehensive documentation in `design/standard_library.md`
3. Try modifying the examples to experiment with different features
4. Create your own programs using the standard library

## Note

These examples demonstrate the API design of the Stratos standard library. The native function bindings in the C++ interpreter still need to be implemented for full functionality.
