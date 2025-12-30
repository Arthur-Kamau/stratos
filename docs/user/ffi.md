# Stratos FFI (Foreign Function Interface)

## Overview

Stratos FFI allows you to call functions from native libraries (C, C++, Rust, etc.) that export a C ABI. This enables interoperability with existing codebases and performance-critical operations.

## Quick Start

### 1. Load a Library

```stratos
use ffi;

fn main() {
    val lib: int = ffi.load("./libmylib.so");  // Linux/Mac
    // val lib: int = ffi.load("./mylib.dll");  // Windows

    if (lib == -1) {
        println("Failed to load library");
        return;
    }

    // Use library functions...

    ffi.unload(lib);
}
```

### 2. Call Functions

```stratos
// Call function that returns int
val result = ffi.callInt(lib, "add", ["int", "int"], [5, 3]);

// Call function that returns double
val pi = ffi.callDouble(lib, "get_pi", [], []);

// Call function that returns void
ffi.callVoid(lib, "print_hello", [], []);
```

## API Reference

### `ffi.load(path: string) -> int`

Load a shared library and return a library ID.

**Parameters:**
- `path`: Path to the shared library (.so, .dylib, or .dll)

**Returns:**
- Library ID (positive integer) on success
- `-1` on failure

**Example:**
```stratos
val lib: int = ffi.load("./libmath.so");
```

### `ffi.unload(libraryId: int) -> void`

Unload a previously loaded library.

**Parameters:**
- `libraryId`: Library ID returned by `ffi.load()`

**Example:**
```stratos
ffi.unload(lib);
```

### `ffi.callInt(libraryId, functionName, paramTypes, args) -> int`

Call a function that returns an integer.

**Parameters:**
- `libraryId`: Library ID
- `functionName`: Name of the function to call
- `paramTypes`: Array of parameter type strings
- `args`: Array of arguments

**Returns:**
- Integer value returned by the function

**Example:**
```stratos
val result = ffi.callInt(lib, "factorial", ["int"], [5]);
// Calls: int factorial(int n)
```

### `ffi.callDouble(libraryId, functionName, paramTypes, args) -> double`

Call a function that returns a double.

**Example:**
```stratos
val result = ffi.callDouble(lib, "sqrt", ["double"], [9.0]);
// Calls: double sqrt(double x)
```

### `ffi.callVoid(libraryId, functionName, paramTypes, args) -> void`

Call a function that returns void.

**Example:**
```stratos
ffi.callVoid(lib, "greet", ["string"], ["World"]);
// Calls: void greet(const char* name)
```

### `ffi.call(libraryId, functionName, returnType, paramTypes, args) -> any`

Generic function call (for advanced use).

**Example:**
```stratos
val result = ffi.call(lib, "add", "int", ["int", "int"], [5, 3]);
```

## Supported Types

| Stratos Type | C Type | Notes |
|-------------|--------|-------|
| `int` | `int` | 32-bit integer |
| `double` | `double` | 64-bit floating point |
| `string` | `const char*` | C string (null-terminated) |
| `void` | `void` | No return value |

## Parameter Limits

- Maximum 6 parameters per function call
- Can be extended in future versions

## Writing FFI-Compatible Libraries

### C Example

```c
// mylib.c
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

void greet(const char* name) {
    printf("Hello, %s!\n", name);
}

double multiply(double a, double b) {
    return a * b;
}
```

Build:
```bash
gcc -shared -fPIC -o libmylib.so mylib.c
```

### C++ Example

```cpp
// mylib.cpp
#include <string>
#include <algorithm>

// IMPORTANT: Use extern "C" to prevent name mangling
extern "C" {

int str_length(const char* str) {
    return std::strlen(str);
}

int count_vowels(const char* str) {
    std::string s(str);
    int count = 0;
    for (char c : s) {
        c = std::tolower(c);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
            count++;
        }
    }
    return count;
}

} // extern "C"
```

Build:
```bash
g++ -shared -fPIC -o libmylib.so mylib.cpp
```

### Rust Example

```rust
// lib.rs
#[no_mangle]
pub extern "C" fn rust_add(a: i32, b: i32) -> i32 {
    a + b
}

#[no_mangle]
pub extern "C" fn rust_multiply(a: f64, b: f64) -> f64 {
    a * b
}
```

Build:
```bash
rustc --crate-type cdylib lib.rs -o librustlib.so
```

Or with Cargo:
```toml
[lib]
crate-type = ["cdylib"]
```

```bash
cargo build --release
```

## Best Practices

### 1. Error Handling

Always check if library loading succeeded:

```stratos
val lib: int = ffi.load("./libmylib.so");
if (lib == -1) {
    println("ERROR: Failed to load library!");
    return;
}
```

### 2. Resource Cleanup

Always unload libraries when done:

```stratos
ffi.unload(lib);
```

### 3. Type Safety

Ensure parameter types match the C function signature:

```c
// C function:
int calculate(int x, double y);
```

```stratos
// Correct:
val result = ffi.call(lib, "calculate", "int", ["int", "double"], [5, 3.14]);

// Wrong: Type mismatch
val result = ffi.call(lib, "calculate", "int", ["double", "int"], [5.0, 3]);
```

### 4. String Handling

C strings are null-terminated. Ensure your C functions handle strings properly:

```c
void process_string(const char* str) {
    if (!str) return;  // Check for NULL
    // Use string...
}
```

## Platform-Specific Notes

### Linux
- Shared libraries: `.so` extension
- Example: `libmath.so`

### macOS
- Shared libraries: `.dylib` extension
- Example: `libmath.dylib`

### Windows
- Shared libraries: `.dll` extension
- Example: `math.dll`

## Limitations

1. **Maximum 6 parameters** - Functions with more than 6 parameters are not currently supported
2. **Simple types only** - Complex types (structs, pointers, arrays) have limited support
3. **String returns** - Functions returning strings need careful memory management
4. **No callbacks** - Function pointers/callbacks are not currently supported

## Examples

See the `samples/ffi/` directory for complete examples:

- `samples/ffi/c_math/` - C library with math functions
- `samples/ffi/cpp_string/` - C++ library with string operations
- `samples/ffi/rust_calc/` - Rust library (requires Rust toolchain)

## Troubleshooting

### Library Not Found

```
ERROR: Failed to load library!
```

**Solutions:**
- Check the library path is correct
- Use absolute paths or `./` for current directory
- On Linux, check `LD_LIBRARY_PATH`
- Ensure library is built for your architecture

### Function Not Found

```
FFI Error calling my_func: Failed to find function 'my_func' in library
```

**Solutions:**
- Verify function name spelling
- For C++, ensure you used `extern "C"`
- Use `nm -D libmylib.so` to list exported symbols

### Type Mismatch

```
FFI Error: argument type mismatch
```

**Solutions:**
- Verify parameter types match C function signature
- Check parameter count matches
- Ensure return type is correct

## Advanced Topics

### Using System Libraries

You can load system libraries:

```stratos
val libm: int = ffi.load("/lib/x86_64-linux-gnu/libm.so.6");
if (libm != -1) {
    val result = ffi.callDouble(libm, "sqrt", ["double"], [16.0]);
    println(result);  // 4.0
    ffi.unload(libm);
}
```

### Performance Considerations

- FFI calls have overhead compared to native Stratos functions
- Minimize boundary crossings for performance-critical code
- Consider batching operations when possible

## Future Enhancements

Planned features:
- Support for more complex types (structs, arrays)
- Callback support
- More than 6 parameters
- Automatic type inference
- Better string handling

## See Also

- [Samples](../samples/ffi/) - Complete working examples
- [Building Stratos](../BUILD.md) - How to build the interpreter with FFI support
