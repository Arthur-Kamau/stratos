# Stratos Standard Library Examples

This directory contains example projects demonstrating the Stratos standard library modules. Each example is a complete Stratos project with its own `stratos.conf` file.

## Example Projects

### 01_basic_math
Demonstrates the `math` module:
- Mathematical constants (PI, E, PHI)
- Trigonometric functions
- Powers and roots
- Number theory (primes, GCD, LCM, factorial)
- Statistics (sum, average, median)
- Utility functions (clamp, lerp)

### 02_file_io
Demonstrates the `io` module:
- Reading and writing files
- Line-based file operations
- Appending to files
- File information and metadata
- Directory operations (create, list, remove)
- File copying

### 03_concurrency
Demonstrates the `concurrent` module:
- Goroutines for concurrent execution
- Channels for communication
- Worker pools
- Mutexes for synchronization
- WaitGroups for coordination
- Pipeline pattern

### 04_http_server
Demonstrates the `net/http` module:
- Creating HTTP servers
- Routing (GET, POST, DELETE)
- Middleware (logging, CORS, rate limiting)
- JSON request/response handling
- RESTful API design
- In-memory data store

### 05_async_await
Demonstrates the `async` module:
- Creating and chaining Promises
- Promise.all and Promise.race
- Async map, filter, reduce
- Retry with exponential backoff
- Throttling and debouncing
- Sequential and parallel execution
- Wait until condition

### 06_testing
Demonstrates the `testing` module:
- Defining test suites with `describe()`
- Writing test cases with `it()`
- Assertions and expectations
- Setup/teardown hooks (beforeEach, afterEach)
- Mocking and spying
- Benchmarking
- Test reporting and JSON export

## Running the Examples

Each example is now a complete Stratos project. To run an example, use the `stratos run` command:

```bash
# From the samples directory
cd /home/kamau/Development/Projects/stratos/samples

# Run an example project by passing the directory
../interpreter/C++/build/stratos run stdlib_examples/01_basic_math/

# Or with verbose output
../interpreter/C++/build/stratos run stdlib_examples/02_file_io/ -v

# From within an example directory
cd stdlib_examples/03_concurrency
../../../interpreter/C++/build/stratos run .
```

## Project Structure

Each example follows the standard Stratos project layout:

```
example_name/
├── stratos.conf    # Project configuration
└── src/
    └── main.st     # Entry point with main() function
```

The `stratos.conf` file specifies:
- Project metadata (name, version, author)
- Project type (executable)
- Entry point (src/main.st)
- Build output location

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
5. Build each example: `stratos build example_dir/`

## Note

These examples demonstrate the API design of the Stratos standard library. The native function bindings in the C++ interpreter still need to be implemented for full functionality.
