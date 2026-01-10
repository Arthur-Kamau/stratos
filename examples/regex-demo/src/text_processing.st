package main;

use regex;

fn runTextProcessingDemo() {
    printHeader("Regex Text Processing Demo");

    // Example 0: Basic Replace (from replace_demo)
    printSection("Basic Replace Operations");
    val text1 = "I like apples";
    println("Original: " + text1);
    println("Replace:  " + regex.replace("apples", text1, "oranges"));

    val text2 = "foo bar foo baz foo";
    println("\nOriginal: " + text2);
    println("ReplaceAll: " + regex.replaceAll("foo", text2, "BAR"));

    // Example 1: Clean up whitespace
    printSection("Example 1: Clean Whitespace");
    val messyText = "Hello    world   with    lots  of   spaces";
    println("Original: " + messyText);
    println("Cleaned:  " + regex.replaceAll("  +", messyText, " "));

    // Example 2: Remove HTML tags
    printSection("Example 2: Remove HTML Tags");
    val htmlText = "<p>Hello <b>world</b>!</p>";
    println("Original: " + htmlText);
    println("Plain:    " + regex.replaceAll("<[^>]+>", htmlText, ""));

    // Example 3: Censor words
    printSection("Example 3: Censor Words");
    val badText = "This is a bad word example";
    println("Original: " + badText);
    println("Censored: " + regex.replaceAll("bad", badText, "***"));

    // Example 4: Extract data
    printSection("Example 4: Extract Data");
    val dataText = "Contact us at support@example.com or sales@company.org";
    println("Text: " + dataText);

    val matches = regex.matchAll(regex.EMAIL_PATTERN, dataText);
    print("Emails found: ");
    println(matches);

    // Example 5: Find and replace with pattern
    printSection("Example 5: Phone Number Formatting");
    val phoneText = "Call 5551234567 or 5559876543";
    println("Original: " + phoneText);
    println("Formatted: " + regex.replaceAll("([0-9]{3})([0-9]{3})([0-9]{4})", phoneText, "($1) $2-$3"));

    // Example 6: Extract numbers
    printSection("Example 6: Extract Numbers");
    val numberText = "The price is $25.99 and quantity is 42 units";
    println("Text: " + numberText);

    val numbers = regex.matchAll("[0-9]+\\.?[0-9]*", numberText);
    print("Numbers: ");
    println(numbers);

    println("\nText processing demos completed!");
}
