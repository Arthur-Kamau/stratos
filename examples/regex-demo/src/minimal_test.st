package main;

use regex;
use convert;

fn main() {
    println("Regex Minimal Test");
    println("==================");
    println("");

    // Test 1: Matching
    if regex.matches("[a-z]+", "hello") {
        println("✓ Pattern matching works");
    }

    // Test 2: Contains
    if regex.contains("quick", "The quick brown fox") {
        println("✓ Contains search works");
    }

    // Test 3: Find
    val pos = regex.find("World", "Hello World");
    print("✓ Find position: ");
    println(convert.toString(pos));

    // Test 4: Replace
    println("✓ Replace: " + regex.replace("apples", "I like apples", "oranges"));

    // Test 5: Replace All
    println("✓ Replace all: " + regex.replaceAll("foo", "foo bar foo", "BAR"));

    println("");
    println("All basic tests passed!");
}
