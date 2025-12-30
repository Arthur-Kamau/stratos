# Code Examples

Learn Stratos through practical, runnable examples. Each section demonstrates key language features with complete, working code.

## Example Categories

### [Basics](/examples/basics)
Variables, types, and basic operations

### [Control Flow](/examples/control-flow)
If statements, loops, and pattern matching

### [Functions](/examples/functions)
Function declarations and pipe operators

### [Null Safety](/examples/null-safety)
Optional types and safe navigation

### [Object-Oriented](/examples/oop)
Classes, interfaces, and inheritance

### [Packages](/examples/packages)
Module system and imports

## Running the Examples

All examples can be run directly with the Stratos interpreter:

```bash
# Save example to a file
echo 'fn main() { print("Hello!"); }' > example.st

# Run it
stratos run example.st
```

Or compile and run:

```bash
# Compile
stratos compile example.st -o example

# Execute
./example
```

## Quick Start Example

Here's a simple "Hello, World!" program to get you started:

```stratos
package main;

fn main() {
    print("Hello, Stratos!");
}
```

::: tip
All examples in this section are based on the test cases from the Stratos interpreter, so they represent real, tested code that works with the current version.
:::
