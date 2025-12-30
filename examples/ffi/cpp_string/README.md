# C++ FFI Sample - String Library

This sample demonstrates calling C++ functions from Stratos using FFI with C ABI.

## Building the C++ Library

```bash
cd samples/ffi/cpp_string
chmod +x build.sh
./build.sh
```

This will create `libstringlib.so` - a shared library containing C++ string manipulation functions.

## Running the Test

From the cpp_string directory:

```bash
../../../interpreter/C++/build/stratos run
```

## Functions in the Library

The C++ library (`stringlib.cpp`) exports the following functions with `extern "C"`:

### String Operations
- `int str_length(const char* str)` - Get string length
- `const char* str_to_upper(const char* str)` - Convert to uppercase
- `const char* str_to_lower(const char* str)` - Convert to lowercase
- `const char* str_reverse(const char* str)` - Reverse a string
- `int str_count_vowels(const char* str)` - Count vowels in string
- `int str_is_palindrome(const char* str)` - Check if string is palindrome
- `const char* str_repeat(const char* str, int n)` - Repeat string n times

## Key Points

1. **extern "C"**: C++ functions must use `extern "C"` to prevent name mangling
2. **C++ Features**: The library uses C++ STL (std::string, std::algorithm, etc.)
3. **C ABI**: Functions are exported with C linkage for FFI compatibility
4. **String Returns**: Functions returning strings use static buffers (simplified for demo)

## Example Usage in Stratos

```stratos
use ffi;

fn main() {
    val lib: int = ffi.load("./libstringlib.so");

    // Call C++ function that returns int
    val len = ffi.callInt(lib, "str_length", ["string"], ["Hello"]);

    // Check if palindrome
    val is_pal = ffi.callInt(lib, "str_is_palindrome", ["string"], ["racecar"]);

    ffi.unload(lib);
}
```

## Notes

- String return functions exist but need additional handling in FFI
- The library demonstrates using C++ internally while exporting C ABI
- This pattern works for any C++ library that needs Stratos FFI compatibility
