# Codebase Audit Report: Stratos Interpreter

## Executive Summary
This audit of the Stratos C++ codebase (`src/src` and `src/include`) identifies critical memory leaks and stability risks. The most severe issue is a fundamental design flaw in the `Interpreter`'s scope management which causes unbounded memory growth during execution. Additionally, the FFI (Native Registry) implementation poses significant risks for segmentation faults and resource leaks.

## 1. Critical Memory Leaks

### 1.1 Unbounded `Environment` Growth (Severity: Critical)
**Location:** `src/runtime/Interpreter.cpp`
**Issue:**
The `Interpreter` class maintains a `std::vector<std::unique_ptr<Environment>> environments` that acts as a storage arena for all scopes created during execution.
```cpp
void Interpreter::enterScope() {
    auto newEnv = std::make_unique<Environment>();
    // ...
    environments.push_back(std::move(newEnv)); 
    // Adds to vector, NEVER removes
}
```
`exitScope` merely updates the `currentEnv` pointer but does **not** remove the `Environment` from the `environments` vector.
**Impact:**
Every block entry, function call, and loop iteration allocates a new `Environment` object that persists until the interpreter shuts down. A long-running script (e.g., a server) will inevitably consume all available RAM and crash, even if it runs a garbage collector, because the `Interpreter` itself holds strong references to these environments.

### 1.2 Native Resource Leaks (Severity: High)
**Location:** `src/runtime/NativeRegistry.cpp`
**Issue:**
Wrappers for C libraries (MySQL, Redis, PostgreSQL) rely entirely on manual disposal via script-level `close()` methods (e.g., `__mysql_close`, `__redis_close`).
**Impact:**
If a Stratos script encounters an error or simply fails to call `close()`, the underlying C handles (`MYSQL*`, `redisContext*`, `PGconn*`) are leaked. There is no automated cleanup (finalizers) associated with these handles.

## 2. Potential Failure Points & Stability Risks

### 2.1 Segmentation Faults via Invalid Handles (Severity: Critical)
**Location:** `src/runtime/NativeRegistry.cpp`
**Issue:**
Native functions use `reinterpret_cast<void*>` to retrieve C pointers from `std::any` without validation.
```cpp
auto handle = std::any_cast<void*>(args[0]);
MYSQL* conn = reinterpret_cast<MYSQL*>(handle); // Blind cast
```
**Impact:**
A script causing a "use-after-free" (calling a method on a closed handle) or passing an invalid argument will cause the interpreter process to segfault immediately, terminating the entire application (including the web server if one is running).

### 2.2 Unhandled Exceptions in Helper Threads
**Location:** `src/devtools/DevToolsServer.cpp`, etc.
**Issue:**
Threads spawned for auxiliary tasks (like DevTools or potential async workers in `Interpreter.cpp`) may not have comprehensive top-level `try-catch` blocks.
**Impact:**
An uncaught exception in a detached thread will call `std::terminate()`, crashing the main interpreter process unexpectedly.

## 3. Recommendations

1.  **Fix Environment Management**:
    *   Change `Interpreter` to use `std::shared_ptr<Environment>` for `currentEnv`.
    *   Remove the global `environments` vector.
    *   Allow environments to be destroyed naturally when their reference count drops to zero (i.e., when they are exited and not captured by any closure).

2.  **Robust Native Handles**:
    *   Implement a "Handle Table" or "Resource Manager" that maps integer IDs to validated pointers, preventing invalid pointer access.
    *   Implement a mechanism to auto-close handles when the associated Stratos object is garbage collected.

3.  **Memory Management**:
    *   The current Garbage Collector seems to trace `RuntimeValue` roots but cannot clean up `Environment` objects rooted in the `interpreter.environments` vector. This must be addressed alongside recommendation #1.
