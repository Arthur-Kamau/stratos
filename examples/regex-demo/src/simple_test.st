package main;

use regex;

fn main() {
    println("Stratos Regex Module - Simple Test");
    println("===================================");
    println("");

    // Test 1: Pattern compilation
    println("Test 1: Pattern Compilation");
    println("----------------------------");

    val validPattern = "[a-z]+";
    val invalidPattern = "[a-z";

    print("Pattern '");
    print(validPattern);
    print("' is valid: ");
    if regex.compile(validPattern) == 1 {
        println("YES");
    } else {
        println("NO");
    }

    print("Pattern '");
    print(invalidPattern);
    print("' is valid: ");
    if regex.compile(invalidPattern) == 1 {
        println("YES");
    } else {
        println("NO");
    }

    println("");

    // Test 2: Matching
    println("Test 2: Pattern Matching");
    println("-------------------------");

    val text1 = "hello";
    val text2 = "Hello123";
    val pattern = "[a-z]+";

    print("Does '");
    print(text1);
    print("' match '");
    print(pattern);
    print("': ");
    if regex.matches(pattern, text1) {
        println("YES");
    } else {
        println("NO");
    }

    print("Does '");
    print(text2);
    print("' match '");
    print(pattern);
    print("': ");
    if regex.matches(pattern, text2) {
        println("YES");
    } else {
        println("NO");
    }

    println("");

    // Test 3: Contains
    println("Test 3: Contains");
    println("-----------------");

    val haystack = "The quick brown fox";
    val needle = "quick";

    print("Does '");
    print(haystack);
    print("' contain '");
    print(needle);
    print("': ");
    if regex.contains(needle, haystack) {
        println("YES");
    } else {
        println("NO");
    }

    println("");

    // Test 4: Find
    println("Test 4: Find Position");
    println("---------------------");

    val searchText = "Hello World";
    val searchPattern = "World";

    print("Position of '");
    print(searchPattern);
    print("' in '");
    print(searchText);
    print("': ");
    val pos = regex.find(searchPattern, searchText);
    println(convert.toString(pos));

    println("");

    // Test 5: Replace
    println("Test 5: Replace");
    println("----------------");

    val original = "I like apples";
    val replacePattern = "apples";
    val replacement = "oranges";

    print("Original: ");
    println(original);
    print("Replace '");
    print(replacePattern);
    print("' with '");
    print(replacement);
    print("': ");
    val replaced = regex.replace(replacePattern, original, replacement);
    println(replaced);

    println("");

    // Test 6: Replace All
    println("Test 6: Replace All");
    println("--------------------");

    val multiText = "foo bar foo baz foo";
    val multiPattern = "foo";
    val multiReplacement = "BAR";

    print("Original: ");
    println(multiText);
    print("Replace all '");
    print(multiPattern);
    print("' with '");
    print(multiReplacement);
    print("': ");
    val allReplaced = regex.replaceAll(multiPattern, multiText, multiReplacement);
    println(allReplaced);

    println("");

    // Test 7: Escape
    println("Test 7: Escape Special Characters");
    println("-----------------------------------");

    val special = "Price: $5.99";
    print("Original: ");
    println(special);
    print("Escaped: ");
    val escaped = regex.escape(special);
    println(escaped);

    println("");
    println("All tests completed!");
}
