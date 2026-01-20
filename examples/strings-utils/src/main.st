// Example: String Manipulation
// Demonstrates the strings module

package main;

use strings;

fn main() {
    println("=== String Manipulation Examples ===");
    println("");

    // Basic string operations
    println("--- Basic Operations ---");
    val text = "Hello, World!";
    println("Original: " + text);
    println("Length: " + strings.length(text));
    println("Is empty: " + strings.isEmpty(text));
    println("Contains 'World': " + strings.contains(text, "World"));
    println("Starts with 'Hello': " + strings.startsWith(text, "Hello"));
    println("Ends with '!': " + strings.endsWith(text, "!"));
    println("Index of 'World': " + strings.indexOf(text, "World"));
    println("");

    // Substring and charAt
    println("--- Substring Operations ---");
    println("Substring (0, 5): " + strings.substring(text, 0, 5));
    println("Substring (7, 12): " + strings.substring(text, 7, 12));
    println("Char at index 0: " + strings.charAt(text, 0));
    println("Char at index 7: " + strings.charAt(text, 7));
    println("");

    // Case conversion
    println("--- Case Conversion ---");
    val mixed = "Hello World";
    println("Original: " + mixed);
    println("Upper: " + strings.toUpper(mixed));
    println("Lower: " + strings.toLower(mixed));
    println("Capitalize: " + strings.capitalize(mixed));
    println("Uncapitalize: " + strings.uncapitalize(mixed));
    println("");

    // Case transformations
    println("--- Case Transformations ---");
    val phrase = "hello world example";
    println("Original: " + phrase);
    println("camelCase: " + strings.toCamelCase(phrase));
    println("PascalCase: " + strings.toPascalCase(phrase));
    println("snake_case: " + strings.toSnakeCase(phrase));
    println("kebab-case: " + strings.toKebabCase(phrase));
    println("");

    // Trimming and whitespace
    println("--- Trimming ---");
    val padded = "  Hello World  ";
    println("Original: '" + padded + "'");
    println("Trimmed: '" + strings.trim(padded) + "'");
    println("");

    // String splitting
    println("--- Splitting ---");
    val csv = "apple,banana,orange";
    println("Original: " + csv);
    val fruits = strings.split(csv, ",");
    println("Split by ',': ");
    for val i in 0..fruits.length() {
        println("  [" + i + "] " + fruits[i]);
    }
    println("");

    // String replacement
    println("--- Replacement ---");
    val original = "Hello World";
    println("Original: " + original);
    println("Replace 'World' with 'Stratos': " + strings.replace(original, "World", "Stratos"));

    val repeated = "foo bar foo baz foo";
    println("Original: " + repeated);
    println("Replace all 'foo' with 'qux': " + strings.replaceAll(repeated, "foo", "qux"));
    println("");

    // String repetition
    println("--- Repetition ---");
    println("Repeat 'Hello' 3 times: " + strings.repeat("Hello", 3));
    println("Repeat '=' 20 times: " + strings.repeat("=", 20));
    println("");

    // Truncation
    println("--- Truncation ---");
    val longText = "This is a very long string that needs to be truncated";
    println("Original: " + longText);
    println("Truncate to 20 chars: " + strings.truncate(longText, 20, "..."));
    println("Truncate to 30 chars: " + strings.truncate(longText, 30, "..."));
    println("");

    // Word count
    println("--- Word Count ---");
    val sentence = "The quick brown fox jumps over the lazy dog";
    println("Sentence: " + sentence);
    println("Word count: " + strings.wordCount(sentence));
    println("");

    // Character type checking
    println("--- Character Type Checking ---");
    println("'12345' is digit: " + strings.isDigit("12345"));
    println("'123abc' is digit: " + strings.isDigit("123abc"));
    println("'hello' is alpha: " + strings.isAlpha("hello"));
    println("'hello123' is alpha: " + strings.isAlpha("hello123"));
    println("'abc123' is alphanumeric: " + strings.isAlphaNumeric("abc123"));
    println("'abc-123' is alphanumeric: " + strings.isAlphaNumeric("abc-123"));
    println("");

    // Lines
    println("--- Lines ---");
    val multiline = "First line\nSecond line\nThird line";
    println("Multiline text:");
    val lineArray = strings.lines(multiline);
    for val i in 0..lineArray.length() {
        println("  Line " + (i + 1) + ": " + lineArray[i]);
    }
    println("");

    // Indentation
    println("--- Indentation ---");
    val code = "if condition {\n    doSomething();\n}";
    println("Original:");
    println(code);
    println("Indented by 4 spaces:");
    println(strings.indent(code, 4));
    println("");

    // Centering
    println("--- Centering ---");
    val title = "TITLE";
    println("Original: " + title);
    println("Centered in 20: '" + strings.center(title, 20, " ") + "'");
    println("Centered in 20 with '=': '" + strings.center(title, 20, "=") + "'");
    println("");

    // Common prefix
    println("--- Common Prefix ---");
    val words: Array<string> = ["testing", "tester", "test"];
    println("Words: testing, tester, test");
    println("Common prefix: '" + strings.commonPrefix(words) + "'");
    println("");

    // String formatting
    println("--- String Formatting (sprintf) ---");
    val name = "Alice";
    val age = 25;

    // Using {} placeholders
    val formatted1 = strings.sprintf("Hello {}, you are {} years old", [name, "" + age]);
    println(formatted1);

    // Using Rust-like format specifiers
    val formatted2 = strings.sprintf("Hello %s, you are %d years old", [name, "" + age]);
    println(formatted2);

    // Using %v (value) format
    val formatted3 = strings.sprintf("Name: %v, Age: %v", [name, "" + age]);
    println(formatted3);

    val template = "The %s %s fox jumps over the %s dog";
    val result = strings.sprintf(template, ["quick", "brown", "lazy"]);
    println(result);
    println("");

    // Remove whitespace
    println("--- Remove Whitespace ---");
    val spacedText = "H e l l o   W o r l d";
    println("Original: '" + spacedText + "'");
    println("No whitespace: '" + strings.removeWhitespace(spacedText) + "'");
    println("");

    println("=== All String Examples Completed ===");
}
