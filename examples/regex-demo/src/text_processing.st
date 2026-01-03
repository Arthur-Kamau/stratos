package main;

use regex;

fn main() {
    println("Regex Text Processing Demo");
    println("===========================");
    println("");

    // Example 1: Clean up whitespace
    println("Example 1: Clean Whitespace");
    println("----------------------------");

    val messyText = "Hello    world   with    lots  of   spaces";
    println("Original: " + messyText);

    val cleanText = regex.replaceAll("  +", messyText, " ");
    println("Cleaned:  " + cleanText);

    println("");

    // Example 2: Remove HTML tags
    println("Example 2: Remove HTML Tags");
    println("----------------------------");

    val htmlText = "<p>Hello <b>world</b>!</p>";
    println("Original: " + htmlText);

    val plainText = regex.replaceAll("<[^>]+>", htmlText, "");
    println("Plain:    " + plainText);

    println("");

    // Example 3: Censor words
    println("Example 3: Censor Words");
    println("------------------------");

    val badText = "This is a bad word example";
    println("Original: " + badText);

    val censoredText = regex.replaceAll("bad", badText, "***");
    println("Censored: " + censoredText);

    println("");

    // Example 4: Extract data
    println("Example 4: Extract Data");
    println("-----------------------");

    val dataText = "Contact us at support@example.com or sales@company.org";
    println("Text: " + dataText);

    val emailPattern = "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}";
    val matches = regex.matchAll(emailPattern, dataText);
    print("Emails found: ");
    println(matches);

    println("");

    // Example 5: Find and replace with pattern
    println("Example 5: Phone Number Formatting");
    println("-----------------------------------");

    val phoneText = "Call 5551234567 or 5559876543";
    println("Original: " + phoneText);

    val formattedPhone = regex.replaceAll("([0-9]{3})([0-9]{3})([0-9]{4})", phoneText, "($1) $2-$3");
    println("Formatted: " + formattedPhone);

    println("");

    // Example 6: Extract numbers
    println("Example 6: Extract Numbers");
    println("--------------------------");

    val numberText = "The price is $25.99 and quantity is 42 units";
    println("Text: " + numberText);

    val numbers = regex.matchAll("[0-9]+\\.?[0-9]*", numberText);
    print("Numbers: ");
    println(numbers);

    println("");
    println("Text processing demos completed!");
}
