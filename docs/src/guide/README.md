# Introduction

Welcome to Stratos, a modern, statically-typed programming language designed for building reliable and efficient software. With a focus on safety, expressiveness, and developer productivity, Stratos combines the best features of modern programming languages.

## Key Features

### Type Safety
Strong static typing with type inference prevents runtime errors and improves code reliability. The type system catches errors at compile time, making your code more robust.

### Native Async
Built-in async/await support for writing concurrent code that's easy to read and maintain. No callback hell, just clean asynchronous code.

### Package Management
Go-like dependency management with transitive resolution and lock files for reproducible builds. Managing dependencies is simple and straightforward.

### Expressive Syntax
Clean, readable syntax with modern features like:
- Pipe operators for function composition
- Pattern matching for control flow
- Null safety to prevent null pointer errors
- First-class functions and closures

## Language Highlights

### Null Safety

Never worry about null pointer exceptions again:

```stratos
val name: Option<string> = getUserName();

match (name) {
    Some(n) -> print(n)  // Type-safe access
    None -> print("No name")
}

// Safe navigation with Option
val length = name.map((n) => n.length()).unwrapOr(0);
```

### Pattern Matching

Powerful pattern matching for handling complex control flow:

```stratos
val grade = when (score) {
    90..=100 -> "A"
    80..=89 -> "B"
    70..=79 -> "C"
    60..=69 -> "D"
    else -> "F"
};
```

### Object-Oriented Programming

Full support for classes, interfaces, and inheritance:

```stratos
interface Shape {
    fn area() double;
}

class Circle : Shape {
    var radius: double;

    constructor(r: double) {
        this.radius = r;
    }

    fn area() double {
        return 3.14159 * this.radius * this.radius;
    }
}
```

### Async/Await

Write asynchronous code that looks synchronous:

```stratos
async fn fetchData(url: string) Future<string>{
    val response = await http.get(url);
    return await response.text();
}

async fn main() {
    val data = await fetchData("https://api.example.com");
    print(data);
}
```

## Why Choose Stratos?

- **Modern Design**: Benefits from decades of programming language evolution
- **Developer-Friendly**: Helpful error messages and great tooling
- **High Performance**: Compiles to efficient native code via LLVM
- **Rich Standard Library**: Batteries included for common tasks
- **Easy Dependencies**: Simple and powerful package management
- **Growing Community**: Active development with community input

## Quick Start Example

Here's a simple "Hello, World!" program to get you started:

```stratos
package main;

fn main() {
    print("Hello, Stratos!");
}
```

## Running Code Examples

All examples can be run directly with the Stratos interpreter:

```bash
# Save example to a file
echo 'package main; fn main() { print("Hello!"); }' > example.st

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

## Example Categories

Learn Stratos through practical, runnable examples. Each section demonstrates key language features with complete, working code.

### [Basics](/examples/basics)
Variables, types, and basic operations
- Variable declarations (`val` and `var`)
- Primitive types (int, double, string, bool)
- Arithmetic and logical operators
- Type conversions and casting

### [Control Flow](/examples/control-flow)
If statements, loops, and pattern matching
- If expressions and ternary operators
- While and for loops
- Pattern matching with `when`
- Guards and multiple conditions

### [Functions](/examples/functions)
Function declarations, lambdas, and pipe operators
- Basic function syntax
- Lambda expressions with `=>`
- Higher-order functions
- Pipe operator for function composition

### [Null Safety](/examples/null-safety)
Optional types and safe navigation
- `Option<T>` type with `Some` and `None`
- Pattern matching with Option
- Safe navigation and unwrapping

### [Object-Oriented](/examples/oop)
Classes, interfaces, and inheritance
- Class definitions and constructors
- Interfaces and implementation
- Inheritance with `super`
- Encapsulation with private members

### [Packages](/examples/packages)
Module system and imports
- Package declarations
- Importing and using modules
- Dependency management
- Multi-file projects

### [Asynchronous Programming](/examples/async)
Futures, async/await, and concurrent operations
- Async functions and Futures
- Using `await` keyword
- Concurrent operations with `async.all()`
- Error handling in async code

## Next Steps

- [Getting Started](/guide/getting-started) - Install Stratos and create your first program
- [Language Basics](/guide/basics) - Learn the fundamentals
- [Functions](/guide/functions) - Master function declarations and lambdas
- [Control Flow](/guide/control-flow) - Conditionals and loops
- [Object-Oriented Programming](/guide/oop) - Classes and interfaces
- [Asynchronous Programming](/guide/async) - Async/await and Futures
- [Testing](/guide/testing) - Write tests for your code
- [CLI Reference](/reference/cli) - Master the command-line tools
- [Standard Library](/reference/stdlib) - Explore built-in modules

::: tip Tested Examples
All examples in this documentation are based on the test cases from the Stratos interpreter, so they represent real, tested code that works with the current version.
:::

::: tip Active Development
Stratos is under active development. Some features may change as we work towards version 1.0. Check the changelog for the latest updates.
:::

## Learning Path

Here's a recommended path for learning Stratos:

1. **Start with Basics** - Understand variables, types, and basic operations
2. **Learn Control Flow** - Master conditionals, loops, and pattern matching
3. **Explore Functions** - Function definitions, lambdas, and composition
4. **Dive into OOP** - Classes, interfaces, and inheritance
5. **Master Async** - Asynchronous programming with Futures
6. **Write Tests** - Test your code with the built-in testing framework
7. **Organize Code** - Packages and module system
8. **Build Projects** - Apply your knowledge to real applications

## Community and Support

- **Documentation**: Comprehensive guides and API references
- **Examples**: Practical code examples for every feature
- **GitHub**: Report issues and contribute to development
- **Discord**: Join the community for help and discussions

Start your journey with Stratos today and experience modern, safe, and expressive programming!