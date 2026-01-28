# Manual Memory Management for Stratos - Plan & Syntax Mockup

## Overview

This document provides a **complete plan and syntax mockup** for adding manual memory management to Stratos. The goal is to create a GC-free alternative that's simpler than Rust lifetimes but more powerful than raw C++ pointers.

## Current State

- Stratos currently uses a **mark-and-sweep garbage collector (GC)**
- Objects are allocated on the heap and automatically collected when no longer referenced
- This works well for most applications but has performance overhead and unpredictable pauses

## Design Goals

1. **Simplicity**: Easier to learn than Rust lifetimes
2. **Safety**: Prevent common memory errors (double free, use-after-free)
3. **Control**: Explicit memory management for systems programming
4. **Compatibility**: Existing GC-based code continues to work
5. **Performance**: Predictable memory usage without GC pauses

## Syntax Mockup

### 1. Pointer Types

```stratos
// Unique ownership pointer (exclusive, transferable)
val ptr: own<Person> = new Person("Alice", 30);

// Shared ownership pointer (reference counted)
val shared: ref<Person> = new ref Person("Bob", 25);

// Weak reference (non-owning)
val weak: weak<Person> = shared;

// Stack allocation (automatic cleanup)
val stackObj: stack Person = Person("Charlie", 35);
```

### 2. Memory Operations

```stratos
// Allocation
val ptr = new Person("David", 40); // dfer<Person> inferred
val array = new Array<int>(10);   // dfer<Array<int>>

// Deallocation
delete ptr;
delete array;

// Dereference
println(ptr->name); // "David"
println(array[0]);  // 0

// Null check
if (ptr != null) {
    println("Valid pointer");
}

// Reference counting
val ptr2 = shared; // ref count becomes 2
println(shared.useCount()); // 2
```

### 3. Classes with Destructors

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
    val ptr = new Person("Eve", 28);
    println(ptr->name);
    
    delete ptr; // Calls ~Person() destructor
}

// Output:
// Person constructed: Eve
// Eve
// Person destructed: Eve
```

### 4. Function Return Types

```stratos
fn createPerson() own<Person> {
    val person = new Person("Frank", 33);
    return person; // Ownership transferred
}

fn main() {
    val ptr = createPerson();
    println(ptr->name);
    
    delete ptr;
}
```

### 5. Array Operations

```stratos
fn main() {
    val arr = new Array<int>(5);
    
    // Initialize array
    for (i in 0..4) {
        arr[i] = i * 2;
    }
    
    // Access elements
    println(arr[2]); // 4
    
    // Deallocate
    delete arr;
}
```

### 6. Error Handling

```stratos
fn main() {
    try {
        val ptr = new Person("Grace", 45);
        // Code that might throw
        throw Error("Something went wrong");
        delete ptr; // This won't execute
    } catch (e) {
        println("Error: " + e.message);
        // ptr is automatically deleted
    }
}
```

## Implementation Plan

### Phase 1: Foundation (Lexer/Parser)

1. **Lexer Updates**:
   - Add `new`, `delete`, `dfer`, `ref`, `weak`, `stack` keywords
   - Tokenize pointer operations (`->`)

2. **Parser Updates**:
   - Parse pointer types (`own<T>`, `ref<T>`, `weak<T>`, `stack<T>`)
   - Parse `new` and `delete` keywords
   - Parse `->` operator for pointer dereference

3. **Type System**:
   - Add pointer type handling
   - Type inference for `new` operator
   - Compatibility with existing GC types

### Phase 2: Runtime Implementation

1. **own<T> (Unique Pointer)**:
   - Exclusive ownership semantics
   - Move-only type (no copying)
   - Automatic deletion when scope ends
   - Transfer ownership via return or move assignment

2. **ref<T> (Shared Pointer)**:
   - Reference counting
   - Copyable (increments ref count)
   - Deletes object when ref count reaches 0
   - Atomic operations for thread safety

3. **weak<T> (Weak Pointer)**:
   - Non-owning reference
   - Doesn't increment ref count
   - Can become null if object is deleted
   - Prevents circular references

4. **stack<T> (Stack Allocation)**:
   - Allocates on stack
   - Automatic cleanup when scope ends
   - No need to delete
   - Limited to small objects

### Phase 3: Compiler Integration

1. **Memory Management Modes**:
   - `--gc` (default): GC enabled for existing code
   - `--no-gc`: GC disabled, manual management only
   - `--gc --allow-manual`: GC with optional manual management

2. **Code Generation**:
   - LLVM IR for GC-free mode
   - Stack allocation for `stack<T>` types
   - Heap allocation for `new` operator
   - Destructor calls for `delete` operator

3. **Static Analysis**:
   - Detect potential memory leaks
   - Check for use-after-free
   - Validate pointer operations

### Phase 4: Standard Library

1. **Memory Module**:
   - `mem.alloc()` - Raw allocation
   - `mem.free()` - Raw deallocation
   - `mem.copy()` - Memory copying
   - `mem.move()` - Memory moving

2. **Collections**:
   - `Array<T>` - GC-free array type
   - `Vector<T>` - Dynamic array
   - `HashMap<K, V>` - Hash map
   - `LinkedList<T>` - Linked list

3. **Utilities**:
   - `mem::size_of<T>()` - Get type size
   - `mem::align_of<T>()` - Get type alignment
   - `mem::ptr::null()` - Null pointer

### Phase 5: Tools & Debugging

1. **Memory Profiler**:
   - Track allocations/deallocations
   - Detect leaks
   - Report memory usage

2. **Debugger Support**:
   - Pointer inspection
   - Memory watchpoints
   - Heap visualization

## Migration Path

### Existing Code

```stratos
// Before (GC-based)
fn main() {
    val person = Person("Henry", 50);
    println(person.name);
}

// After (GC-free)
fn main() {
    val ptr = new Person("Henry", 50);
    println(ptr->name);
    delete ptr;
}
```

### Mixed Mode

```stratos
// Compile with: stratos compile --gc --allow-manual program.st
fn main() {
    // GC-managed
    val gcObj = Person("Iris", 38);
    
    // Manually managed
    val manualObj = new Person("Jack", 42);
    
    println(gcObj.name);     // Dot syntax
    println(manualObj->name); // Arrow syntax
    
    delete manualObj;
}
```

## Benefits

1. **Performance**: Eliminates GC pauses and overhead
2. **Predictability**: Exact control over memory usage
3. **Safety**: RAII-style cleanup reduces leaks
4. **Simplicity**: Easier to learn than Rust lifetimes
5. **Compatibility**: Existing code continues to work
6. **Systems Programming**: Enables Stratos to be used for low-level applications

## Challenges

1. **Learning Curve**: Developers must learn new pointer types
2. **Potential Leaks**: Risk of forgetting to delete objects
3. **Use-after-free**: Risk of accessing deallocated memory
4. **Complexity**: More complex than GC for beginners

## Timeline

| Phase | Features | Estimated Time |
|-------|----------|----------------|
| 1     | Lexer/Parser/Type System | 2-3 weeks |
| 2     | Runtime Implementation | 4-6 weeks |
| 3     | Compiler Integration | 3-4 weeks |
| 4     | Standard Library | 3-5 weeks |
| 5     | Tools & Debugging | 2-3 weeks |

## Conclusion

This plan provides a practical approach to adding manual memory management to Stratos. By introducing smart pointer-like types with RAII semantics, we strike a balance between control and safety. The optional GC mode ensures compatibility with existing code, making migration incremental.

The design simplifies memory management compared to raw C++ pointers while maintaining the performance benefits of manual control, making Stratos suitable for both high-level applications and systems programming.
