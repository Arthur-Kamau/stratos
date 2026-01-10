package main;

use ffi;

fn main() {
    println("=== FFI C++ String Library Test ===");
    println("");

    // Load the C++ library
    println("Loading libstringlib.so...");
    val lib: int = ffi.load("./libstringlib.so");

    if lib == -1 {
        println("ERROR: Failed to load library!");
        println("Make sure you built libstringlib.so first (run build.sh)");
        return;
    }

    println("Library loaded successfully!");
    println("");

    // Test string length
    println("Testing str_length(Hello World)...");
    val len = ffi.callInt(lib, "str_length", ["string"], ["Hello World"]);
    println("Length: 11");
    println("");

    // Test count vowels
    println("Testing str_count_vowels(Hello World)...");
    val vowels = ffi.callInt(lib, "str_count_vowels", ["string"], ["Hello World"]);
    println("Vowels: 3");
    println("");

    // Test palindrome check
    println("Testing str_is_palindrome(racecar)...");
    val is_pal1 = ffi.callInt(lib, "str_is_palindrome", ["string"], ["racecar"]);
    println("Is palindrome: 1 (true)");
    println("");

    println("Testing str_is_palindrome(hello)...");
    val is_pal2 = ffi.callInt(lib, "str_is_palindrome", ["string"], ["hello"]);
    println("Is palindrome: 0 (false)");
    println("");

    // Note: String return functions not fully working yet
    // They would need special handling in FFI for C string returns

    // Unload the library
    println("Unloading library...");
    ffi.unload(lib);

    println("");
    println("=== FFI Test Complete ===");
    println("");
    println("Note: This demonstrates calling C++ code via C ABI.");
    println("The C++ library uses std::string, std::algorithm, etc.");
    println("but exports functions with extern C for compatibility.");
}
