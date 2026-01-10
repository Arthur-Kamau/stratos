---
title: Packages and Modules
description: Module system and code organization in Stratos
---

# Packages and Modules

Learn how to organize your Stratos code using packages and the module system. Stratos uses a straightforward approach to code organization inspired by modern languages.

## Package Declaration

Every Stratos file should declare its package:

```stratos
package main;

fn main() {
    print("Hello from main package");
}
```

### Package Naming

```stratos
// Single-level package
package utils;

// Multi-level package (namespaced)
package com.example.utils;

// Project-specific package
package myproject.models;
```

::: info Package Convention
Use lowercase names. For multi-level packages, use dot notation similar to Java or Go.
:::

## Importing Packages

### Using Standard Library

```stratos
package main;

use math;
use strings;
use log;

fn main() {
    val result = math.sqrt(16.0);
    val text = strings.toUpper("hello");

    log.info("Result: " + result);
    log.info("Text: " + text);
}
```

### Importing Custom Packages

```stratos
// File: src/utils/mathutils.st
package utils;

fn square(x: int) int {
    return x * x;
}

fn cube(x: int) int {
    return x * x * x;
}
```

```stratos
// File: src/main.st
package main;

use utils;  // Import the utils package

fn main() {
    val squared = utils.square(5);
    val cubed = utils.cube(3);

    print("5 squared: " + squared);
    print("3 cubed: " + cubed);
}
```

## Module Structure

### Single-File Module

```stratos
// File: geometry.st
package geometry;

use math;

class Point {
    var x: double;
    var y: double;

    constructor(x: double, y: double) {
        this.x = x;
        this.y = y;
    }

    fn distance(other: Point) double {
        val dx = this.x - other.x;
        val dy = this.y - other.y;
        return math.sqrt(dx * dx + dy * dy);
    }
}

fn createPoint(x: double, y: double) Point {
    return Point(x, y);
}
```

### Multi-File Package

```
myproject/
├── src/
│   ├── main.st
│   ├── models/
│   │   ├── user.st
│   │   └── product.st
│   └── utils/
│       ├── validation.st
│       └── formatting.st
└── stratos.conf
```

```stratos
// File: src/models/user.st
package models;

class User {
    var id: int;
    var name: string;
    var email: string;

    constructor(id: int, name: string, email: string) {
        this.id = id;
        this.name = name;
        this.email = email;
    }

    fn getDisplayName() string {
        return this.name + " <" + this.email + ">";
    }
}
```

```stratos
// File: src/main.st
package main;

use models;  // Import the models package

fn main() {
    val user = models.User(1, "Alice", "alice@example.com");
    print(user.getDisplayName());
}
```

## Using External Dependencies

With Stratos's dependency management system, you can use external packages:

### Add Dependency to stratos.conf

```hocon
project {
  name = my-app
  version = "1.0.0"
}

build {
  entry = src/main.st
  output = build/my-app
}

dependencies = [
  {
    name = http
    url = "https://github.com/stratos-lang/http"
    tag = "v2.1.0"
  }
  {
    name = json
    url = "https://github.com/stratos-lang/json"
    tag = "v1.5.0"
  }
]
```

### Fetch Dependencies

```bash
stratos get
```

### Use External Package

```stratos
package main;

use http;
use encoding;
use log;

async fn main() {
    // Fetch data from API
    val response = await http.get("https://api.github.com/users/octocat");

    if (response.status() == 200) {
        val data = encoding.json.decode(response.text());

        log.info("User: " + data.login);
        log.info("Name: " + data.name);
        log.info("Repos: " + data.public_repos);
    } else {
        log.error("Request failed");
    }
}
```

::: tip Dependency Management
Run `stratos get` after adding dependencies to download them. Dependencies are stored in the `deps/` directory by default.
:::

## Package Exports

Define what a package exports in its main file:

```stratos
// File: utils/init.st (package entry point)
package utils;

// Public functions - exported
fn add(a: int, b: int) int {
    return a + b;
}

fn multiply(a: int, b: int) int {
    return a * b;
}

// Private helper - not exported
private fn validate(x: int) bool {
    return x >= 0;
}

// Public class - exported
class Calculator {
    fn calculate(a: int, b: int, op: string) int {
        return when (op) {
            "+" -> add(a, b)
            "*" -> multiply(a, b)
            else -> 0
        };
    }
}
```

::: info Visibility Rules
- **Top-Level Items:** Functions and classes declared at the top level of a file are **public by default**. They are exported and can be used by other files that import the package. You can use the `private` keyword to make them internal to the package.
- **Class Members:** In contrast, methods and fields defined within a class are **private by default**. You must explicitly use the `pub` keyword to make them accessible from outside the class.
:::

## Selective Imports (Future Feature)

Future versions of Stratos will support selective imports:

```stratos
// Import specific items
use math.{sin, cos, PI};
use strings.{trim, split, join};

fn main() {
    // Use directly without package prefix
    val angle = PI / 2.0;
    val result = sin(angle);

    val text = "  hello  ";
    val cleaned = trim(text);
}
```

## Package Organization Best Practices

### Recommended Structure

```
my-project/
├── stratos.conf          # Project configuration
├── src/
│   ├── main.st          # Entry point
│   ├── models/          # Data models
│   │   ├── init.st
│   │   ├── user.st
│   │   └── product.st
│   ├── services/        # Business logic
│   │   ├── init.st
│   │   ├── auth.st
│   │   └── database.st
│   ├── utils/           # Utilities
│   │   ├── init.st
│   │   ├── validation.st
│   │   └── formatting.st
│   └── handlers/        # Request handlers
│       ├── init.st
│       └── api.st
├── tests/               # Test files
│   ├── models_test.st
│   └── services_test.st
└── deps/                # Downloaded dependencies
```

### init.st Convention

The `init.st` file serves as the entry point for a package:

```stratos
// File: src/models/init.st
package models;

// Re-export classes from other files in the package
export User from "user.st";
export Product from "product.st";
export Order from "order.st";

// Package-level functions
fn createSampleData() Array<User> {
    return [
        User(1, "Alice", "alice@example.com"),
        User(2, "Bob", "bob@example.com")
    ];
}
```

::: tip Use init.st
Create an `init.st` file as the entry point for each package to control what gets exported and provide a clear API surface.
:::

## Complete Example: Multi-Package Project

### Project Structure

```
calculator-app/
├── stratos.conf
├── src/
│   ├── main.st
│   ├── operations/
│   │   └── basic.st
│   └── utils/
│       └── formatting.st
```

### operations/basic.st

```stratos
package operations;

fn add(a: double, b: double) double {
    return a + b;
}

fn subtract(a: double, b: double) double {
    return a - b;
}

fn multiply(a: double, b: double) double {
    return a * b;
}

fn divide(a: double, b: double) Result<double, Error> {
    if (b == 0.0) {
        return Err(Error("Division by zero"));
    }
    return Ok(a / b);
}

class Calculator {
    fn execute(a: double, b: double, op: string) Result<double, Error> {
        return when (op) {
            "+" -> Ok(add(a, b))
            "-" -> Ok(subtract(a, b))
            "*" -> Ok(multiply(a, b))
            "/" -> divide(a, b)
            else -> Err(Error("Unknown operation: " + op))
        };
    }
}
```

### utils/formatting.st

```stratos
package utils;

fn formatResult(operation: string, a: double, b: double, result: double) string {
    return a + " " + operation + " " + b + " = " + result;
}

fn formatError(message: string) string {
    return "ERROR: " + message;
}
```

### main.st

```stratos
package main;

use operations;
use utils;
use log;

fn performCalculation(a: double, b: double, op: string) {
    val calc = operations.Calculator();

    val result = calc.execute(a, b, op);

    match (result) {
        Ok(value) -> {
            val formatted = utils.formatResult(op, a, b, value);
            log.info(formatted);
        }
        Err(error) -> {
            val errorMsg = utils.formatError(error.message);
            log.error(errorMsg);
        }
    }
}

fn main() {
    log.info("Calculator Application");
    log.info("======================");

    performCalculation(10.0, 5.0, "+");   // 10.0 + 5.0 = 15.0
    performCalculation(20.0, 4.0, "-");   // 20.0 - 4.0 = 16.0
    performCalculation(6.0, 7.0, "*");    // 6.0 * 7.0 = 42.0
    performCalculation(15.0, 3.0, "/");   // 15.0 / 3.0 = 5.0
    performCalculation(10.0, 0.0, "/");   // ERROR: Division by zero
}
```

### stratos.conf

```hocon
project {
  name = calculator-app
  version = "1.0.0"
  type = executable
}

build {
  entry = src/main.st
  source_dir = src
  output = build/calculator
}

dependencies = []
```

### Build and Run

```bash
# Navigate to project directory
cd calculator-app

# Build the project
stratos build

# Run the executable
./build/calculator
```

### Output

```
[INFO] Calculator Application
[INFO] ======================
[INFO] 10.0 + 5.0 = 15.0
[INFO] 20.0 - 4.0 = 16.0
[INFO] 6.0 * 7.0 = 42.0
[INFO] 15.0 / 3.0 = 5.0
[ERROR] ERROR: Division by zero
```

## Local vs External Dependencies

### Local Dependency

Reference packages from your local filesystem:

```hocon
dependencies = [
  {
    name = shared-utils
    url = "../shared-utils"  # Relative path
  }
  {
    name = core
    url = "/home/user/libs/core"  # Absolute path
  }
]
```

### External Dependency (Git)

Reference packages from Git repositories:

```hocon
dependencies = [
  {
    name = http
    url = "https://github.com/stratos-lang/http"
    tag = "v2.1.0"  # Specific version tag
  }
  {
    name = database
    url = "https://github.com/company/db-driver"
    branch = main  # Track a branch
  }
  {
    name = experimental
    url = "https://github.com/user/lib"
    commit = "abc123def456"  # Specific commit
  }
]
```

::: warning Version Pinning
Always use specific tags or commits for production dependencies. Tracking branches like `main` can lead to unexpected changes.
:::

## Best Practices

::: tip Organize by Feature
Group related code into packages by feature rather than by type (e.g., `user` package containing models, services, and handlers for users).
:::

::: tip Use init.st
Create an `init.st` file as the entry point for each package to control what gets exported and provide a clear package API.
:::

::: tip Minimize Dependencies
Only import what you need. Excessive dependencies can slow down compilation and create maintenance issues.
:::

::: info Package Naming
Use lowercase names without underscores. For nested packages, use dot notation (e.g., `com.example.utils`).
:::

::: warning Avoid Circular Dependencies
Two packages should not depend on each other. Refactor shared code into a separate package if needed.
:::

::: tip Keep Packages Focused
Each package should have a single, clear responsibility. If a package is growing too large, consider splitting it.
:::

::: tip Document Your Packages
Add comments at the package level explaining what the package provides and how to use it.
:::

## Package System Quick Reference

| Concept | Syntax | Purpose |
|---------|--------|---------|
| Package declaration | `package name;` | Declare file's package |
| Import package | `use packagename;` | Import external package |
| Use imported | `packagename.function()` | Call imported function |
| Package path | `package com.example.utils;` | Multi-level package |
| Private member | `private fn helper()` | Internal to package |
| Local dependency | `url = "../path"` | Reference local package |
| Git dependency | `url = "github.com/..."` | Reference remote package |
| Fetch deps | `stratos get` | Download dependencies |
| Build project | `stratos build` | Compile project |

## Common Package Patterns

### Utility Package

```stratos
package utils;

// String utilities
fn capitalize(s: string) string {
    if (s.length() == 0) {
        return s;
    }
    return s.charAt(0).toUpper() + s.substring(1);
}

// Math utilities
fn clamp(value: double, min: double, max: double) double {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Array utilities
fn unique<T>(arr: Array<T>) Array<T> {
    val seen = Set<T>();
    val result = Array<T>();

    for (item in arr) {
        if (!seen.contains(item)) {
            seen.add(item);
            result.push(item);
        }
    }

    return result;
}
```

### Service Package

```stratos
package services;

use models;
use log;

class UserService {
    private var users: Array<models.User>;

    constructor() {
        this.users = [];
    }

    fn createUser(name: string, email: string) Result<models.User, Error> {
        // Validate
        if (name.length() == 0) {
            return Err(Error("Name cannot be empty"));
        }

        // Create user
        val id = this.users.length + 1;
        val user = models.User(id, name, email);

        this.users.push(user);
        log.info("Created user: " + user.getDisplayName());

        return Ok(user);
    }

    fn findById(id: int) Option<models.User> {
        for (user in this.users) {
            if (user.id == id) {
                return Some(user);
            }
        }
        return None;
    }
}
```

### Configuration Package

```stratos
package config;

class AppConfig {
    var port: int;
    var host: string;
    var debug: bool;

    constructor(port: int, host: string, debug: bool) {
        this.port = port;
        this.host = host;
        this.debug = debug;
    }
}

// Package-level factory function (replaces static method)
fn fromEnv() AppConfig {
    val port = env.get("PORT").unwrapOr("8080").toInt();
    val host = env.get("HOST").unwrapOr("localhost");
    val debug = env.get("DEBUG").unwrapOr("false") == "true";

    return AppConfig(port, host, debug);
}
```

## Next Steps

- [Object-Oriented Programming](/guide/oop) - Learn about classes and interfaces
- [Error Handling](/guide/error-handling) - Work with Result and Option types
- [Testing](/guide/testing) - Write tests for your packages
- [Build System](/guide/build) - Advanced build configuration