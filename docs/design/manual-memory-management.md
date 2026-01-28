# Manual Memory Management for Stratos

## Overview

This proposal introduces manual memory management for Stratos, providing an alternative to the existing garbage collector (GC). The goal is to offer C++-style manual memory management with smart pointer-like behavior, making it simpler than Rust lifetimes but more explicit than automatic GC.

## Motivation

- **Performance**: Manual memory management can provide predictable performance by eliminating GC pauses
- **Predictability**: Developers have full control over when objects are allocated and deallocated
- **Low-level Control**: Enables Stratos to be used for systems programming and embedded applications
- **Learning Curve**: Simpler than Rust lifetimes but more explicit than GC

## Design Principles

1. **Explicit Allocation/Deallocation**: Memory must be explicitly allocated and deallocated
2. **Smart Pointer-like Behavior**: Provide automatic cleanup through RAII-style mechanisms
3. **Safety First**: Prevent common issues like double free and use-after-free (as much as possible)
4. **Optional GC**: Allow GC to be disabled entirely for performance-critical code
5. **Compatibility**: Existing GC-based code should continue to work

## Syntax Proposals

### 1. Memory Allocation with `new` Keyword

```stratos
// Allocate a single object
val ptr: own<Person> = new Person("Alice", 30);
```

### 2. Memory Deallocation with `delete` Keyword

```stratos
// Explicit deallocation
delete ptr;
```

### 3. Smart Pointer Types

#### `own<T>` - Unique Pointer (Exclusive Ownership)

```stratos
fn createPerson() own<Person> {
    val person = new Person("Bob", 25);
    return person; // Transfer ownership
}

fn main() {
    val ptr = createPerson();
    println(ptr->name); // Access through pointer
    
    delete ptr; // Required to avoid memory leak
}
```

#### `ref<T>` - Shared Pointer (Reference Counted)

```stratos
fn main() {
    val ptr: ref<Person> = new ref Person("Charlie", 35);
    val ptr2 = ptr; // Reference count becomes 2
    
    println(ptr->name);
    println(ptr2->age);
    
    // No need to delete - automatic when reference count reaches 0
}
```

#### `weak<T>` - Weak Pointer (Non-owning Reference)

```stratos
fn main() {
    val strong = new ref Person("Dave", 40);
    val weakRef: weak<Person> = strong;
    
    if (weakRef) {
        println(weakRef->name);
    }
    
    // If strong is deleted, weakRef becomes null
}
```

### 4. Stack Allocation with `stack` Keyword

```stratos
fn main() {
    val person: stack <Person> = Person("Eve", 28);
    println(person.name);
    
    // No deletion needed - automatically deallocated when scope ends
}
```

### 5. Array Allocation

```stratos
fn main() {
    // Allocate array on heap
    val arr: dfer<Array<int>> = new Array<int>(10);
    
    // Initialize elements
    for (i in 0..9) {
        arr[i] = i * 2;
    }
    
    // Deallocate array
    delete arr;
}
```

### 6. Custom Deleters

```stratos
fn customDelete(ptr: dfer<Person>) {
    println("Deleting person: " + ptr->name);
    delete ptr;
}

fn main() {
    val ptr: dfer<Person> = new Person("Frank", 33);
    customDelete(ptr);
}
```

## Implementation Details

### Memory Management Modes

Stratos programs can run in two modes:

1. **GC Mode (Default)**: Existing behavior with garbage collection
2. **GC-Free Mode**: Manual memory management only

Mode is specified via compiler flag:
```bash
# GC mode (default)
stratos compile program.st

# GC-free mode (manual memory management)
stratos compile --no-gc program.st
```

### Pointer Operations

```stratos
fn main() {
    val ptr = new Person("Grace", 45);
    
    // Dereference
    val name = ptr->name;
    
    // Null check
    if (ptr != null) {
        println(name);
    }
    
    // Pointer arithmetic (for arrays)
    val arr = new Array<int>(5);
    val secondElement = arr[1];
    
    delete arr;
    delete ptr;
}
```



### Classes for Manual Memory Management

```stratos
class Person {
    val name: string;
    val age: int;
    
    constructor(name: string, age: int) {
        this.name = name;
        this.age = age;
        println("Person constructed: " + name);
    }
    
    // Destructor (called when delete is invoked)
    ~Person() {
        println("Person destructed: " + this.name);
    }
}

fn main() {
    val ptr = new Person("Isabel", 38);
    delete ptr; // Calls ~Person()
}
```

## Migration Path

### Existing Code

Existing GC-based code continues to work in GC mode. To use manual memory management, code must be refactored:

```stratos
// Before (GC-based)
fn main() {
    val person = Person("Jack", 25);
    println(person.name);
}

// After (GC-free)
fn main() {
    val ptr = new Person("Jack", 25);
    println(ptr->name);
    delete ptr;
}
```

### Compatibility Flags

```bash
# Compile with GC (default)
stratos compile --gc program.st

# Compile without GC (manual memory management)
stratos compile --no-gc program.st

# Compile with GC but allow manual memory management
stratos compile --gc --allow-manual program.st
```

## Type System Changes

### New Types

1. `own<T>` - Unique ownership pointer
2. `ref<T>` - Shared ownership pointer (reference counted)
3. `weak<T>` - Non-owning weak pointer
4. `stack<T>` - Stack-allocated type

### Type Inference

```stratos
fn main() {
    // Type inferred
    val ptr = new Person("Kate", 42); // dfer<Person>
    
    // Explicit type
    val shared: ref<Person> = new ref Person("Leo", 35);
}
```

## Benefits

1. **Performance**: Eliminates GC pauses and overhead
2. **Predictability**: Exact control over memory usage
3. **Safety**: RAII-style cleanup reduces leaks
4. **Simplicity**: Easier to learn than Rust lifetimes
5. **Compatibility**: Existing code continues to work
6. **Systems Programming**: Enables Stratos to be used for low-level applications

## Drawbacks

1. **Increased Complexity**: Requires manual memory management
2. **Potential Leaks**: Risk of memory leaks if delete is forgotten
3. **Use-after-free**: Risk of accessing deallocated memory
4. **Learning Curve**: New concepts to learn

## Conclusion

This proposal provides a balanced approach to manual memory management for Stratos. By introducing smart pointer-like types and RAII-style destructors, it simplifies memory management compared to raw C++ pointers while maintaining the performance benefits of manual control. The optional GC mode ensures compatibility with existing code, making migration incremental and straightforward.

The design aims to provide predictable performance for systems programming while keeping the language accessible to developers familiar with C++ or similar languages.
