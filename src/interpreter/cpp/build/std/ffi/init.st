// FFI (Foreign Function Interface) module - native bindings
package ffi;

// Load a shared library and return a library ID
// Returns: library ID (int) or -1 on failure
// Example: val lib = ffi.load("./libmylib.so");
fn load(path: string) int;

// Unload a previously loaded library
// Example: ffi.unload(lib);
fn unload(libraryId: int) void;

// Call a foreign function (generic version)
// Parameters: libraryId, functionName, returnType, paramTypes, args
// Example: val result = ffi.call(lib, "add", "int", ["int", "int"], [5, 3]);
fn call(libraryId: int, functionName: string, returnType: string, paramTypes: array, args: array) int;

// Call a foreign function that returns void
// Example: ffi.callVoid(lib, "greet", ["string"], ["World"]);
fn callVoid(libraryId: int, functionName: string, paramTypes: array, args: array) void;

// Call a foreign function that returns int
// Example: val result = ffi.callInt(lib, "add", ["int", "int"], [5, 3]);
fn callInt(libraryId: int, functionName: string, paramTypes: array, args: array) int;

// Call a foreign function that returns double
// Example: val result = ffi.callDouble(lib, "sqrt", ["double"], [9.0]);
fn callDouble(libraryId: int, functionName: string, paramTypes: array, args: array) double;
