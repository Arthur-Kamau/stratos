package main;

use regex;
use convert;

fn runSimpleTest() {
    printHeader("Stratos Regex Module - Simple Test");

    // Test 1: Pattern compilation
    printSection("Test 1: Pattern Compilation");

    val validPattern = "[a-z]+";
    val invalidPattern = "[a-z";

    print("Pattern '" + validPattern + "' is valid: ");
    if regex.compile(validPattern) == 1 { println("YES"); } else { println("NO"); }

    print("Pattern '" + invalidPattern + "' is valid: ");
    if regex.compile(invalidPattern) == 1 { println("YES"); } else { println("NO"); }

    // Test 2: Matching
    printSection("Test 2: Pattern Matching");
    val pattern = "[a-z]+";
    assertMatch(pattern, "hello", "should match");
    assertMatch(pattern, "Hello123", "should not match");

    // Test 3: Contains
    printSection("Test 3: Contains");
    val haystack = "The quick brown fox";
    val needle = "quick";
    print("Does '" + haystack + "' contain '" + needle + "': ");
    if regex.contains(needle, haystack) { println("YES"); } else { println("NO"); }

    // Test 4: Find
    printSection("Test 4: Find Position");
    val searchText = "Hello World";
    val searchPattern = "World";
    print("Position of '" + searchPattern + "' in '" + searchText + "': ");
    println(convert.toString(regex.find(searchPattern, searchText)));

    // Test 5: Replace
    printSection("Test 5: Replace");
    val original = "I like apples";
    println("Original: " + original);
    println("Replace:  " + regex.replace("apples", original, "oranges"));

    // Test 6: Replace All
    printSection("Test 6: Replace All");
    val multiText = "foo bar foo baz foo";
    println("Original: " + multiText);
    println("ReplaceAll: " + regex.replaceAll("foo", multiText, "BAR"));

    // Test 7: Escape
    printSection("Test 7: Escape Special Characters");
    val special = "Price: $5.99";
    println("Original: " + special);
    println("Escaped:  " + regex.escape(special));

    // Test 8: Groups (Extraction)
    printSection("Test 8: Groups Extraction");
    val extractText = "The year is 2024";
    println("Text: " + extractText);
    println("Extracted: " + regex.groups("([0-9]+)", extractText));

    println("\nAll tests completed!");
}
