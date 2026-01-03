package main;

use regex;

fn main() {
    println("Regex Validation Demo");
    println("=====================");
    println("");

    // Email validation
    println("Email Validation:");
    println("-----------------");

    val emailPattern = "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}";

    testEmail(emailPattern, "user@example.com");
    testEmail(emailPattern, "john.doe@company.co.uk");
    testEmail(emailPattern, "invalid.email");
    testEmail(emailPattern, "@example.com");

    println("");

    // URL validation
    println("URL Validation:");
    println("---------------");

    val urlPattern = "https?://[a-zA-Z0-9.-]+(:[0-9]+)?(/.*)?";

    testURL(urlPattern, "http://example.com");
    testURL(urlPattern, "https://www.example.com:8080/path");
    testURL(urlPattern, "ftp://example.com");
    testURL(urlPattern, "example.com");

    println("");

    // Number validation
    println("Number Validation:");
    println("------------------");

    val intPattern = "^-?[0-9]+$";
    val floatPattern = "^-?[0-9]+(\\.[0-9]+)?$";

    testNumber(intPattern, "Integer", "42");
    testNumber(intPattern, "Integer", "-100");
    testNumber(intPattern, "Integer", "3.14");

    testNumber(floatPattern, "Float", "3.14");
    testNumber(floatPattern, "Float", "-2.5");
    testNumber(floatPattern, "Float", "abc");

    println("");
    println("Validation tests completed!");
}

fn testEmail(pattern: string, email: string) void {
    print("  ");
    print(email);
    print(": ");

    if regex.matches(pattern, email) {
        println("✓ Valid");
    } else {
        println("✗ Invalid");
    }
}

fn testURL(pattern: string, url: string) void {
    print("  ");
    print(url);
    print(": ");

    if regex.matches(pattern, url) {
        println("✓ Valid");
    } else {
        println("✗ Invalid");
    }
}

fn testNumber(pattern: string, numType: string, value: string) void {
    print("  ");
    print(value);
    print(" (");
    print(numType);
    print("): ");

    if regex.matches(pattern, value) {
        println("✓ Valid");
    } else {
        println("✗ Invalid");
    }
}
