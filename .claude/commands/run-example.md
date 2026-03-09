# Run a Stratos Example

Run one of the 70+ example projects to test language features.

## Arguments
- $ARGUMENTS: Example name or pattern (e.g., "hello-world", "oop", "async", "all")

## Instructions

1. List available examples by reading `examples/` directory.

2. If $ARGUMENTS is "all" or "list", list all available examples grouped by category:
   - **Basics**: hello-world, variables, comments-and-documentation, strings-utils, string-interpolation, escape-sequences
   - **Control Flow**: conditionals, loops, when-statement-test, when_docs_demo, select-test, select-syntax-test
   - **Functions**: function-definition, callbacks, math_and_recursion
   - **OOP**: oop-classes, oop-demo, oop-inheritance, oop-interfaces, method_test, method_test2, method_test3, oop-class-visibility
   - **Data Types**: arrays, maps, struct-type, enum-type, booleans, casts, number-convert, data-convert, range-test
   - **Advanced**: async-demo, 05_async_await, concurrency, threads, defer-test, destructuring-test, expect
   - **Standard Library**: file_io, file_io_simple, json-data, database, regex-demo, uuid-demo, terminal-demo, colors-demo
   - **Networking**: http-simple, websocket
   - **FFI**: ffi_c_math, ffi_cpp_string
   - **Package Management**: imports, package, dependency-test, greeting-test, prelude-demo
   - **External Packages**: linq-demo, stdlib-examples, sorting-no-gc, fibonacci-no-gc, primes-no-gc, calculator-no-gc, string-utils-no-gc
   - **Testing**: testing, test-prelude-functions, test-static-compile, empty-project

3. If a specific example name is given, run it:
   ```bash
   ./src/build/stratos run ./examples/<example-name>/
   ```

4. If a partial match or category is given (e.g., "oop"), run all matching examples.

5. Report the output and whether the example ran successfully.

6. If the stratos binary doesn't exist, suggest running `/build` first.
