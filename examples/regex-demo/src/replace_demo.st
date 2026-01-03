package main;

use regex;

fn main() {
    println("Regex Replace Demo");
    println("==================");
    println("");

    // Example 1: Simple replace
    println("Example 1: Simple Replace");
    val text1 = "I like apples";
    println("Original: " + text1);
    println("Replace:  " + regex.replace("apples", text1, "oranges"));

    println("");

    // Example 2: Replace all
    println("Example 2: Replace All");
    val text2 = "foo bar foo baz foo";
    println("Original: " + text2);
    println("Replace:  " + regex.replaceAll("foo", text2, "BAR"));

    println("");

    // Example 3: Clean whitespace
    println("Example 3: Clean Whitespace");
    val text3 = "Too    many     spaces";
    println("Original: " + text3);
    println("Cleaned:  " + regex.replaceAll("  +", text3, " "));

    println("");

    // Example 4: Remove HTML tags
    println("Example 4: Remove HTML");
    val text4 = "<p>Hello <b>world</b>!</p>";
    println("Original: " + text4);
    println("Plain:    " + regex.replaceAll("<[^>]+>", text4, ""));

    println("");

    // Example 5: Censor words
    println("Example 5: Word Censoring");
    val text5 = "This bad text has bad words";
    println("Original:  " + text5);
    println("Censored:  " + regex.replaceAll("bad", text5, "***"));

    println("");
    println("Replace demos completed!");
}
