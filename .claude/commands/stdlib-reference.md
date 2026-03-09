# Standard Library Reference

Show the API reference for a Stratos standard library module.

## Arguments
- $ARGUMENTS: Module name (e.g., "math", "strings", "net", "db", "collections")

## Instructions

1. Parse the module name from $ARGUMENTS.
2. Read the module's source file at `std/<module>/init.st` (or other .st files in the directory).
3. Present the module's API:
   - List all exported functions with their signatures
   - List all exported classes/structs/interfaces
   - Show brief descriptions from doc comments
   - Provide usage examples

If no module name is given, list all available modules:
```
std/
├── async          - Promises, async/await runtime
├── collections    - List, Map, Set, Queue, Stack
├── concurrent     - Goroutines, channels, mutex, waitgroup
├── convert        - Type conversion utilities
├── crypto         - Hashing (SHA, MD5), encryption (AES)
├── db             - Database (SQLite, PostgreSQL, MySQL, Redis)
├── encoding       - JSON, base64, CSV, XML, YAML
├── ffi            - Foreign function interface
├── fs             - File system operations
├── greeting       - Greeting utilities
├── io             - I/O operations
├── log            - Structured logging
├── math           - Math functions (trig, stats, random)
├── net            - TCP/UDP, HTTP server/client
├── os             - OS interface, env, process
├── regex          - Regular expressions
├── strings        - String manipulation
├── template       - Template engine
├── terminal       - Terminal colors, cursor, input
├── testing        - Unit testing framework
├── time           - Time, duration, formatting
├── websocket      - WebSocket client/server
└── zip            - Archive compression
```

4. For external packages, also check `external/`:
   - `stratos-cli-args` - CLI argument parsing
   - `stratos-colors` - Terminal color utilities
   - `stratos-linq` - LINQ-style query operations
   - `stratos-valid` - Data validation library
