package main;

use regex;
use convert;

fn main() {
    println("Regex Features Demo");
    println("===================");
    println("");

    // Feature 1: Pattern Matching
    println("1. Pattern Matching");
    if regex.matches("[a-z]+", "hello") {
        println("   ✓ 'hello' matches [a-z]+");
    }
    if !regex.matches("[a-z]+", "Hello123") {
        println("   ✓ 'Hello123' doesn't match [a-z]+");
    }
    println("");

    // Feature 2: Search (contains)
    println("2. Contains Search");
    if regex.contains("world", "Hello world!") {
        println("   ✓ Found 'world' in 'Hello world!'");
    }
    if !regex.contains("xyz", "Hello world!") {
        println("   ✓ 'xyz' not found (as expected)");
    }
    println("");

    // Feature 3: Find position
    println("3. Find Position");
    print("   Position of 'World': ");
    println(convert.toString(regex.find("World", "Hello World")));
    print("   Position of 'xyz': ");
    println(convert.toString(regex.find("xyz", "Hello World")));
    println("");

    // Feature 4: Replace (single)
    println("4. Replace First");
    println("   " + regex.replace("apple", "apple pie and apple juice", "orange"));
    println("");

    // Feature 5: Replace All
    println("5. Replace All");
    println("   " + regex.replaceAll("apple", "apple pie and apple juice", "orange"));
    println("");

    // Feature 6: Escape special characters
    println("6. Escape Special Characters");
    println("   $5.99 escaped: " + regex.escape("$5.99"));
    println("");

    // Feature 7: Capture groups
    println("7. Capture Groups");
    println("   Extract from 'Year: 2024': " + regex.groups("([0-9]+)", "Year: 2024"));
    println("");

    // Feature 8: Find all positions
    println("8. Find All Positions");
    println("   'a' positions in 'banana': " + regex.findAll("a", "banana"));
    println("");

    // Feature 9: Match all with groups
    println("9. Match All");
    println("   Numbers in 'a1b2c3': " + regex.matchAll("[0-9]", "a1b2c3"));
    println("");

    // Feature 10: Email validation
    println("10. Email Validation");
    val emailPattern = "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}";
    if regex.matches(emailPattern, "user@example.com") {
        println("    ✓ user@example.com is valid");
    }
    if !regex.matches(emailPattern, "invalid-email") {
        println("    ✓ invalid-email is not valid");
    }
    println("");

    println("All features demonstrated!");
}
