# Documentation Generator Example

This example project demonstrates the **Stratos Documentation Generator**, showcasing how to write properly documented code and generate beautiful documentation in multiple formats.

## Overview

The project includes:
- **math_utils.st** - Statistical and number theory functions
- **geometry.st** - Geometric shapes and calculations (Circle, Rectangle, Triangle)
- **main.st** - Example usage of the documented modules

All code is extensively documented using Kotlin-style `/** */` doc comments with JSDoc-like tags.

## Documentation Features Demonstrated

### 1. Package Documentation
```stratos
/**
 * Math utilities module - Advanced mathematical operations
 *
 * This module provides extended mathematical functions beyond the standard
 * math library, including statistical operations, number theory functions,
 * and geometric calculations.
 *
 * @since 1.0.0
 */
package mathutils;
```

### 2. Function Documentation
```stratos
/**
 * Checks if a number is prime.
 *
 * A prime number is a natural number greater than 1 that has no positive
 * divisors other than 1 and itself. This implementation uses trial division
 * up to the square root of n for efficiency.
 *
 * @param n The number to test for primality
 * @return True if n is prime, false otherwise
 * @example
 * val is7Prime = isPrime(7);   // Returns true
 * val is10Prime = isPrime(10); // Returns false
 * @since 1.0.0
 */
fn isPrime(n: int) bool {
    // Implementation...
}
```

### 3. Class Documentation
```stratos
/**
 * Represents a circle in 2D space.
 *
 * A circle is defined by a center point (x, y) and a radius.
 * This class provides methods for calculating area, circumference,
 * and checking if a point lies within the circle.
 *
 * @example
 * val circle = Circle(0.0, 0.0, 5.0);
 * val area = circle.area();  // Returns ~78.54
 * @since 1.0.0
 */
class Circle {
    // Implementation...
}
```

### 4. Supported Tags

The documentation generator supports these JSDoc-style tags:
- `@param` - Parameter descriptions
- `@return` - Return value description
- `@throws` - Exceptions that may be thrown
- `@example` - Usage examples
- `@since` - Version when introduced
- `@deprecated` - Deprecation notice

## Generating Documentation

### HTML Documentation (Interactive)
```bash
cd examples/doc-example
../../src/interpreter/cpp/build/stratos doc generate -f html -o docs
```
Then open `docs/index.html` in your browser.

### Markdown Documentation (GitHub-friendly)
```bash
cd examples/doc-example
../../src/interpreter/cpp/build/stratos doc generate -f markdown -o docs-md
```
View the generated `.md` files in `docs-md/`.

### JSON Documentation (API/Tooling)
```bash
cd examples/doc-example
../../src/interpreter/cpp/build/stratos doc generate -f json -o api-docs
```
The structured JSON will be at `api-docs/documentation.json`.

### Verbose Mode
```bash
../../src/interpreter/cpp/build/stratos doc generate -v
```

## Command Line Options

```
stratos doc generate [options]

Options:
  -s, --source <dir>     Source directory (default: src/)
  -o, --output <dir>     Output directory (default: docs/)
  -f, --format <fmt>     Output format: html, markdown, json (default: html)
  -t, --title <name>     Project title
  -v, --verbose          Verbose output
```

## Project Structure

```
doc-example/
├── README.md              # This file
├── stratos.conf           # Project configuration
├── src/
│   ├── main.st           # Main entry point
│   ├── math_utils.st     # Mathematical utilities
│   └── geometry.st       # Geometric shapes
└── docs/                 # Generated documentation (after running doc generate)
```

## Example Functions

### Statistical Functions
- `mean()` - Arithmetic mean of numbers
- `median()` - Median value
- `standardDeviation()` - Standard deviation

### Number Theory
- `isPrime()` - Prime number checker
- `factorial()` - Factorial calculation
- `gcd()` - Greatest common divisor

### Geometric Classes
- `Circle` - Circle with area, circumference, containment checks
- `Rectangle` - Rectangle with area, perimeter, diagonal
- `Triangle` - Triangle with area calculation

## Documentation Best Practices

Based on this example, here are some best practices:

1. **Start with a summary** - First paragraph is the summary
2. **Add detailed description** - Follow with detailed explanation
3. **Document all parameters** - Use `@param` for every parameter
4. **Describe return values** - Use `@return` to explain what's returned
5. **Provide examples** - Use `@example` to show usage
6. **Include edge cases** - Mention special cases and error conditions
7. **Use markdown** - Bold, italic, and code formatting work in docs
8. **Version tracking** - Use `@since` to track when features were added

## Running the Example

```bash
# Build the project
cd examples/doc-example
../../src/interpreter/cpp/build/stratos build

# Run the example
./build/doc-example

# Generate documentation
../../src/interpreter/cpp/build/stratos doc generate -f html
```

## Sample Output

The generated documentation will include:
- Package index with all modules
- Function signatures with parameter types
- Full descriptions with markdown formatting
- Example code snippets
- Source code links
- Navigation between packages

## Learn More

- See `docs/developer/implementation/doc-generator.md` for implementation details
- Check the [Stratos Documentation](../../docs/) for more information
- View the standard library documentation for more examples

## License

This example is part of the Stratos project and is provided for educational purposes.
