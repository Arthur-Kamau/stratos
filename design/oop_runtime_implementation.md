# OOP Runtime Implementation

## Overview

The Stratos interpreter now has full OOP support with runtime object allocation, constructor execution, method dispatch, and field access.

## Implementation Details

### 1. Struct Generation

Classes are compiled to LLVM struct types:

```llvm
%struct.Rectangle = type { double, double }
```

- Each field becomes a member of the struct
- Fields are indexed from 0
- Empty classes have a dummy i8 field

### 2. Object Allocation & Constructors

Constructors allocate memory and initialize fields:

**Stratos Code:**
```stratos
class Rectangle {
    var width: double;
    var height: double;

    constructor(w: double, h: double) {
        this.width = w;
        this.height = h;
    }
}

val rect = Rectangle(10.0, 5.0);
```

**Generated LLVM IR:**
```llvm
define %struct.Rectangle* @Rectangle(double %arg0, double %arg1) {
  %t0 = call i8* @malloc(i64 16)          ; Allocate memory
  %t1 = bitcast i8* %t0 to %struct.Rectangle*
  ; ... constructor body executes here ...
  ret %struct.Rectangle* %t1               ; Return object pointer
}
```

### 3. Method Dispatch

Methods receive `this` pointer as first parameter:

**Stratos Code:**
```stratos
fn area() double {
    return this.width * this.height;
}
```

**Generated LLVM IR:**
```llvm
define double @Rectangle_area(%struct.Rectangle* %this_ptr) {
  %t0 = getelementptr inbounds %struct.Rectangle, %struct.Rectangle* %this_ptr, i32 0, i32 0
  %t1 = load double, double* %t0
  %t2 = getelementptr inbounds %struct.Rectangle, %struct.Rectangle* %this_ptr, i32 0, i32 1
  %t3 = load double, double* %t2
  %t4 = fmul double %t1, %t3
  ret double %t4
}
```

**Method Call:**
```llvm
%result = call double @Rectangle_area(%struct.Rectangle* %rect_ptr)
```

### 4. Field Access

Direct field access via `getelementptr`:

**Stratos Code:**
```stratos
val w = rect.width;
```

**Generated LLVM IR:**
```llvm
%field_ptr = getelementptr inbounds %struct.Rectangle, %struct.Rectangle* %rect, i32 0, i32 0
%w = load double, double* %field_ptr
```

### 5. `this` Pointer Handling

The `this` pointer is treated specially in code generation:

- In methods, `this` is passed as a direct parameter (not allocated on stack)
- No load instruction needed when accessing `this`
- Field access via `this.field` uses direct getelementptr

## Class Metadata Tracking

The IR Generator maintains class information:

```cpp
struct ClassInfo {
    std::string name;
    std::vector<FieldInfo> fields;
    std::vector<MethodInfo> methods;
    std::string superclass;
    bool isInterface;
};
```

This metadata enables:
- Struct type generation
- Field index lookup
- Method signature generation
- Constructor generation

## Features Implemented

✅ **Class Declarations**
- Field declarations
- Method declarations
- Constructor declarations

✅ **Object Creation**
- Constructor calls with parameters
- Memory allocation via `malloc`
- Field initialization

✅ **Method Calls**
- Instance method invocation
- `this` pointer passing
- Return values

✅ **Field Access**
- Reading fields (`obj.field`)
- Writing fields in constructors

✅ **Interfaces**
- Interface declarations (parsed but not enforced)
- Multiple interfaces per class

## Current Limitations

1. **No Inheritance Implementation**
   - Superclass fields not included in structs
   - Virtual method dispatch not implemented
   - `super` keyword not supported

2. **No Field Assignment Outside Constructors**
   - Can read fields anywhere
   - Can only write fields in constructor body

3. **No Dynamic Dispatch**
   - Method calls are statically resolved
   - No virtual method tables

4. **No Memory Management**
   - Objects allocated with `malloc`
   - No automatic garbage collection
   - No destructors

5. **Simplified Size Calculation**
   - Uses field_count * 8 bytes
   - Should use proper LLVM type size calculation

## Example: Complete OOP Usage

**Input (Stratos):**
```stratos
class Rectangle {
    var width: double;
    var height: double;

    constructor(w: double, h: double) {
        this.width = w;
        this.height = h;
    }

    fn area() double {
        return this.width * this.height;
    }

    fn perimeter() double {
        return 2.0 * (this.width + this.height);
    }
}

val rect = Rectangle(10.0, 5.0);
print(rect.area());        // 50.0
print(rect.perimeter());   // 30.0
```

**Output (LLVM IR):**
```llvm
%struct.Rectangle = type { double, double }

define %struct.Rectangle* @Rectangle(double %arg0, double %arg1) {
  %t0 = call i8* @malloc(i64 16)
  %t1 = bitcast i8* %t0 to %struct.Rectangle*
  ; Initialize width and height from parameters
  ret %struct.Rectangle* %t1
}

define double @Rectangle_area(%struct.Rectangle* %this_ptr) {
  ; Load width and height, multiply, return
}

define double @Rectangle_perimeter(%struct.Rectangle* %this_ptr) {
  ; Load width and height, calculate perimeter, return
}
```

## Technical Notes

### Why Methods Need %this_ptr Parameter

Unlike languages with implicit `this`, LLVM requires explicit passing of object pointers:

```llvm
; Correct - this_ptr is a parameter
define double @Rectangle_area(%struct.Rectangle* %this_ptr) {
  ; Use %this_ptr directly
}

; Wrong - would need to allocate and load
define double @Rectangle_area() {
  ; No access to object!
}
```

### Field Index Calculation

Fields are indexed from 0 in declaration order:

```stratos
class Point {
    var x: double;  // index 0
    var y: double;  // index 1
    var z: double;  // index 2
}
```

```llvm
; Access y field (index 1)
%y_ptr = getelementptr inbounds %struct.Point, %struct.Point* %point, i32 0, i32 1
```

## Future Enhancements

1. **Virtual Method Dispatch**
   - Add vtable to struct
   - Generate vtable initialization
   - Implement virtual method calls

2. **Inheritance**
   - Include parent fields in struct
   - Support `super` calls
   - Method overriding with virtual dispatch

3. **Smart Pointers**
   - Reference counting
   - Automatic memory management

4. **Access Modifiers**
   - Private/public fields
   - Protected methods
   - Visibility enforcement

5. **Static Members**
   - Class-level fields
   - Static methods
   - Initialization logic
