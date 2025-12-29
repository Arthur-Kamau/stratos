# Stratos Style Guide

Official style guide for writing Stratos code. The `stratos fmt` command enforces these rules automatically.

## Table of Contents

- [Philosophy](#philosophy)
- [Formatting Rules](#formatting-rules)
  - [Indentation](#indentation)
  - [Line Length](#line-length)
  - [Spacing](#spacing)
  - [Braces](#braces)
  - [Semicolons](#semicolons)
  - [Blank Lines](#blank-lines)
- [Declarations](#declarations)
  - [Variables](#variables)
  - [Functions](#functions)
  - [Classes](#classes)
  - [Packages](#packages)
- [Statements](#statements)
  - [Conditionals](#conditionals)
  - [Loops](#loops)
  - [Return Statements](#return-statements)
- [Expressions](#expressions)
  - [Binary Operations](#binary-operations)
  - [Function Calls](#function-calls)
  - [Array Indexing](#array-indexing)
- [Comments](#comments)
- [Using the Formatter](#using-the-formatter)

## Philosophy

The Stratos style guide follows the principle of **"one standard way to format code"**. This approach:

- **Eliminates debates** about formatting preferences
- **Reduces cognitive load** when reading code
- **Makes code reviews** focus on logic, not style
- **Enables automatic formatting** with `stratos fmt`

Inspired by Go's `gofmt` and Rust's `rustfmt`, the formatter is **opinionated and non-configurable**. If the formatter produces it, it's correct.

## Formatting Rules

### Indentation

- Use **4 spaces** for each indentation level
- **Never use tabs**

```stratos
fn example() {
    val x = 10;
    if (x > 5) {
        println("Greater than 5");
    }
}
```

### Line Length

- No hard limit enforced by the formatter
- Keep lines reasonable (generally under 100 characters for readability)

### Spacing

#### Space After Keywords

Always put a space after control flow keywords:

```stratos
// ✓ Correct
if (condition) { }
while (condition) { }
for (item in list) { }

// ✗ Incorrect
if(condition) { }
while(condition) { }
for(item in list) { }
```

#### Space Around Operators

Binary operators should have space on both sides:

```stratos
// ✓ Correct
val sum = a + b;
val result = x * y - z;
val isEqual = name == "Arthur";

// ✗ Incorrect
val sum = a+b;
val result = x*y-z;
val isEqual = name=="Arthur";
```

#### Space After Commas

Put a space after commas in parameter lists and arguments:

```stratos
// ✓ Correct
fn add(a: int, b: int) { }
calculate(10, 20, 30);

// ✗ Incorrect
fn add(a: int,b: int) { }
calculate(10,20,30);
```

#### Space After Colons

Put a space after colons in type annotations:

```stratos
// ✓ Correct
val name: String = "Arthur";
fn greet(user: String) { }

// ✗ Incorrect
val name:String = "Arthur";
fn greet(user:String) { }
```

#### No Space Before Function Parentheses

Don't put space between function name and opening parenthesis:

```stratos
// ✓ Correct
println("Hello");
calculateSum(a, b);

// ✗ Incorrect
println ("Hello");
calculateSum (a, b);
```

### Braces

#### Opening Braces on Same Line

Opening braces go on the same line as the declaration (K&R style):

```stratos
// ✓ Correct
fn hello() {
    println("world");
}

class User {
    val name: String;
}

if (condition) {
    doSomething();
}

// ✗ Incorrect
fn hello()
{
    println("world");
}

class User
{
    val name: String;
}
```

#### Single-Line vs Multi-Line Blocks

Always use braces, even for single statements:

```stratos
// ✓ Correct
if (condition) {
    doSomething();
}

// ✗ Incorrect (not supported by Stratos)
if (condition)
    doSomething();
```

### Semicolons

- Statements must end with semicolons
- Semicolons go immediately after the statement (no space before)

```stratos
// ✓ Correct
val x = 10;
println(x);
return result;

// ✗ Incorrect
val x = 10 ;
println(x) ;
```

### Blank Lines

- **One blank line** between top-level declarations (functions, classes, packages)
- **No blank lines** at the start or end of blocks

```stratos
// ✓ Correct
package main;

fn first() {
    println("First");
}

fn second() {
    println("Second");
}

class User {
    val name: String;
}

// ✗ Incorrect (no blank line between declarations)
package main;
fn first() {
    println("First");
}
fn second() {
    println("Second");
}
```

## Declarations

### Variables

```stratos
// Mutable variable
var count: int = 0;

// Immutable variable (preferred)
val name: String = "Arthur";

// Type inference
val age = 25;

// Multiple declarations
val x = 10;
val y = 20;
val z = 30;
```

### Functions

```stratos
// Function without parameters
fn hello() {
    println("Hello, World!");
}

// Function with parameters
fn greet(name: String) {
    println("Hello");
    println(name);
}

// Function with multiple parameters
fn add(a: int, b: int) int {
    return a + b;
}

// Function with return type
fn getAge(name: String) int {
    return 26;
}
```

**Note:** Functions without an explicit return type don't include `void` in formatted output:

```stratos
// ✓ Formatted as:
fn hello() {
    println("world");
}

// Not:
fn hello() void {
    println("world");
}
```

### Classes

```stratos
// Simple class
class User {
    val name: String;
    val age: int;
}

// Class with inheritance
class Student extends User {
    val grade: int;
}

// Class with methods
class Calculator {
    fn add(a: int, b: int) int {
        return a + b;
    }

    fn subtract(a: int, b: int) int {
        return a - b;
    }
}
```

### Packages

Package declarations should be the first statement in a file:

```stratos
package main;

// Rest of the code...
```

Use statements follow package declarations:

```stratos
package main;

use math;
use strings;

fn main() {
    // ...
}
```

## Statements

### Conditionals

```stratos
// Simple if
if (condition) {
    doSomething();
}

// If-else
if (x > 10) {
    println("Greater");
} else {
    println("Smaller or equal");
}

// If-else-if
if (score >= 90) {
    println("A");
} else if (score >= 80) {
    println("B");
} else {
    println("C");
}
```

### Loops

#### While Loops

```stratos
while (count < 10) {
    println(count);
    count = count + 1;
}
```

#### For Loops (if supported)

```stratos
for (item in collection) {
    println(item);
}
```

### Return Statements

```stratos
// Return with value
fn getValue() int {
    return 42;
}

// Return without value
fn doSomething() {
    println("Done");
    return;
}

// Return expression
fn add(a: int, b: int) int {
    return a + b;
}
```

## Expressions

### Binary Operations

Always space around binary operators:

```stratos
// Arithmetic
val sum = a + b;
val product = x * y;
val difference = a - b;
val quotient = x / y;

// Comparison
val isEqual = x == y;
val isNotEqual = x != y;
val isGreater = x > y;
val isLess = x < y;

// Logical
val both = condition1 && condition2;
val either = condition1 || condition2;
```

### Function Calls

```stratos
// No arguments
hello();

// Single argument
greet("World");

// Multiple arguments
calculate(10, 20, 30);

// Nested calls
process(getData(), transform());
```

### Array Indexing

```stratos
// Array access
val item = array[0];
val value = matrix[i][j];

// No spaces around brackets
items[index];
```

### Grouping

Use parentheses for grouping expressions:

```stratos
val result = (a + b) * c;
val value = (x > 0) && (y > 0);
```

## Comments

**Important:** The current formatter **does not preserve comments** because comments are not part of the AST (Abstract Syntax Tree). This is a known limitation.

When the formatter is run, comments will be removed. Until comment preservation is implemented:

- Add comments **after** formatting
- Or avoid using the `-w` flag to preserve commented files

```stratos
// This comment will be lost after formatting
fn hello() {
    println("world");  // This too
}
```

**Future Enhancement:** Comment preservation is planned for a future version.

## Using the Formatter

### Important Requirements

**The formatter requires syntactically valid code.** If your code has syntax errors (missing semicolons, incorrect keywords, etc.), the formatter may crash with a segmentation fault.

**Before formatting:**
```bash
# Always validate your code first with the check command
stratos check src/main.st

# Then format if validation passes
stratos fmt src/main.st -w
```

If you encounter a segfault:
1. Run `stratos check <file>` to identify syntax errors
2. Fix the syntax errors
3. Then run `stratos fmt` again

### Check if Code is Formatted

```bash
# Check single file
stratos fmt src/main.st --check

# Check entire project
stratos fmt . --check

# Use in CI/CD
stratos fmt . --check || exit 1
```

### Format Code

```bash
# Print formatted code to stdout
stratos fmt src/main.st

# Format file in-place
stratos fmt src/main.st -w

# Format entire project
stratos fmt . -w

# Format with verbose output
stratos fmt src/ -w -v
```

### Before and After Example

**Before formatting:**
```stratos
package main ;


fn hello(){
   println("hello world");
}

fn add(a:int,b:int)int{
return a+b;
}

fn main(){
val result=add(1,2);
println(result);
}
```

**After formatting:**
```stratos
package main;

fn hello() {
    println("hello world");
}

fn add(a: int, b: int) int {
    return a + b;
}

fn main() {
    val result = add(1, 2);
    println(result);
}
```

## Integration

### Pre-commit Hook

Add to `.git/hooks/pre-commit`:

```bash
#!/bin/bash
stratos fmt . --check || {
    echo "Code is not formatted. Run: stratos fmt . -w"
    exit 1
}
```

### CI/CD Pipeline

```yaml
# GitHub Actions example
- name: Check formatting
  run: stratos fmt . --check
```

### Editor Integration

Most editors can be configured to run `stratos fmt -w` on save. Configure your editor to run:

```bash
stratos fmt %filepath -w
```

## Questions?

This style guide is enforced by `stratos fmt`. If you have questions about style:

1. Run `stratos fmt` on your code
2. The output is the correct style
3. Refer to this guide for explanations

The formatter is the ultimate source of truth for Stratos style.
