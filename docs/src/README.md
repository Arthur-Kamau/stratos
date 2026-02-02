---
home: true
heroImage: /images/logo.svg
heroText: Stratos Programming Language
tagline: Modern, safe, and expressive programming language with powerful type system and native async support
actions:
  - text: Get Started
    link: /guide/getting-started
    type: primary
  - text: View on GitHub
    link: https://github.com/Arthur-Kamau/stratos
    type: secondary
features:
  - title: Type Safety
    details: Strong static typing with type inference prevents runtime errors and improves code reliability
  - title: Concurrency
    details: Go-style goroutines, channels, and async/await for powerful concurrent programming
  - title: Package Management
    details: Go-like dependency management with transitive resolution and lock files for reproducible builds
  - title: Expressive Syntax
    details: Clean, readable syntax with string interpolation, pattern matching, defer statements, and null safety
  - title: Performance
    details: Compiled to efficient native code or LLVM IR for optimal performance
  - title: Standard Library
    details: Comprehensive standard library with 100+ native functions covering common programming needs
footer: MIT Licensed | Copyright © 2024 Stratos Programming Language
---

## Quick Example

```stratos
package main;

use math;
use log;

fn factorial(n: int) int {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

fn main() {
    val numbers = [1, 2, 3, 4, 5];

    for val num in numbers {
        val result = factorial(num);
        log.info("Factorial of $num is $result");
    }

    // Pipe operator for function composition
    val result = 5 |> square() |> double();
    print("Result: $result");
}
```

## Why Stratos?

- **Modern Language Design**: Learn from decades of language evolution
- **Developer Experience**: Great tooling with helpful error messages
- **Performance**: Compiled to efficient native code or LLVM IR
- **Standard Library**: Comprehensive stdlib covering common use cases
- **Package Management**: Dependency system inspired by Go modules
- **Active Development**: Continuously evolving with community feedback

## Getting Started

Install Stratos:

```bash
curl -sSL https://get.stratos-lang.org | sh
```

Create your first program:

```stratos
package main;

fn main() {
    print("Hello, Stratos!");
}
```

Run it:

```bash
stratos run hello.st
```

## Community

- 💬 [Discord Server](https://discord.gg/stratos) - Chat with other developers
- 📝 [GitHub Repository](https://github.com/Arthur-Kamau/stratos) - Contribute to the project
- 🐦 [Twitter](https://twitter.com/stratoslang) - Stay updated with the latest news
