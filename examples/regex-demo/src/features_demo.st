package main;

use regex;
use convert;

fn runFeaturesDemo() {
    printHeader("Regex Features Demo");

    // Feature 1: Pattern Matching
    printSection("1. Pattern Matching");
    if regex.matches("[a-z]+", "hello") {
        println("   ✓ 'hello' matches [a-z]+");
    }
    if !regex.matches("[a-z]+", "Hello123") {
        println("   ✓ 'Hello123' doesn't match [a-z]+");
    }

    // Feature 2: Search (contains)
    printSection("2. Contains Search");
    if regex.contains("world", "Hello world!") {
        println("   ✓ Found 'world' in 'Hello world!'");
    }
    if !regex.contains("xyz", "Hello world!") {
        println("   ✓ 'xyz' not found (as expected)");
    }

    // Feature 3: Find position
    printSection("3. Find Position");
    print("   Position of 'World': ");
    println(convert.toString(regex.find("World", "Hello World")));
    print("   Position of 'xyz': ");
    println(convert.toString(regex.find("xyz", "Hello World")));

    // Feature 4: Replace (single)
    printSection("4. Replace First");
    println("   " + regex.replace("apple", "apple pie and apple juice", "orange"));

    // Feature 5: Replace All
    printSection("5. Replace All");
    println("   " + regex.replaceAll("apple", "apple pie and apple juice", "orange"));

    // Feature 6: Escape special characters
    printSection("6. Escape Special Characters");
    println("   $5.99 escaped: " + regex.escape("$5.99"));

    // Feature 7: Capture groups
    printSection("7. Capture Groups");
    println("   Extract from 'Year: 2024': " + regex.groups("([0-9]+)", "Year: 2024"));

    // Feature 8: Find all positions
    printSection("8. Find All Positions");
    println("   'a' positions in 'banana': " + regex.findAll("a", "banana"));

    // Feature 9: Match all with groups
    printSection("9. Match All");
    println("   Numbers in 'a1b2c3': " + regex.matchAll("[0-9]", "a1b2c3"));

    // Feature 10: Email validation
    printSection("10. Email Validation");
    if regex.matches(regex.EMAIL_PATTERN, "user@example.com") {
        println("    ✓ user@example.com is valid");
    }
    if !regex.matches(regex.EMAIL_PATTERN, "invalid-email") {
        println("    ✓ invalid-email is not valid");
    }

    println("\nAll features demonstrated!");
}
