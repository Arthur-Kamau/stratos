---
title: Packages and Modules
description: Organizing code with packages in Stratos
---

# Packages and Modules

Stratos uses a package system to organize code into reusable modules.

## Package Declaration

Every Stratos file belongs to a package. This is declared at the top of the file:

```stratos
package main;
```

Root files typically belong to `package main`. Files in subdirectories must have a package name matching their directory name.

## Visibility

### Top-Level Functions
Functions declared at the top level of a file (outside of any class) are **public by default**. They are exported and can be used by other files that import the package.

```stratos
package utils;

// This function is exported automatically
fn helper() {
    print("Helping!");
}
```

### Class Members
In contrast, methods defined within a class are **private by default**. You must use the `pub` keyword to expose them.

```stratos
class Service {
    pub fn start() { ... } // Public
    fn internalInit() { ... } // Private
}
```
