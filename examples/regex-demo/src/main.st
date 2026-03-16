package main;

use regex;

fn main() {
    println("Stratos Regex Module - Demo");
    println("===========================");

    // 1. Basic matching
    println("\n1. Basic Pattern Matching");
    println("-------------------------");
    println("  Match '[0-9]+' on '123': " + regex.matches("[0-9]+", "123"));
    println("  Match '[0-9]+' on 'abc': " + regex.matches("[0-9]+", "abc"));
    println("  Match '[a-z]+' on 'hello': " + regex.matches("[a-z]+", "hello"));

    // 2. Email validation
    println("\n2. Email Validation");
    println("-------------------");
    val emailPattern = "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}";
    println("  'user@example.com': " + regex.matches(emailPattern, "user@example.com"));
    println("  'invalid-email': " + regex.matches(emailPattern, "invalid-email"));
    println("  'test@test.org': " + regex.matches(emailPattern, "test@test.org"));

    // 3. Number validation
    println("\n3. Number Validation");
    println("--------------------");
    val numberPattern = "^-?[0-9]+(\\.[0-9]+)?$";
    println("  '42': " + regex.matches(numberPattern, "42"));
    println("  '3.14': " + regex.matches(numberPattern, "3.14"));
    println("  '-7': " + regex.matches(numberPattern, "-7"));
    println("  'abc': " + regex.matches(numberPattern, "abc"));

    // 4. Replace
    println("\n4. String Replace");
    println("-----------------");
    val replaced = regex.replace("[0-9]+", "abc123def456", "NUM");
    println("  Replace digits: " + replaced);

    // 5. Split
    println("\n5. String Split");
    println("---------------");
    val parts = regex.split(",", "apple,banana,cherry");
    println("  Split CSV: " + parts);

    // 6. Find
    println("\n6. Find Position");
    println("----------------");
    val pos = regex.find("[0-9]+", "hello123world");
    println("  First digit position in 'hello123world': " + pos);

    println("\n===========================");
    println("Regex Demo Complete!");
    println("===========================");
}
