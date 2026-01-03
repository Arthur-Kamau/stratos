# Stratos Documentation Guide

Learn how to write great documentation for your Stratos code and generate beautiful docs in multiple formats.

## Quick Start

### 1. Write Documented Code

Add documentation comments using `/** */` syntax:

```stratos
/**
 * Calculates the factorial of a number.
 *
 * @param n The number to calculate factorial for
 * @return The factorial of n
 * @example
 * val result = factorial(5);  // Returns 120
 */
fn factorial(n: int) int {
    if n <= 1 { return 1; }
    return n * factorial(n - 1);
}
```

### 2. Generate Documentation

```bash
# Generate HTML documentation (default)
stratos doc generate

# Generate Markdown documentation
stratos doc generate -f markdown

# Generate JSON documentation
stratos doc generate -f json
```

### 3. View Your Docs

Open `docs/index.html` in your browser to view the generated documentation!

## Writing Documentation Comments

### Basic Syntax

Documentation comments start with `/**` and end with `*/`:

```stratos
/**
 * This is a documentation comment.
 */
fn myFunction() void {
    // ...
}
```

### Summary and Description

The **first paragraph** becomes the summary. Additional paragraphs form the detailed description:

```stratos
/**
 * Validates an email address.
 *
 * This function checks if the provided string matches a valid
 * email format using regex pattern matching. It supports standard
 * email formats including international domains.
 *
 * @param email The email address to validate
 * @return True if valid, false otherwise
 */
fn isValidEmail(email: string) bool {
    // ...
}
```

### Documenting Parameters

Use `@param` to document each parameter:

```stratos
/**
 * Calculates the distance between two points.
 *
 * @param x1 X coordinate of first point
 * @param y1 Y coordinate of first point
 * @param x2 X coordinate of second point
 * @param y2 Y coordinate of second point
 * @return The distance between the points
 */
fn distance(x1: double, y1: double, x2: double, y2: double) double {
    // ...
}
```

### Documenting Return Values

Use `@return` to describe what the function returns:

```stratos
/**
 * Finds the maximum value in an array.
 *
 * @param numbers Array of numbers to search
 * @return The maximum value, or 0 if array is empty
 */
fn max(numbers: Array<int>) int {
    // ...
}
```

### Adding Examples

Use `@example` to show how to use your code:

```stratos
/**
 * Formats a number with thousand separators.
 *
 * @param num The number to format
 * @return Formatted string
 * @example
 * val formatted = formatNumber(1234567);
 * // Returns "1,234,567"
 */
fn formatNumber(num: int) string {
    // ...
}
```

### Exception Documentation

Use `@throws` to document what exceptions might be thrown:

```stratos
/**
 * Reads a file from disk.
 *
 * @param path Path to the file
 * @return File contents as string
 * @throws "FileNotFound" if file doesn't exist
 * @throws "PermissionDenied" if file cannot be read
 */
fn readFile(path: string) string {
    // ...
}
```

### Version Information

Use `@since` to indicate when a feature was added:

```stratos
/**
 * Validates a JWT token.
 *
 * @param token The JWT token to validate
 * @return True if valid
 * @since 2.0.0
 */
fn validateJWT(token: string) bool {
    // ...
}
```

### Deprecation Notices

Use `@deprecated` to mark deprecated code:

```stratos
/**
 * Gets the current user's name.
 *
 * @return User's name
 * @deprecated Use getUserProfile().name instead
 */
fn getUserName() string {
    // ...
}
```

## Documenting Classes

### Class Documentation

Document the class purpose and usage:

```stratos
/**
 * Represents a user account.
 *
 * This class encapsulates all user-related data and provides
 * methods for authentication, profile management, and permissions.
 *
 * @example
 * val user = User("john@example.com", "John Doe");
 * user.login("password123");
 */
class User {
    var email: string;
    var name: string;

    // ...
}
```

### Property Documentation

Document class properties:

```stratos
class User {
    /** User's email address (must be unique) */
    var email: string;

    /** User's full display name */
    var name: string;

    /** Whether the user account is active */
    var isActive: bool;
}
```

### Method Documentation

Document methods just like functions:

```stratos
class User {
    /**
     * Authenticates the user with a password.
     *
     * @param password The user's password
     * @return True if authentication successful
     */
    fn login(password: string) bool {
        // ...
    }
}
```

## Documenting Packages

Add package-level documentation at the start of your main file:

```stratos
/**
 * HTTP client library for making web requests.
 *
 * This package provides a simple HTTP client with support for
 * GET, POST, PUT, DELETE requests, custom headers, and automatic
 * JSON serialization.
 *
 * @example
 * use http;
 * val response = http.get("https://api.example.com/data");
 *
 * @since 1.0.0
 */
package http;

// Package contents...
```

## Using Markdown

Documentation supports basic markdown formatting:

```stratos
/**
 * Processes user input with **validation** and *sanitization*.
 *
 * This function handles:
 * - Input validation
 * - XSS prevention
 * - SQL injection protection
 *
 * Use `sanitize()` for simple cases or this function for
 * comprehensive protection.
 *
 * @param input Raw user input
 * @return Sanitized input safe for use
 */
fn processInput(input: string) string {
    // ...
}
```

Supported markdown:
- **Bold**: `**text**`
- *Italic*: `*text*`
- `Code`: `` `code` ``
- Links: `[text](url)`
- Lists: `-` or `*` for bullets

## Generating Documentation

### Command Syntax

```bash
stratos doc generate [options]
```

### Options

| Option | Description | Default |
|--------|-------------|---------|
| `-s, --source <dir>` | Source directory | `src/` |
| `-o, --output <dir>` | Output directory | `docs/` |
| `-f, --format <fmt>` | Output format (html/markdown/json) | `html` |
| `-t, --title <name>` | Project title | `Stratos Documentation` |
| `-v, --verbose` | Verbose output | Off |

### Examples

```bash
# Default: Generate HTML docs from src/ to docs/
stratos doc generate

# Custom source and output
stratos doc generate -s lib -o api-docs

# Generate Markdown docs
stratos doc generate -f markdown -o docs-md

# Generate JSON with custom title
stratos doc generate -f json -t "My API" -o api

# Verbose mode
stratos doc generate -v

# Document standard library
stratos doc generate -s /path/to/stratos/std -o stdlib-docs
```

## Output Formats

### HTML Documentation

**Best for**: Web hosting, interactive browsing

**Features**:
- Responsive design
- Clean, professional styling
- Navigation between packages
- Syntax-highlighted code
- Source code links

**Output structure**:
```
docs/
├── index.html           # Package index
├── package1.html        # Package documentation
├── package2.html
└── ...
```

**Usage**:
```bash
stratos doc generate -f html
# Open docs/index.html in browser
```

### Markdown Documentation

**Best for**: GitHub/GitLab, version control

**Features**:
- GitHub-friendly .md files
- Easy to read in text editors
- Version control friendly
- Integrates with GitHub Pages

**Output structure**:
```
docs/
├── README.md           # Package index
├── package1.md         # Package documentation
├── package2.md
└── ...
```

**Usage**:
```bash
stratos doc generate -f markdown
# View .md files in your editor or GitHub
```

### JSON Documentation

**Best for**: API documentation, tooling integration

**Features**:
- Machine-readable format
- Complete documentation structure
- Easy to parse programmatically
- IDE integration

**Output**:
```
docs/
└── documentation.json
```

**Usage**:
```bash
stratos doc generate -f json
# Use JSON for custom tools or API documentation
```

## Best Practices

### 1. Write Clear Summaries

The first paragraph is crucial - it appears in listings and summaries:

✅ **Good**:
```stratos
/**
 * Validates and parses a JSON string into an object.
 * ...
 */
```

❌ **Bad**:
```stratos
/**
 * This function does stuff with JSON.
 * ...
 */
```

### 2. Document All Public APIs

Every public function, class, and method should be documented:

✅ **Good**:
```stratos
/** Starts the HTTP server on the specified port. */
fn start(port: int) void { }

/** Stops the HTTP server gracefully. */
fn stop() void { }
```

### 3. Provide Practical Examples

Show real-world usage, not trivial examples:

✅ **Good**:
```stratos
/**
 * @example
 * val user = authenticate("user@example.com", "password");
 * if user != null {
 *     println("Welcome, " + user.name);
 * }
 */
```

❌ **Bad**:
```stratos
/**
 * @example
 * val result = myFunc(x);
 */
```

### 4. Keep Documentation Updated

Update docs when you change code:

```stratos
/**
 * Processes payment using the specified method.
 *
 * @param amount Payment amount in cents (not dollars!)
 * @param method Payment method (card/paypal/crypto)
 * @return Transaction ID
 * @since 2.0.0
 */
```

### 5. Use Consistent Style

Follow a consistent documentation style across your project:

```stratos
// All summaries should be complete sentences ending with period
// All @param descriptions should start with capital letter
// All examples should be realistic and runnable
```

### 6. Document Edge Cases

Mention special cases, limits, and gotchas:

```stratos
/**
 * Calculates percentage with rounding.
 *
 * Note: Returns 0 if denominator is 0 to avoid division by zero.
 * Results are rounded to 2 decimal places.
 *
 * @param numerator The numerator
 * @param denominator The denominator (0 returns 0)
 * @return Percentage value
 */
```

### 7. Link Related Items

Reference related functions and classes:

```stratos
/**
 * Validates a user's email address.
 *
 * Use this before calling `sendVerificationEmail()` to ensure
 * the email is valid. See also `isValidPhoneNumber()`.
 *
 * @param email Email address to validate
 * @return True if valid format
 */
```

## Project Setup

### Directory Structure

Recommended project structure:

```
my-project/
├── src/
│   ├── main.st
│   ├── utils.st
│   └── models.st
├── docs/                  # Generated documentation
├── stratos.conf
└── README.md
```

### stratos.conf

Include documentation settings in your project config:

```toml
name = "my-project"
version = "1.0.0"
entry = "src/main.st"

[documentation]
title = "My Project API Documentation"
source = "src"
output = "docs"
```

### CI/CD Integration

Generate docs automatically on each commit:

```yaml
# GitHub Actions example
- name: Generate Documentation
  run: |
    stratos doc generate -f html -o docs
    stratos doc generate -f markdown -o docs-md
```

## Troubleshooting

### Documentation Not Appearing

**Problem**: Functions documented but not showing in generated docs

**Solutions**:
1. Ensure doc comment is directly before the declaration (no blank lines)
2. Check that `/**` is used (not `/*`)
3. Run with `-v` flag to see which files are processed

### Formatting Issues

**Problem**: Markdown not rendering correctly

**Solutions**:
1. Check markdown syntax (proper backticks, asterisks)
2. Escape special characters if needed
3. View generated HTML to see raw output

### Missing Packages

**Problem**: Some packages not appearing

**Solutions**:
1. Check source directory path with `-s` flag
2. Ensure files have `.st` extension
3. Run with `-v` to see file processing

## Examples

See the `examples/doc-example/` directory for a complete, well-documented example project demonstrating all documentation features.

## Learn More

- [Implementation Guide](../developer/implementation/doc-generator.md) - Technical implementation details
- [Example Project](../../examples/doc-example/) - Complete documented example
- [Standard Library Docs](../../stdlib-docs/) - Generated standard library documentation

## Feedback

Have suggestions for the documentation generator? Open an issue on GitHub!
