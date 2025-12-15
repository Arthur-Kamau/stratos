# Packages

Packages organize code into logical modules, similar to Go's package system. Each directory represents a package, and files in that directory belong to that package.

## Purpose

- **Code organization**: Group related functionality together
- **Name collision prevention**: Avoid conflicts between identically named items
- **Module system**: Create clear boundaries between different parts of your codebase
- **Reusability**: Share code across projects

## Package Basics

### Package Declaration

Every Stratos source file starts with a `package` declaration:

```stratos
package main

fn main() {
    print("Hello, Stratos!");
}
```

### Package Rules

- **One package per directory**: All `.st` files in a directory must declare the same package
- **Package name = directory name**: The package name should match the directory name
- **Package-level scope**: Functions, classes, and types in the same package can access each other

## Directory Structure

```
myproject/
├── main.st              # package main
├── utils/
│   ├── math.st         # package utils
│   └── string.st       # package utils
└── models/
    └── user.st         # package models
```

### main.st
```stratos
package main

use utils;
use models;

fn main() {
    val sum = utils.add(5, 10);
    val user = models.User { name: "Alice" };
}
```

### utils/math.st
```stratos
package utils

fn add(a: int, b: int) int {
    return a + b;
}
```

## Importing Packages

Use the `use` keyword to import packages:

```stratos
package main

use utils;
use models;

fn main() {
    val result = utils.add(10, 20);
}
```

## Special Package: `main`

The `main` package is the entry point for executable programs:

```stratos
package main

fn main() {
    // Program starts here
}
```

## What Packages Can Contain

- ✅ Functions
- ✅ Classes
- ✅ Structs
- ✅ Enums
- ✅ Interfaces

Packages CANNOT:
- ❌ Be instantiated
- ❌ Have constructors  
- ❌ Inherit from other packages

## Summary

Packages in Stratos work like Go:
- **One directory = One package**
- **Package declaration at top of each file**
- **Use `use` keyword for imports**
- **Flat, directory-based organization**
