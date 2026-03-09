# Add a Standard Library Module

Create or extend a Stratos standard library module.

## Arguments
- $ARGUMENTS: Module name and optional function specs (e.g., "crypto" or "math --add sqrt cbrt")

## Instructions

1. Parse the module name and any function specifications from $ARGUMENTS.

2. Check if the module already exists in `std/<module>/`:
   - If it exists, read the existing `init.st` to understand what's already implemented
   - If creating a new module, create the directory structure

3. **Standard library module structure:**
```
std/<module>/
└── init.st          # Module entry point with function declarations
```

4. **Stratos function declaration patterns** (read existing modules for reference):

```stratos
package <module>;

/// Documentation comment for the function
/// @param x Description of parameter
/// @returns Description of return value
fn functionName(x: int) int {
    // Implementation in Stratos
    // OR native binding (implemented in C++ NativeRegistry)
}
```

5. For **native functions** (implemented in C++), you also need to:
   - Add the native binding in `src/src/runtime/NativeRegistry.cpp`
   - Register the function with the module name and function name
   - Pattern:
   ```cpp
   registerFunction("<module>", "<functionName>",
       [](const std::vector<std::any>& args) -> std::any {
           // C++ implementation
       }
   );
   ```

6. After creating the module:
   - Show the user the created/modified files
   - If native bindings are needed, create a template in NativeRegistry.cpp
   - Suggest creating an example in `examples/` to test the module
   - Remind them to rebuild with `/build` if C++ changes were made

7. Follow existing module conventions:
   - Read `std/math/init.st`, `std/strings/init.st`, or `std/io/init.st` as reference
   - Use doc comments (`///`) for all public functions
   - Group related functions together
