# C FFI Sample - Math Library

This sample demonstrates calling C functions from Stratos using FFI (Foreign Function Interface).

## Building the C Library

```bash
cd samples/ffi/c_math
chmod +x build.sh
./build.sh
```

This will create `libmathlib.so` - a shared library containing math functions.

## Running the Test

From the project root:

```bash
cd samples/ffi/c_math
../../../interpreter/C++/build/stratos run
```

Or specify the entry point explicitly:

```bash
../../../interpreter/C++/build/stratos run src/main.st
```

## Functions in the Library

The C library (`mathlib.c`) exports the following functions:

### Integer Math
- `int add(int a, int b)` - Add two integers
- `int subtract(int a, int b)` - Subtract two integers
- `int multiply(int a, int b)` - Multiply two integers
- `int divide(int a, int b)` - Divide two integers
- `int factorial(int n)` - Calculate factorial
- `int power(int base, int exp)` - Calculate power

### Double Math
- `double add_double(double a, double b)` - Add two doubles

### String Functions
- `void greet(const char* name)` - Print greeting
- `void print_number(const char* label, int number)` - Print labeled number
- `const char* get_version()` - Get library version

## FFI API Usage

### Load a Library
```stratos
val lib = ffi.load("path/to/library.so");
```

### Call Functions

**Integer return:**
```stratos
val result = ffi.callInt(lib, "add", ["int", "int"], [5, 3]);
```

**Double return:**
```stratos
val result = ffi.callDouble(lib, "add_double", ["double", "double"], [3.14, 2.86]);
```

**Void return:**
```stratos
ffi.callVoid(lib, "greet", ["string"], ["Stratos"]);
```

### Unload a Library
```stratos
ffi.unload(lib);
```

## Supported Types

- `int` - 32-bit integer
- `double` - 64-bit floating point
- `string` - C string (const char*)
- `void` - No return value

## Notes

- C functions must use C linkage (not C++ name mangling)
- Maximum 6 parameters supported
- Strings are passed as const char* pointers
- Library IDs are integers returned by ffi.load()
