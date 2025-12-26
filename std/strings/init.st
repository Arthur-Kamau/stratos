// Strings module - String manipulation utilities
// Inspired by Go's strings package and Kotlin's string extensions

package strings;

// ============================================================================
// Native function declarations (implemented in C++)
// ============================================================================

// Case conversion
fn toUpper(s: string) string;
fn toLower(s: string) string;
fn toTitle(s: string) string;

// Trimming
fn trim(s: string) string;
fn trimLeft(s: string) string;
fn trimRight(s: string) string;
fn trimPrefix(s: string, prefix: string) string;
fn trimSuffix(s: string, suffix: string) string;

// Splitting and joining
fn split(s: string, separator: string) Array<string>;
fn join(parts: Array<string>, separator: string) string;

// Searching
fn contains(s: string, substr: string) bool;
fn indexOf(s: string, substr: string) int;
fn lastIndexOf(s: string, substr: string) int;
fn startsWith(s: string, prefix: string) bool;
fn endsWith(s: string, suffix: string) bool;
fn count(s: string, substr: string) int;

// Replacement
fn replace(s: string, old: string, new: string) string;
fn replaceAll(s: string, old: string, new: string) string;

// Extraction
fn substring(s: string, start: int, end: int) string;
fn charAt(s: string, index: int) string;

// Repetition and padding
fn repeat(s: string, count: int) string;
fn padLeft(s: string, length: int, pad: string) string;
fn padRight(s: string, length: int, pad: string) string;

// Comparison
fn compare(a: string, b: string) int;
fn equals(a: string, b: string) bool;
fn equalsIgnoreCase(a: string, b: string) bool;

// Utility
fn length(s: string) int;
fn isEmpty(s: string) bool;
fn isBlank(s: string) bool;
fn reverse(s: string) string;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Check if string contains only digits
fn isDigit(s: string) bool {
    if isEmpty(s) {
        return false;
    }

    val digits = "0123456789";
    for val char in s {
        if !contains(digits, char) {
            return false;
        }
    }
    return true;
}

// Check if string contains only letters
fn isAlpha(s: string) bool {
    if isEmpty(s) {
        return false;
    }

    val lower = toLower(s);
    val letters = "abcdefghijklmnopqrstuvwxyz";

    for val char in lower {
        if !contains(letters, char) {
            return false;
        }
    }
    return true;
}

// Check if string contains only alphanumeric characters
fn isAlphaNumeric(s: string) bool {
    if isEmpty(s) {
        return false;
    }

    for val char in s {
        if !isAlpha(char) && !isDigit(char) {
            return false;
        }
    }
    return true;
}

// Capitalize first letter
fn capitalize(s: string) string {
    if isEmpty(s) {
        return s;
    }

    val first = toUpper(substring(s, 0, 1));
    val rest = substring(s, 1, length(s));
    return first + rest;
}

// Uncapitalize first letter
fn uncapitalize(s: string) string {
    if isEmpty(s) {
        return s;
    }

    val first = toLower(substring(s, 0, 1));
    val rest = substring(s, 1, length(s));
    return first + rest;
}

// Convert to camelCase
fn toCamelCase(s: string) string {
    val parts = split(s, " ");
    var result = "";

    for i in 0..parts.length() {
        if i == 0 {
            result = toLower(parts[i]);
        } else {
            result = result + capitalize(parts[i]);
        }
    }

    return result;
}

// Convert to PascalCase
fn toPascalCase(s: string) string {
    val parts = split(s, " ");
    var result = "";

    for val part in parts {
        result = result + capitalize(part);
    }

    return result;
}

// Convert to snake_case
fn toSnakeCase(s: string) string {
    val parts = split(s, " ");
    var result = "";

    for i in 0..parts.length() {
        if i == 0 {
            result = toLower(parts[i]);
        } else {
            result = result + "_" + toLower(parts[i]);
        }
    }

    return result;
}

// Convert to kebab-case
fn toKebabCase(s: string) string {
    val parts = split(s, " ");
    var result = "";

    for i in 0..parts.length() {
        if i == 0 {
            result = toLower(parts[i]);
        } else {
            result = result + "-" + toLower(parts[i]);
        }
    }

    return result;
}

// Truncate string to max length
fn truncate(s: string, maxLength: int, suffix: string) string {
    if length(s) <= maxLength {
        return s;
    }

    val truncated = substring(s, 0, maxLength - length(suffix));
    return truncated + suffix;
}

// Remove all whitespace
fn removeWhitespace(s: string) string {
    return replaceAll(replaceAll(replaceAll(s, " ", ""), "\t", ""), "\n", "");
}

// Word count
fn wordCount(s: string) int {
    val trimmed = trim(s);
    if isEmpty(trimmed) {
        return 0;
    }

    val words = split(trimmed, " ");
    return words.length();
}

// Lines
fn lines(s: string) Array<string> {
    return split(s, "\n");
}

// Indent each line
fn indent(s: string, spaces: int) string {
    val padding = repeat(" ", spaces);
    val lineArray = lines(s);
    var result = "";

    for i in 0..lineArray.length() {
        if i > 0 {
            result = result + "\n";
        }
        result = result + padding + lineArray[i];
    }

    return result;
}

// Center string within width
fn center(s: string, width: int, fill: string) string {
    val len = length(s);
    if len >= width {
        return s;
    }

    val padding = width - len;
    val leftPad = padding / 2;
    val rightPad = padding - leftPad;

    return repeat(fill, leftPad) + s + repeat(fill, rightPad);
}

// Common prefix of strings
fn commonPrefix(strings: Array<string>) string {
    if strings.length() == 0 {
        return "";
    }

    val first = strings[0];
    var prefix = "";

    for i in 0..length(first) {
        val char = charAt(first, i);
        var match = true;

        for val str in strings {
            if !startsWith(str, prefix + char) {
                match = false;
                break;
            }
        }

        if match {
            prefix = prefix + char;
        } else {
            break;
        }
    }

    return prefix;
}
