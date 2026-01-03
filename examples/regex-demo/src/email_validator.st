package main;

use regex;

fn main() {
    println("Email Validator Demo");
    println("====================");
    println("");

    val emailPattern = "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}";

    // Test valid emails
    println("Valid Emails:");
    validateEmail(emailPattern, "user@example.com");
    validateEmail(emailPattern, "john.doe@company.co.uk");
    validateEmail(emailPattern, "test.email+tag@domain.org");

    println("");

    // Test invalid emails
    println("Invalid Emails:");
    validateEmail(emailPattern, "invalid.email");
    validateEmail(emailPattern, "@example.com");
    validateEmail(emailPattern, "user@");
    validateEmail(emailPattern, "no-at-sign.com");

    println("");
    println("Validation complete!");
}

fn validateEmail(pattern: string, email: string) void {
    print("  ");
    print(email);
    print(": ");

    if regex.matches(pattern, email) {
        println("✓ Valid");
    } else {
        println("✗ Invalid");
    }
}
