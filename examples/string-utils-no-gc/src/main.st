package main;

use strings;

// String utilities example demonstrating Stratos without GC
// This project has garbage collection disabled in stratos.conf

fn printSeparator() {
    println("----------------------------------------");
}

fn demonstrateBasicOperations() {
    println("=== Basic String Operations ===");
    println("");

    val greeting = "Hello, World!";
    println("Original: '${greeting}'");
    println("Length: ${strings.len(greeting)}");
    println("Uppercase: '${strings.toUpper(greeting)}'");
    println("Lowercase: '${strings.toLower(greeting)}'");

    printSeparator();
}

fn demonstrateTrimming() {
    println("=== Trimming Operations ===");
    println("");

    val padded = "   Hello, Stratos!   ";
    println("Original: '${padded}'");
    println("Trimmed: '${strings.trim(padded)}'");
    println("Trim Left: '${strings.trimLeft(padded)}'");
    println("Trim Right: '${strings.trimRight(padded)}'");

    printSeparator();
}

fn demonstrateSearching() {
    println("=== Search Operations ===");
    println("");

    val text = "The quick brown fox jumps over the lazy dog";
    println("Text: '${text}'");

    println("Contains 'fox': ${strings.contains(text, "fox")}");
    println("Contains 'cat': ${strings.contains(text, "cat")}");
    println("Starts with 'The': ${strings.startsWith(text, "The")}");
    println("Ends with 'dog': ${strings.endsWith(text, "dog")}");
    println("Index of 'brown': ${strings.indexOf(text, "brown")}");
    println("Index of 'cat': ${strings.indexOf(text, "cat")}");

    printSeparator();
}

fn demonstrateSubstrings() {
    println("=== Substring Operations ===");
    println("");

    val text = "Hello, Stratos Programming!";
    println("Original: '${text}'");
    println("Substring(0, 5): '${strings.substring(text, 0, 5)}'");
    println("Substring(7, 14): '${strings.substring(text, 7, 14)}'");

    printSeparator();
}

fn demonstrateReplace() {
    println("=== Replace Operations ===");
    println("");

    val original = "foo bar foo baz foo";
    println("Original: '${original}'");
    println("Replace 'foo' with 'qux': '${strings.replace(original, "foo", "qux")}'");

    val sentence = "I like apples and apples are healthy";
    println("Original: '${sentence}'");
    println("Replace 'apples' with 'oranges': '${strings.replace(sentence, "apples", "oranges")}'");

    printSeparator();
}

fn demonstrateSplitJoin() {
    println("=== Split and Join Operations ===");
    println("");

    val csv = "apple,banana,cherry,date";
    println("CSV: '${csv}'");

    val fruits = strings.split(csv, ",");
    println("Split by ',': ${fruits}");

    val joined = strings.join(fruits, " | ");
    println("Joined with ' | ': '${joined}'");

    val words = "Hello World Stratos";
    val wordList = strings.split(words, " ");
    println("Words: ${wordList}");

    printSeparator();
}

fn demonstrateRepeat() {
    println("=== Repeat Operations ===");
    println("");

    val star = "*";
    println("'${star}' repeated 10 times: '${strings.repeat(star, 10)}'");

    val pattern = "ab";
    println("'${pattern}' repeated 5 times: '${strings.repeat(pattern, 5)}'");

    printSeparator();
}

fn demonstratePadding() {
    println("=== Padding Operations ===");
    println("");

    val num = "42";
    println("Original: '${num}'");
    println("Pad left to 6 with '0': '${strings.padLeft(num, 6, "0")}'");
    println("Pad right to 6 with ' ': '${strings.padRight(num, 6, " ")}'|");

    val text = "Hi";
    println("'${text}' padded left to 10: '${strings.padLeft(text, 10, "-")}'");
    println("'${text}' padded right to 10: '${strings.padRight(text, 10, "-")}'");

    printSeparator();
}

fn demonstrateCharOperations() {
    println("=== Character Operations ===");
    println("");

    val text = "Hello";
    println("Text: '${text}'");

    for i in 0..strings.len(text) {
        val char = strings.charAt(text, i);
        println("Character at ${i}: '${char}'");
    }

    printSeparator();
}

fn main() {
    println("===========================================");
    println("   String Utilities Demo (No GC)");
    println("===========================================");
    println("");

    demonstrateBasicOperations();
    demonstrateTrimming();
    demonstrateSearching();
    demonstrateSubstrings();
    demonstrateReplace();
    demonstrateSplitJoin();
    demonstrateRepeat();
    demonstratePadding();
    demonstrateCharOperations();

    println("===========================================");
    println("   Demo Complete!");
    println("===========================================");
}
