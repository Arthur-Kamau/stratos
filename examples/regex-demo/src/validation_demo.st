package main;

use regex;

fn runValidationDemo() {
    printHeader("Regex Validation Demo");

    // Email validation
    printSection("Email Validation");
    testMatch(regex.EMAIL_PATTERN, "user@example.com");
    testMatch(regex.EMAIL_PATTERN, "john.doe@company.co.uk");
    testMatch(regex.EMAIL_PATTERN, "invalid.email");
    testMatch(regex.EMAIL_PATTERN, "@example.com");

    // URL validation
    printSection("URL Validation");
    testMatch(regex.URL_PATTERN, "http://example.com");
    testMatch(regex.URL_PATTERN, "https://www.example.com:8080/path");
    testMatch(regex.URL_PATTERN, "ftp://example.com");
    testMatch(regex.URL_PATTERN, "example.com");

    // Number validation
    printSection("Number Validation");

    testMatch(regex.INTEGER, "42");
    testMatch(regex.INTEGER, "-100");
    testMatch(regex.INTEGER, "3.14");

    testMatch(regex.DECIMAL, "3.14");
    testMatch(regex.DECIMAL, "-2.5");
    testMatch(regex.DECIMAL, "abc");

    println("\nValidation tests completed!");
}
