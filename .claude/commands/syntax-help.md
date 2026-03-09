# Stratos Language Syntax Help

Provide syntax help and examples for Stratos language features.

## Arguments
- $ARGUMENTS: The topic or feature to explain (e.g., "variables", "classes", "async", "when", "loops")

## Instructions

Look up the topic from $ARGUMENTS and provide a clear explanation with working code examples. Reference the CLAUDE.md for the language quick reference, and read actual example projects from `examples/` for real-world usage patterns.

### Topic Reference Map

Use these example directories for each topic:

| Topic | Example Directory | Key Concepts |
|-------|-------------------|-------------|
| variables, val, var | `examples/variables/` | `val` (immutable), `var` (mutable), type inference, explicit types |
| functions, fn | `examples/function-definition/` | Parameters, return types, expression bodies, lambdas |
| classes, oop | `examples/oop-classes/`, `examples/oop-demo/` | Constructor, methods, public/private, `this` |
| inheritance, extends | `examples/oop-inheritance/` | `extends`, `super`, `override` |
| interfaces, implements | `examples/oop-interfaces/` | Interface declaration, implementation |
| structs | `examples/struct-type/` | Struct declaration, instantiation |
| enums | `examples/enum-type/` | Enum variants, associated values |
| arrays | `examples/arrays/` | Array literals, methods (push, pop, map, filter) |
| maps | `examples/maps/` | Map literals, access, iteration |
| strings | `examples/strings-utils/`, `examples/string-interpolation/` | String methods, `${expr}` interpolation |
| loops, for, while | `examples/loops/` | `for..in`, ranges (`0..10`), `while`, `loop` |
| conditionals, if | `examples/conditionals/` | `if/else if/else` |
| when, pattern matching | `examples/when-statement-test/`, `examples/when_docs_demo/` | `when` expression, `is`, `in`, ranges |
| async, await | `examples/async-demo/`, `examples/05_async_await/` | `async fn`, `await`, promises |
| concurrency, goroutines | `examples/concurrency/` | `go`, channels, `select` |
| select | `examples/select-test/`, `examples/select-syntax-test/` | Channel select statement |
| imports, packages | `examples/imports/` | `package`, `import`, `as` alias |
| callbacks, closures | `examples/callbacks/` | Function references, closures |
| error handling, expect | `examples/expect/` | `expect...else`, try/catch |
| defer | `examples/defer-test/` | `defer` statement |
| destructuring | `examples/destructuring-test/` | Tuple/object destructuring |
| casting, type conversion | `examples/casts/`, `examples/number-convert/` | `as` keyword |
| testing | `examples/testing/` | TestSuite, assertions |
| http, server | `examples/http-simple/` | HTTP server, routes |
| database, sqlite | `examples/database/` | Database connections, queries |
| file io | `examples/file_io/`, `examples/file_io_simple/` | File read/write |
| ffi | `examples/ffi_c_math/`, `examples/ffi_cpp_string/` | Foreign function interface |
| regex | `examples/regex-demo/` | Regular expressions |
| json | `examples/json-data/` | JSON parsing/serialization |
| linq | `examples/linq-demo/` | LINQ-style queries |
| range | `examples/range-test/` | Range expressions |
| comments, docs | `examples/comments-and-documentation/` | `//`, `/* */`, `///` doc comments |
| terminal, colors | `examples/terminal-demo/`, `examples/colors-demo/` | Terminal colors, formatting |
| threads | `examples/threads/` | Thread creation |
| websocket | `examples/websocket/` | WebSocket client/server |
| prelude | `examples/prelude-demo/` | Built-in prelude functions |
| uuid | `examples/uuid-demo/` | UUID generation |

### Response Format

1. Read the relevant example file(s) from the examples directory
2. Provide a brief explanation of the feature
3. Show the syntax pattern
4. Show a working example from the actual codebase
5. List related features or topics the user might want to explore next

If the topic is not recognized, list all available topics.
