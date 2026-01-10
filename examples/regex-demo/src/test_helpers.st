package main;

use regex;

// Helper function to print a header for a demo section
fn printHeader(title: string) void {
    println("\n" + "=".repeat(50));
    println(title);
    println("=".repeat(50));
    println("");
}

// Helper function to print a sub-section header
fn printSection(title: string) void {
    println("\n--- " + title + " ---");
}

// Helper to test if a pattern matches a text and print the result
fn assertMatch(pattern: string, text: string, description: string) void {
    print("  Checking '" + text + "' (" + description + "): ");
    if regex.matches(pattern, text) {
        println("✓ Match");
    } else {
        println("✗ No Match");
    }
}

// Helper to test if a pattern matches (simple version)
fn testMatch(pattern: string, text: string) void {
    print("  '" + text + "': ");
    if regex.matches(pattern, text) {
        println("✓ Valid");
    } else {
        println("✗ Invalid");
    }
}
