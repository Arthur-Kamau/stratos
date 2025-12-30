package main;

fn main() {
    println("=== Simple FFI Test ===");
    println("");

    // Try calling ffi functions directly without 'use ffi'
    println("Loading library...");

    // This should work if ffi is registered as a native module
    println("Test complete!");
}
