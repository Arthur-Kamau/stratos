# Memory Management

Stratos provides flexible memory management options, allowing you to choose between automatic garbage collection (GC) and manual memory management depending on your application's needs.

## Overview

By default, Stratos uses a **mark-and-sweep garbage collector** that automatically manages memory allocation and deallocation. This works well for most applications but may not be suitable for:

- Systems programming where predictable performance is critical
- Real-time applications that cannot tolerate GC pauses
- Embedded systems with limited memory
- Performance-critical code paths

For these use cases, Stratos offers **manual memory management** as an alternative.

## Configuration

Memory management mode is configured in `stratos.conf`:

```hocon
memory {
  gc = true           # Enable garbage collection (default)
  allow_manual = false # Allow manual memory alongside GC
}
```

### Available Modes

| Mode | gc | allow_manual | Description |
|------|-----|--------------|-------------|
| GC Only | `true` | `false` | Default mode. All memory is garbage collected. |
| Manual Only | `false` | `false` | No GC. All memory must be manually managed. |
| Mixed | `true` | `true` | GC enabled with optional manual management. |

## GC Mode (Default)

In GC mode, objects are allocated on the heap and automatically collected when no longer referenced:

```stratos
fn main() {
    val person = Person("Alice", 30);  // GC-managed
    println(person.name);
    // No need to free - GC handles cleanup
}
```

This is the simplest mode and recommended for most applications.

## Manual Memory Management

When GC is disabled (`gc = false`), you must use explicit memory management syntax.

### Pointer Types

Stratos provides four pointer types for manual memory management:

#### `own<T>` - Unique Ownership

Exclusive ownership pointer. Only one `own<T>` can point to an object at a time.

```stratos
val ptr: own<Person> = new Person("Alice", 30);
println(ptr->name);
delete ptr;  // Must explicitly free
```

#### `ref<T>` - Shared Ownership

Reference-counted pointer. Multiple `ref<T>` can share ownership.

```stratos
val shared: ref<Person> = new ref Person("Bob", 25);
val copy = shared;  // ref count = 2
println(shared.useCount());  // 2
// Automatically freed when ref count reaches 0
```

#### `weak<T>` - Weak Reference

Non-owning reference. Does not prevent deallocation.

```stratos
val shared: ref<Person> = new ref Person("Charlie", 35);
val weak: weak<Person> = shared;

// weak becomes null if shared is deallocated
if (weak != null) {
    println(weak->name);
}
```

#### `stack<T>` - Stack Allocation

Allocates on the stack. Automatically cleaned up when scope ends.

```stratos
fn process() {
    val local: stack<Point> = Point(10, 20);
    println(local.x);
    // Automatically cleaned up at end of scope
}
```

### Memory Operations

```stratos
// Allocation
val ptr = new Person("David", 40);  // Heap allocation

// Deallocation
delete ptr;  // Explicit cleanup

// Dereference
println(ptr->name);  // Arrow syntax for pointers

// Null check
if (ptr != null) {
    println("Valid pointer");
}
```

### Destructors

Classes can define destructors that are called when `delete` is invoked:

```stratos
class FileHandle {
    val fd: int;

    constructor(path: string) {
        this.fd = open(path);
    }

    ~FileHandle() {
        close(this.fd);
        println("File closed");
    }
}

fn main() {
    val file = new FileHandle("/tmp/data.txt");
    // ... use file ...
    delete file;  // Calls ~FileHandle()
}
```

## GC Syntax Errors

When GC is disabled and you use GC-style syntax, Stratos provides helpful error messages:

```stratos
// stratos.conf: memory { gc = false }

fn main() {
    val person = Person("Alice", 30);  // Error!
}
```

**Error output:**
```
[Error] main.st:4:18: GC-style object instantiation is not allowed when GC is disabled.
       |
       = help: Use 'new' for heap allocation:
              val obj: own<Person> = new Person(...);
       |
       = note: GC is disabled in stratos.conf. Use manual memory management syntax.
       = see:  https://stratos-lang.org/guide/memory-management
```

## Mixed Mode

Enable both GC and manual memory management:

```hocon
memory {
  gc = true
  allow_manual = true
}
```

In mixed mode, you can use both styles:

```stratos
fn main() {
    // GC-managed (dot syntax)
    val gcObj = Person("Alice", 30);
    println(gcObj.name);

    // Manually managed (arrow syntax)
    val manualObj = new Person("Bob", 25);
    println(manualObj->name);
    delete manualObj;
}
```

## Best Practices

### When to Use GC Mode
- General application development
- Rapid prototyping
- When simplicity is preferred over control

### When to Use Manual Mode
- Systems programming
- Real-time applications
- Game engines
- Embedded systems
- Performance-critical code

### Memory Safety Tips

1. **Always pair `new` with `delete`** - Every heap allocation needs explicit cleanup
2. **Use `ref<T>` for shared ownership** - Avoids manual reference counting
3. **Use `weak<T>` to break cycles** - Prevents memory leaks from circular references
4. **Prefer `stack<T>` for short-lived objects** - Automatic cleanup, better cache locality
5. **Use RAII patterns** - Put cleanup logic in destructors

## Standard Library Support

The `mem` module provides low-level memory operations:

```stratos
use mem;

// Raw allocation
val buffer = mem.alloc(1024);

// Memory operations
mem.copy(dest, src, size);
mem.move(dest, src, size);

// Raw deallocation
mem.free(buffer);

// Type information
val size = mem.size_of<Person>();
val align = mem.align_of<Person>();
```

## Migration Guide

### From GC to Manual

**Before (GC):**
```stratos
fn main() {
    val person = Person("Henry", 50);
    println(person.name);
}
```

**After (Manual):**
```stratos
fn main() {
    val ptr = new Person("Henry", 50);
    println(ptr->name);
    delete ptr;
}
```

### Key Differences

| Aspect | GC Mode | Manual Mode |
|--------|---------|-------------|
| Allocation | `Person(...)` | `new Person(...)` |
| Access | `obj.field` | `ptr->field` |
| Deallocation | Automatic | `delete ptr` |
| Type annotation | `Person` | `own<Person>` |

## See Also

- [Error Handling](error-handling.md) - Exception safety with manual memory
- [OOP](oop.md) - Classes and destructors
- [Testing](testing.md) - Memory leak detection in tests
