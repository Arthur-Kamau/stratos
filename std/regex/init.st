// Regex module - Regular expression support
// Provides pattern matching, searching, and text manipulation using regex

package regex;

// ============================================================================
// Types
// ============================================================================

class Match {
    var text: string;           // The full matched text
    var start: int;             // Start position of match
    var end: int;               // End position of match
    var groups: Array<string>;  // Capture groups (group 0 is full match)
}

class Regex {
    var pattern: string;
    var flags: int;
    var valid: bool;
}

// Regex flags
val CASE_INSENSITIVE = 1;  // Case-insensitive matching
val MULTILINE = 2;         // ^ and $ match line boundaries
val DOTALL = 4;            // . matches newlines
val EXTENDED = 8;          // Ignore whitespace and allow comments

// ============================================================================
// Native function declarations - Core Functions
// ============================================================================

// Compile a regex pattern
// Returns 1 if valid, 0 if invalid
fn compile(pattern: string) int;

// Compile with flags
fn compileWithFlags(pattern: string, flags: int) int;

// Test if entire string matches pattern
fn matches(pattern: string, text: string) bool;

// Test if pattern is found anywhere in string
fn contains(pattern: string, text: string) bool;

// Find first match in string
// Returns match position or -1 if not found
fn find(pattern: string, text: string) int;

// Find all matches in string
// Returns array of start positions
fn findAll(pattern: string, text: string) Array<int>;

// Replace first occurrence
fn replace(pattern: string, text: string, replacement: string) string;

// Replace all occurrences
fn replaceAll(pattern: string, text: string, replacement: string) string;

// Split string by regex pattern
fn split(pattern: string, text: string) Array<string>;

// Get capture groups from first match
// Returns array where index 0 is full match, 1+ are groups
fn groups(pattern: string, text: string) Array<string>;

// Get all matches with capture groups
fn matchAll(pattern: string, text: string) Array<Array<string>>;

// Escape special regex characters
fn escape(text: string) string;

// ============================================================================
// Pure Stratos implementations - Helper Functions
// ============================================================================

// Test if string starts with pattern
fn startsWith(pattern: string, text: string) bool {
    val anchoredPattern = "^" + pattern;
    return contains(anchoredPattern, text);
}

// Test if string ends with pattern
fn endsWith(pattern: string, text: string) bool {
    val anchoredPattern = pattern + "$";
    return contains(anchoredPattern, text);
}

// Count occurrences of pattern
fn count(pattern: string, text: string) int {
    val positions = findAll(pattern, text);
    return positions.length();
}

// Extract all matches (without groups)
fn extract(pattern: string, text: string) Array<string> {
    val allMatches = matchAll(pattern, text);
    var results = Array<string>();

    for val match in allMatches {
        if match.length() > 0 {
            results.add(match[0]);  // Add full match
        }
    }

    return results;
}

// Replace with callback function
fn replaceFunc(pattern: string, text: string, replacer: Function<string, string>) string {
    val matches = extract(pattern, text);
    var result = text;

    for val match in matches {
        val replacement = replacer(match);
        result = replace(pattern, result, replacement);
    }

    return result;
}

// Test multiple patterns (OR)
fn matchesAny(patterns: Array<string>, text: string) bool {
    for val pattern in patterns {
        if matches(pattern, text) {
            return true;
        }
    }
    return false;
}

// Test all patterns (AND)
fn matchesAll(patterns: Array<string>, text: string) bool {
    for val pattern in patterns {
        if !matches(pattern, text) {
            return false;
        }
    }
    return true;
}

// ============================================================================
// Common Regex Patterns
// ============================================================================

// Email validation (simplified)
val EMAIL_PATTERN = "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}";

// URL pattern (simplified)
val URL_PATTERN = "https?://[a-zA-Z0-9.-]+(:[0-9]+)?(/.*)?";

// IPv4 address
val IPV4_PATTERN = "((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)";

// Phone number (US format)
val PHONE_PATTERN = "\\(?[0-9]{3}\\)?[-. ]?[0-9]{3}[-. ]?[0-9]{4}";

// Date patterns
val DATE_ISO = "[0-9]{4}-[0-9]{2}-[0-9]{2}";  // YYYY-MM-DD
val DATE_US = "[0-9]{2}/[0-9]{2}/[0-9]{4}";   // MM/DD/YYYY
val DATE_EU = "[0-9]{2}\\.[0-9]{2}\\.[0-9]{4}"; // DD.MM.YYYY

// Time pattern (24-hour)
val TIME_24H = "[0-2][0-9]:[0-5][0-9](:[0-5][0-9])?";

// Hex color code
val HEX_COLOR = "#[0-9a-fA-F]{6}";

// Integer number
val INTEGER = "-?[0-9]+";

// Decimal number
val DECIMAL = "-?[0-9]+(\\.[0-9]+)?";

// Whitespace
val WHITESPACE = "\\s+";

// Word boundary
val WORD = "\\b\\w+\\b";

// ============================================================================
// Validation Functions
// ============================================================================

fn isEmail(text: string) bool {
    return matches(EMAIL_PATTERN, text);
}

fn isURL(text: string) bool {
    return matches(URL_PATTERN, text);
}

fn isIPv4(text: string) bool {
    return matches(IPV4_PATTERN, text);
}

fn isPhoneNumber(text: string) bool {
    return matches(PHONE_PATTERN, text);
}

fn isDateISO(text: string) bool {
    return matches(DATE_ISO, text);
}

fn isInteger(text: string) bool {
    return matches("^" + INTEGER + "$", text);
}

fn isDecimal(text: string) bool {
    return matches("^" + DECIMAL + "$", text);
}

fn isHexColor(text: string) bool {
    return matches("^" + HEX_COLOR + "$", text);
}

// ============================================================================
// Text Processing Functions
// ============================================================================

// Extract all emails from text
fn extractEmails(text: string) Array<string> {
    return extract(EMAIL_PATTERN, text);
}

// Extract all URLs from text
fn extractURLs(text: string) Array<string> {
    return extract(URL_PATTERN, text);
}

// Extract all numbers from text
fn extractNumbers(text: string) Array<string> {
    return extract(DECIMAL, text);
}

// Extract all words from text
fn extractWords(text: string) Array<string> {
    return extract(WORD, text);
}

// Remove all whitespace
fn removeWhitespace(text: string) string {
    return replaceAll(WHITESPACE, text, "");
}

// Normalize whitespace (collapse multiple spaces to one)
fn normalizeWhitespace(text: string) string {
    return replaceAll("\\s+", text, " ");
}

// Remove HTML tags
fn stripHTMLTags(text: string) string {
    return replaceAll("<[^>]+>", text, "");
}

// Extract text between delimiters
fn extractBetween(text: string, start: string, end: string) Array<string> {
    val pattern = escape(start) + "(.*?)" + escape(end);
    val matches = matchAll(pattern, text);
    var results = Array<string>();

    for val match in matches {
        if match.length() > 1 {
            results.add(match[1]);  // Add captured group
        }
    }

    return results;
}

// Mask sensitive data (like credit card numbers)
fn maskSensitive(text: string, pattern: string, maskChar: string) string {
    return replaceAll(pattern, text, maskChar);
}

// Convert camelCase to snake_case
fn camelToSnake(text: string) string {
    var result = replaceAll("([A-Z])", text, "_$1");
    return strings.toLowerCase(result);
}

// Convert snake_case to camelCase
fn snakeToCamel(text: string) string {
    return replaceAll("_([a-z])", text, "\\U$1");
}

// ============================================================================
// Advanced Pattern Matching
// ============================================================================

// Find overlapping matches
fn findOverlapping(pattern: string, text: string) Array<string> {
    var results = Array<string>();
    var i = 0;

    while i < strings.length(text) {
        val substring = strings.substring(text, i, strings.length(text));
        val pos = find(pattern, substring);

        if pos >= 0 {
            // Extract the match
            val match = extract(pattern, substring);
            if match.length() > 0 {
                results.add(match[0]);
            }
            i = i + pos + 1;  // Move one character past the start
        } else {
            i = i + 1;
        }
    }

    return results;
}

// Split with limit
fn splitLimit(pattern: string, text: string, limit: int) Array<string> {
    val parts = split(pattern, text);

    if parts.length() <= limit {
        return parts;
    }

    var results = Array<string>();
    var i = 0;

    while i < limit - 1 {
        results.add(parts[i]);
        i = i + 1;
    }

    // Join remaining parts
    var remaining = "";
    while i < parts.length() {
        if i > limit - 1 {
            remaining = remaining + " ";
        }
        remaining = remaining + parts[i];
        i = i + 1;
    }
    results.add(remaining);

    return results;
}

// Match with position information
class MatchInfo {
    var text: string;
    var start: int;
    var end: int;
    var length: int;
}

// Get detailed match information
fn findWithInfo(pattern: string, text: string) MatchInfo? {
    val pos = find(pattern, text);

    if pos < 0 {
        return null;
    }

    val matches = extract(pattern, text);
    if matches.length() == 0 {
        return null;
    }

    val matchText = matches[0];

    return MatchInfo {
        text: matchText,
        start: pos,
        end: pos + strings.length(matchText),
        length: strings.length(matchText)
    };
}

// ============================================================================
// Utility Functions
// ============================================================================

// Check if pattern is valid
fn isValid(pattern: string) bool {
    return compile(pattern) == 1;
}

// Quote/escape a string to be used as literal text in regex
fn quote(text: string) string {
    return escape(text);
}

// Create alternation pattern (a|b|c)
fn alternate(patterns: Array<string>) string {
    return strings.join(patterns, "|");
}

// Create character class [abc]
fn charClass(chars: string) string {
    return "[" + escape(chars) + "]";
}

// Create negated character class [^abc]
fn negatedCharClass(chars: string) string {
    return "[^" + escape(chars) + "]";
}

// Make pattern optional (pattern)?
fn optional(pattern: string) string {
    return "(" + pattern + ")?";
}

// Make pattern repeat one or more times (pattern)+
fn oneOrMore(pattern: string) string {
    return "(" + pattern + ")+";
}

// Make pattern repeat zero or more times (pattern)*
fn zeroOrMore(pattern: string) string {
    return "(" + pattern + ")*";
}

// Make pattern repeat exactly n times
fn exactly(pattern: string, n: int) string {
    return "(" + pattern + "){" + convert.toString(n) + "}";
}

// Make pattern repeat between min and max times
fn between(pattern: string, min: int, max: int) string {
    return "(" + pattern + "){" + convert.toString(min) + "," + convert.toString(max) + "}";
}
