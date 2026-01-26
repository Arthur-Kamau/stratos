# HTTP Module Method Resolution Fix

## Problem

The HTTP server example (`./examples/stdlib-examples/04_http_server/`) fails semantic analysis with errors like:

```
[Error] init.st:139:12: Undefined variable 'get'.
[Error] init.st:148:23: Undefined variable 'get'.
[Error] main.st:22:12: Undefined variable 'get'.
```

Method calls like `router.get("*", handler)` are being treated as if `get` is a standalone variable rather than a method on the `router` object.

## Root Cause Analysis

The semantic analyzer has code to handle method calls on objects (lines 711-753 in `SemanticAnalyzer.cpp`), but it's not working for the HTTP module.

### How Method Calls Should Work

For `router.get("*", handler)`:

1. Parser creates a `CallExpr` where callee is a `BinaryExpr` with `DOT` operator
2. Left side: `VariableExpr("router")`
3. Right side: `VariableExpr("get")`

The semantic analyzer should:
1. Resolve `router` in the symbol table as a `VARIABLE`
2. Get its type: `Router` (inferred from `val router = newRouter();`)
3. Check if `Router` exists in `classMembers`
4. Check if `get` exists in `classMembers["Router"]`
5. If yes, allow the call

### Suspected Issues

1. **Type Inference**: When `val router = newRouter();` is processed, `inferType` needs to return `"Router"`. This requires `newRouter` to be registered in the symbol table with return type `Router`.

2. **Class Member Registration**: When the HTTP module is loaded, the `Router` class and its methods (`get`, `post`, etc.) need to be registered in `classMembers`.

3. **Module Loading Order**: The HTTP module has dependencies (`use time; use log; use io; use json; use concurrent;`). These nested imports might be interfering with the registration process.

## Files Involved

- `src/sema/SemanticAnalyzer.cpp`:
  - `loadModule()` (lines 1128-1432): Parses and registers module symbols
  - `visit(CallExpr&)` (lines 621-782): Handles method call validation
  - `inferType()` (lines 1434+): Infers expression types

- `std/net/http/init.st`:
  - Contains `Router` class (lines 58-69) with methods like `get`, `post`
  - Contains `newRouter()` function (line 109) returning `Router`
  - Contains `serve()` function (lines 137-143) that uses `router.get()`

## Current Status

**Added debug output** to the method call handling code (lines 711-753) to trace:
- Whether `router` resolves in symbol table
- What type it has
- Whether `Router` is in `classMembers`
- Whether the method exists

**Next Steps**:
1. Build and run with debug output to identify which condition is failing
2. Fix the identified issue (likely in `loadModule` or `inferType`)
3. Remove debug output once fixed

## Related Working Example

`examples/method_test3/` works correctly - it has a similar structure with a local `myhttp` module containing a `Router` class. The difference is:
- `method_test3` uses a module in `src/myhttp/` (project-local)
- HTTP module is in `std/net/http/` (standard library path)

This suggests the issue may be related to how standard library modules are loaded vs. project-local modules.

## Test Commands

```bash
# Build
cd src/interpreter/cpp/build && ninja

# Run HTTP example (failing)
./stratos run ./examples/stdlib-examples/04_http_server/

# Run method_test3 (working)
./stratos run ./examples/method_test3/
```
