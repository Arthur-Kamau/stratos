# Stratos Prelude - Auto-Imported Functions

Stratos implements a **prelude system** similar to Rust, where common functions are automatically imported without requiring an explicit `use` statement. This makes the language more ergonomic while maintaining a clean standard library design.

## Available Prelude Functions

### `println(value)` - Print with Newline

Prints a value followed by a newline character.

```stratos
println("Hello, World!");  // Output: Hello, World!\n
println(42);               // Output: 42\n
println(3.14);             // Output: 3.14\n
println(true);             // Output: true\n
```

### `print(value)` - Print without Newline

Prints a value without adding a newline. Useful for building output on the same line.

```stratos
print("Hello, ");
print("World!");
println("");  // Add newline
// Output: Hello, World!\n
```

### `printf(format, ...args)` - Formatted Printing

Prints a formatted string with `{}` placeholders replaced by arguments.

```stratos
printf("Name: {}, Age: {}\n", "Alice", 25);
// Output: Name: Alice, Age: 25\n

val x = 10;
val y = 20;
printf("x + y = {}\n", x + y);
// Output: x + y = 30\n
```

#### Supported Escape Sequences in printf:
- `\n` - Newline
- `\t` - Tab
- `\r` - Carriage return
- `\\` - Backslash

#### Supported Types:
- `int`
- `double`
- `string`
- `bool`
- `char`

## Design Rationale

### Why Prelude?

Stratos is designed as a **systems programming language**, and following Rust's approach:

1. **Clean core language**: I/O is not a language primitive
2. **Standard library consistency**: All I/O goes through library functions
3. **Explicit imports for modules**: Only common functions are auto-imported
4. **No magic**: The prelude is just a module that's imported automatically

### What's in the Prelude?

The prelude contains only the most fundamental functions that nearly every program needs:
- **I/O functions**: `print`, `println`, `printf`
- Future additions may include basic panic/error handling

### What's NOT in the Prelude?

Everything else requires explicit import:
- Math functions: `use math;`
- File I/O: `use io;`
- Logging: `use log;`
- Collections, networking, etc.

## Migration from Old `print` Statement

If you have code using the old `print` statement:

**Before:**
```stratos
print("Hello");  // Old built-in statement
```

**After:**
```stratos
println("Hello");  // Prelude function with newline
// OR
print("Hello");    // Prelude function without newline
```

The key difference:
- Old `print` statement: Always added newline
- New `print()` function: No newline (use `println()` for old behavior)

## Implementation Details

The prelude functions are:
1. Registered in `NativeRegistry::initPrelude()`
2. Automatically resolved by `SemanticAnalyzer` without requiring `use prelude;`
3. Implemented in C++ for maximum performance
4. Available in all Stratos programs without any imports

## Examples

### Basic Output
```stratos
fn main() {
    println("=== Stratos Prelude Demo ===");
    println("");

    val name = "Bob";
    val age = 25;

    printf("Hello, {}! You are {} years old.\n", name, age);
}
```

### Building Complex Output
```stratos
fn main() {
    print("[");
    print("INFO");
    print("] ");
    println("Application started");

    // Output: [INFO] Application started
}
```

### Debugging
```stratos
fn main() {
    val x = 42;
    printf("Debug: x = {}\n", x);

    val result = x * 2;
    printf("Debug: result = {}\n", result);
}
```

## Performance

Prelude functions are implemented as native C++ functions, providing:
- Zero overhead compared to C++ I/O
- No runtime penalty for the auto-import mechanism
- Type dispatch happens at the native layer for maximum efficiency

## Future Prelude Additions

Potential future additions to the prelude (community discussion needed):
- `panic(message)` - Unrecoverable error
- `assert(condition, message)` - Runtime assertion
- `dbg(value)` - Debug print with source location

---

For more information about the Stratos standard library, see the full stdlib documentation.
