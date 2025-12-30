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
val name: string? = getUserName();

if (name != null) {
    print(name);  // Type-safe access
}

// Safe navigation
val length = name?.length() ?? 0;
```

### Pattern Matching

Powerful pattern matching for handling complex control flow:

```stratos
match value {
    0 => print("Zero"),
    1..10 => print("Small"),
    11..100 => print("Medium"),
    _ => print("Large")
}
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
        return math.PI * this.radius * this.radius;
    }
}
```

### Async/Await

Write asynchronous code that looks synchronous:

```stratos
async fn fetchData(url: string) string {
    val response = await http.get(url);
    return await response.text();
}

fn main() async {
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

## Next Steps

- [Getting Started](/guide/getting-started) - Install Stratos and create your first program
- [Language Basics](/guide/basics) - Learn the fundamentals
- [Examples](/examples/) - See practical code examples
- [CLI Reference](/reference/cli) - Master the command-line tools
- [Standard Library](/reference/stdlib) - Explore built-in modules

::: tip
Stratos is under active development. Some features may change as we work towards version 1.0.
:::
