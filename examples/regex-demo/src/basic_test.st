package main;

use regex;
use convert;

fn main() {
    println("Stratos Regex Module - Basic Test");
    println("==================================");
    println("");

    // Test 1: Pattern matching
    println("Test 1: Pattern Matching");
    println("-------------------------");

    if regex.matches("[a-z]+", "hello") {
        println("✓ 'hello' matches [a-z]+");
    } else {
        println("✗ 'hello' does not match [a-z]+");
    }

    if regex.matches("[a-z]+", "Hello123") {
        println("✓ 'Hello123' matches [a-z]+");
    } else {
        println("✗ 'Hello123' does not match [a-z]+");
    }

    println("");

    // Test 2: Contains
    println("Test 2: Contains Search");
    println("-----------------------");

    if regex.contains("quick", "The quick brown fox") {
        println("✓ Found 'quick' in 'The quick brown fox'");
    } else {
        println("✗ Did not find 'quick'");
    }

    if regex.contains("slow", "The quick brown fox") {
        println("✓ Found 'slow'");
    } else {
        println("✗ Did not find 'slow' (expected)");
    }

    println("");

    // Test 3: Find position
    println("Test 3: Find Position");
    println("---------------------");

    val position = regex.find("World", "Hello World");
    print("Position of 'World' in 'Hello World': ");
    println(convert.toString(position));

    val notFound = regex.find("xyz", "Hello World");
    print("Position of 'xyz' in 'Hello World': ");
    println(convert.toString(notFound));

    println("");

    // Test 4: Replace
    println("Test 4: Text Replacement");
    println("------------------------");

    val replaced = regex.replace("apples", "I like apples", "oranges");
    print("Replace 'apples' with 'oranges': ");
    println(replaced);

    val allReplaced = regex.replaceAll("foo", "foo bar foo baz", "BAR");
    print("Replace all 'foo' with 'BAR': ");
    println(allReplaced);

    println("");

    // Test 5: Escape
    println("Test 5: Escape Special Characters");
    println("----------------------------------");

    val specialText = "Price: $5.99";
    val escapedText = regex.escape(specialText);
    print("Original: ");
    println(specialText);
    print("Escaped:  ");
    println(escapedText);

    println("");

    // Test 6: Validation
    println("Test 6: Common Validations");
    println("---------------------------");

    val emailPattern = "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}";

    if regex.matches(emailPattern, "user@example.com") {
        println("✓ 'user@example.com' is a valid email");
    } else {
        println("✗ Invalid email");
    }

    if regex.matches(emailPattern, "not-an-email") {
        println("✓ 'not-an-email' is valid");
    } else {
        println("✗ 'not-an-email' is invalid (expected)");
    }

    println("");

    // Test 7: Number extraction
    println("Test 7: Pattern Extraction");
    println("---------------------------");

    val groupsResult = regex.groups("([0-9]+)", "The year is 2024");
    print("Extract number from 'The year is 2024': ");
    println(groupsResult);

    println("");
    println("All tests completed!");
}
