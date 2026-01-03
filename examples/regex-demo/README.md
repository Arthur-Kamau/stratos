# Regex Demo

This example demonstrates the capabilities of Stratos's regex module for pattern matching, text searching, and string manipulation.

## Features Demonstrated

- **Pattern Matching**: Test if strings match patterns
- **Search Operations**: Find patterns within text
- **Text Replacement**: Replace patterns with new text
- **Capture Groups**: Extract matched subpatterns
- **Validation**: Email, URL, number validation
- **Position Finding**: Locate pattern occurrences
- **Text Processing**: Clean, extract, and transform text

## Files

- `src/minimal_test.st` - Quick verification of basic regex functions
- `src/features_demo.st` - Comprehensive demonstration of all features
- `src/email_validator.st` - Email validation example
- `src/validation_demo.st` - Various validation patterns
- `src/text_processing.st` - Text manipulation examples
- `src/replace_demo.st` - Search and replace examples

## Running the Examples

### Minimal Test

Quick test of core functionality:

```bash
../../src/interpreter/cpp/build/stratos run src/minimal_test.st
```

**Output:**
```
✓ Pattern matching works
✓ Contains search works
✓ Find position: 6
✓ Replace: I like oranges
✓ Replace all: BAR bar BAR
```

### Features Demo

Comprehensive demonstration of all regex features:

```bash
../../src/interpreter/cpp/build/stratos run src/features_demo.st
```

This demonstrates:
- Pattern matching with `matches()`
- Search with `contains()`
- Finding positions with `find()`
- Single replacement with `replace()`
- Global replacement with `replaceAll()`
- Escaping special characters
- Capture groups
- Finding all occurrences
- Email validation

## Regex Module API

### Core Functions

**Pattern Validation:**
- `compile(pattern)` - Check if pattern is valid
- `compileWithFlags(pattern, flags)` - Compile with options

**Matching:**
- `matches(pattern, text)` - Test if entire string matches
- `contains(pattern, text)` - Test if pattern found anywhere
- `find(pattern, text)` - Get position of first match
- `findAll(pattern, text)` - Get all match positions

**Replacement:**
- `replace(pattern, text, replacement)` - Replace first occurrence
- `replaceAll(pattern, text, replacement)` - Replace all occurrences

**Extraction:**
- `groups(pattern, text)` - Extract capture groups from first match
- `matchAll(pattern, text)` - Get all matches with groups

**Utilities:**
- `escape(text)` - Escape special regex characters
- `split(pattern, text)` - Split string by pattern

### Validation Functions

- `isEmail(text)` - Validate email addresses
- `isURL(text)` - Validate URLs
- `isIPv4(text)` - Validate IPv4 addresses
- `isInteger(text)` - Check if valid integer
- `isDecimal(text)` - Check if valid decimal
- `isHexColor(text)` - Validate hex color codes

## Common Use Cases

### 1. Email Validation

```stratos
use regex;

fn main() {
    val emailPattern = "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}";

    if regex.matches(emailPattern, "user@example.com") {
        println("Valid email");
    }
}
```

### 2. Text Cleaning

```stratos
// Remove extra whitespace
val cleaned = regex.replaceAll("  +", "Too    many    spaces", " ");

// Remove HTML tags
val plain = regex.replaceAll("<[^>]+>", "<p>Hello</p>", "");

// Censor words
val censored = regex.replaceAll("bad", "bad word", "***");
```

### 3. Data Extraction

```stratos
// Extract all numbers
val numbers = regex.matchAll("[0-9]+", "Found 42 items and 17 more");

// Extract emails
val emails = regex.matchAll(emailPattern, "Contact: a@b.com or c@d.com");

// Get positions
val positions = regex.findAll("a", "banana");  // Returns: 1;3;5
```

### 4. Phone Number Formatting

```stratos
val phone = "5551234567";
val formatted = regex.replace("([0-9]{3})([0-9]{3})([0-9]{4})", phone, "($1) $2-$3");
// Result: (555) 123-4567
```

### 5. URL Parsing

```stratos
val url = "https://example.com:8080/path?query=value";
val pattern = "(https?)://([^:]+)(:[0-9]+)?(.*)";
val parts = regex.groups(pattern, url);
// Extract protocol, domain, port, path
```

## Pattern Syntax Reference

### Character Classes
- `.` - Any character
- `\d` - Digit [0-9]
- `\w` - Word character [a-zA-Z0-9_]
- `\s` - Whitespace
- `[abc]` - Any of a, b, or c
- `[^abc]` - Not a, b, or c
- `[a-z]` - Range

### Quantifiers
- `*` - 0 or more
- `+` - 1 or more
- `?` - 0 or 1
- `{n}` - Exactly n times
- `{n,m}` - Between n and m times

### Anchors
- `^` - Start of string
- `$` - End of string
- `\b` - Word boundary

### Groups
- `(...)` - Capture group
- `|` - OR operator

### Examples
- `[a-z]+` - One or more lowercase letters
- `\d{3}-\d{4}` - Pattern like 123-4567
- `^https?://` - Starts with http:// or https://
- `.*@.*\.com$` - Ends with @something.com
- `([A-Z]\w+)` - Capitalized word (captured)

## Common Patterns

```stratos
// Email
"[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}"

// URL
"https?://[a-zA-Z0-9.-]+(:[0-9]+)?(/.*)?"

// Phone (US)
"\\(?[0-9]{3}\\)?[-. ]?[0-9]{3}[-. ]?[0-9]{4}"

// Date (YYYY-MM-DD)
"[0-9]{4}-[0-9]{2}-[0-9]{2}"

// Hex Color
"#[0-9a-fA-F]{6}"

// IPv4 Address
"((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"

// Integer
"-?[0-9]+"

// Decimal
"-?[0-9]+(\\.[0-9]+)?"
```

## Return Value Formats

### String-Based Returns

Since full array support is still in development, some functions return formatted strings:

**findAll()**: Semicolon-separated positions
```
"1;3;5"  // Found at positions 1, 3, and 5
```

**groups()**: Semicolon-separated groups
```
"full match;group1;group2"
```

**matchAll()**: Pipe-separated matches, semicolon-separated groups
```
"match1|match2|match3"
"full;g1;g2|full;g1;g2"
```

Parse these with `strings.split()`:
```stratos
val positions = regex.findAll("a", "banana");
val parts = strings.split(positions, ";");
// parts[0] = "1", parts[1] = "3", parts[2] = "5"
```

## Tips and Tricks

1. **Escape special characters**: Use `regex.escape()` for literal matching
2. **Test patterns first**: Use `regex.compile()` to validate before use
3. **Use capture groups**: Extract specific parts with parentheses
4. **Anchors for exact matches**: Use `^pattern$` for full string matching
5. **Non-greedy matching**: Use `.*?` instead of `.*` when possible

## Requirements

- Terminal or command-line environment
- Stratos interpreter with regex module support

## Platform Support

- **Linux/Unix**: Full support using C++ `<regex>` library
- **macOS**: Full support
- **Windows**: Full support

## See Also

- [Regex Module Documentation](../../docs/stdlib/regex.md)
- [Standard Library Documentation](../../std/README.md)
- [Strings Module](../../docs/stdlib/strings.md)
