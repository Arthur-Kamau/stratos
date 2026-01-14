---
title: Strings Module
description: String manipulation and formatting in Stratos
---

# Strings Module

The `strings` module provides comprehensive string manipulation utilities inspired by Go's strings package and Kotlin's string extensions.

## Importing

```stratos
use strings;
```

## Basic Operations

### Length and Empty Checks

```stratos
val text = "Hello, World!";
val len = strings.length(text);        // 13
val empty = strings.isEmpty(text);     // false
```

### Contains and Search

```stratos
val text = "Hello, World!";

strings.contains(text, "World");       // true
strings.startsWith(text, "Hello");     // true
strings.endsWith(text, "!");           // true
strings.indexOf(text, "World");        // 7
```

### Substring and Character Access

```stratos
val text = "Hello, World!";

val hello = strings.substring(text, 0, 5);    // "Hello"
val world = strings.substring(text, 7, 12);   // "World"
val firstChar = strings.charAt(text, 0);      // "H"
```

## Case Conversion

```stratos
val text = "Hello World";

strings.toUpper(text);          // "HELLO WORLD"
strings.toLower(text);          // "hello world"
strings.capitalize(text);       // "Hello World"
strings.uncapitalize(text);     // "hello World"
```

## Case Transformations

Convert between different naming conventions:

```stratos
val phrase = "hello world example";

strings.toCamelCase(phrase);    // "helloWorldExample"
strings.toPascalCase(phrase);   // "HelloWorldExample"
strings.toSnakeCase(phrase);    // "hello_world_example"
strings.toKebabCase(phrase);    // "hello-world-example"
```

## String Manipulation

### Trimming

```stratos
val padded = "  Hello World  ";
strings.trim(padded);           // "Hello World"
```

### Splitting

```stratos
val csv = "apple,banana,orange";
val fruits = strings.split(csv, ",");
// fruits[0] = "apple"
// fruits[1] = "banana"
// fruits[2] = "orange"
```

### Replacement

```stratos
val text = "Hello World";
strings.replace(text, "World", "Stratos");     // "Hello Stratos"

val repeated = "foo bar foo baz foo";
strings.replaceAll(repeated, "foo", "qux");    // "qux bar qux baz qux"
```

### Repetition

```stratos
strings.repeat("Hello", 3);     // "HelloHelloHello"
strings.repeat("=", 20);        // "===================="
```

## String Formatting (sprintf)

The `sprintf` function provides powerful string formatting with multiple placeholder styles:

### Format Specifiers

| Specifier | Description | Example |
|-----------|-------------|---------|
| `{}` | Positional placeholder | `"Hello {}"` |
| `%v` | Value (default formatting) | `"Value: %v"` |
| `%s` | String | `"Name: %s"` |
| `%d` | Integer/Digit | `"Age: %d"` |
| `%f` | Float/Double | `"Price: %f"` |
| `%#v` | Debug representation | `"Debug: %#v"` |

### Basic Usage

```stratos
use strings;

// Using {} placeholders
val name = "Alice";
val age = 25;
val msg1 = strings.sprintf("Hello {}, you are {} years old", [name, "" + age]);
// "Hello Alice, you are 25 years old"

// Using Rust-like format specifiers
val msg2 = strings.sprintf("Hello %s, you are %d years old", [name, "" + age]);
// "Hello Alice, you are 25 years old"

// Using %v (value) format
val msg3 = strings.sprintf("Name: %v, Age: %v", [name, "" + age]);
// "Name: Alice, Age: 25"
```

### Advanced Formatting

```stratos
// Multiple values
val template = "The %s %s fox jumps over the %s dog";
val result = strings.sprintf(template, ["quick", "brown", "lazy"]);
// "The quick brown fox jumps over the lazy dog"

// Debug representation (useful for debugging complex types)
val value = "some-value";
val debug = strings.sprintf("Debug: %#v", [value]);
```

::: tip Format Specifier Priority
When sprintf processes arguments, it looks for format specifiers in this order:
1. `%#v` (debug format)
2. `%v`, `%s`, `%d`, `%f` (any of these)
3. `{}` (positional)

Each argument is matched to the first available specifier found.
:::

::: warning Type Conversion
Since array literals require all elements to be the same type, you must convert non-string values to strings:

```stratos
val age = 25;
strings.sprintf("Age: %d", ["" + age]);  // Convert int to string
```
:::

## Character Type Checking

```stratos
strings.isDigit("12345");          // true
strings.isDigit("123abc");         // false

strings.isAlpha("hello");          // true
strings.isAlpha("hello123");       // false

strings.isAlphaNumeric("abc123");  // true
strings.isAlphaNumeric("abc-123"); // false
```

## String Utilities

### Truncation

```stratos
val longText = "This is a very long string that needs to be truncated";
strings.truncate(longText, 20, "...");  // "This is a very lo..."
```

### Word Count

```stratos
val sentence = "The quick brown fox jumps over the lazy dog";
strings.wordCount(sentence);  // 9
```

### Lines

```stratos
val multiline = "First line\nSecond line\nThird line";
val lines = strings.lines(multiline);
// lines[0] = "First line"
// lines[1] = "Second line"
// lines[2] = "Third line"
```

### Indentation

```stratos
val code = "if condition {\n    doSomething();\n}";
val indented = strings.indent(code, 4);
// Indents each line by 4 spaces
```

### Centering

```stratos
val title = "TITLE";
strings.center(title, 20, " ");   // "       TITLE        "
strings.center(title, 20, "=");   // "=======TITLE========"
```

### Common Prefix

```stratos
val words: Array<string> = ["testing", "tester", "test"];
strings.commonPrefix(words);  // "test"
```

### Remove Whitespace

```stratos
val spacedText = "H e l l o   W o r l d";
strings.removeWhitespace(spacedText);  // "HelloWorld"
```

## String Iteration

Strings are iterable in Stratos. You can loop over each character:

```stratos
val text = "Hello";

// Direct character iteration
for val char in text {
    println(char);
}

// Index-based iteration
for val i in 0..strings.length(text) {
    val char = strings.charAt(text, i);
    println("Char at " + i + ": " + char);
}
```

See [Control Flow - String Iteration](/guide/control-flow#iterating-over-strings) for more details.

## Complete Example

```stratos
package main;

use strings;

fn main() {
    val text = "Hello, World!";

    // Basic operations
    println("Length: " + strings.length(text));
    println("Contains 'World': " + strings.contains(text, "World"));

    // Case conversion
    println("Upper: " + strings.toUpper(text));
    println("Lower: " + strings.toLower(text));

    // Substring
    val hello = strings.substring(text, 0, 5);
    println("Substring: " + hello);

    // Splitting
    val csv = "apple,banana,orange";
    val fruits = strings.split(csv, ",");
    for val i in 0..fruits.length() {
        println("Fruit " + i + ": " + fruits[i]);
    }

    // Formatting
    val name = "Alice";
    val age = 25;
    val msg = strings.sprintf("Hello %s, you are %d years old", [name, "" + age]);
    println(msg);

    // String iteration
    for val char in "Hello" {
        println(char);
    }
}
```

## See Also

- [Basics - Strings](/guide/basics#strings)
- [Control Flow - String Iteration](/guide/control-flow#iterating-over-strings)
- [Standard Library Reference](/reference/stdlib)
