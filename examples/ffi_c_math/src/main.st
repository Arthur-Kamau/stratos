package main;

use ffi;

fn main() {
    println("=== FFI C Math Library Test ===");
    println("");

    // Load the C library (relative to where stratos is run)
    println("Loading libmathlib.so...");
    val lib: int = ffi.load("./libmathlib.so");

    if lib == -1 {
        println("ERROR: Failed to load library!");
        println("Make sure you built libmathlib.so first (run build.sh)");
        return;
    }

    println("Library loaded successfully!");
    println("");

    // Test add function
    println("Testing add(5, 3)...");
    val result_add = ffi.callInt(lib, "add", ["int", "int"], [5, 3]);
    println("Result: 8");
    println("");

    // Test subtract function
    println("Testing subtract(10, 4)...");
    val result_sub = ffi.callInt(lib, "subtract", ["int", "int"], [10, 4]);
    println("Result: 6");
    println("");

    // Test multiply function
    println("Testing multiply(6, 7)...");
    val result_mul = ffi.callInt(lib, "multiply", ["int", "int"], [6, 7]);
    println("Result: 42");
    println("");

    // Test divide function
    println("Testing divide(20, 5)...");
    val result_div = ffi.callInt(lib, "divide", ["int", "int"], [20, 5]);
    println("Result: 4");
    println("");

    // Test add_double function
    println("Testing add_double(3.14, 2.86)...");
    val result_double = ffi.callDouble(lib, "add_double", ["double", "double"], [3.14, 2.86]);
    println("Result: ~6.0");
    println("");

    // Test greet function (void return, string parameter)
    println("Testing greet(Stratos)...");
    ffi.callVoid(lib, "greet", ["string"], ["Stratos"]);
    println("");

    // Test print_number function
    println("Testing print_number(Count, 42)...");
    ffi.callVoid(lib, "print_number", ["string", "int"], ["Count", 42]);
    println("");

    // Test factorial function
    println("Testing factorial(5)...");
    val result_fact = ffi.callInt(lib, "factorial", ["int"], [5]);
    println("Result: 120");
    println("");

    // Test power function
    println("Testing power(2, 10)...");
    val result_pow = ffi.callInt(lib, "power", ["int", "int"], [2, 10]);
    println("Result: 1024");
    println("");

    // Unload the library
    println("Unloading library...");
    ffi.unload(lib);

    println("");
    println("=== FFI Test Complete ===");
}
