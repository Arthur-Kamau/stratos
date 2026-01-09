---
title: None Safety
description: Optional types and safe value handling in Stratos
---

# None Safety

Stratos provides robust None safety features to prevent null pointer exceptions and make code more reliable. Learn how to work with optional values safely.

## The Problem with None

Traditional languages allow variables to be `None`, leading to runtime errors:

```stratos
// In languages without None safety:
val user = getUser(123);
print(user.name);  // ❌ Crashes if user is None!
```

Stratos solves this with **Optional types** that make nullability explicit in the type system.

## Optional Types

### Declaration Syntax

In Stratos, `Optional<Type>` is used to indicate that a value can be None. This makes nullability explicit and checked at compile time.

```stratos
// Non-nullable (cannot be None)
val name: string = "Alice";
// name = None;  // ❌ Compile error!

// Nullable (can be None)
val maybeName: Optional<string> = None;     // ✅ Allowed
val userName: Optional<string> = Some("Bob");   // ✅ Also allowed
```

## Creating Optional Values

### Using Optional\<T>

```stratos
// Explicit Optional type with Some wrapper
val maybeNumber: Optional<int> = Some(42);
val empty: Optional<int> = None;

// Check if value exists
when (maybeNumber) {
    Some(v) -> {
        print("Value is " + v);
    }
    None -> {
        print("No value");
    }
}
// Output: Value is 42
```

::: info
**Important**: When assigning a value to an Optional type, you must wrap it with `Some()`. When assigning None, use `None` directly.
:::

### Checking Optional Values

```stratos
val number: Optional<int> = Some(42);
val noNumber: Optional<int> = None;

// Using is_some() and is_none()
if number.is_some() {
    print("Has a value");
}

if noNumber.is_none() {
    print("No value present");
}
```

## Auto-Unwrapping Rules

Stratos automatically unwraps Optional values in safe contexts:

### 1. Guard Statements with `is_some()`

When you check if an Optional has a value using `is_some()`, inside that block the value is automatically unwrapped:

```stratos
val maybeAge: Optional<int> = Some(25);

if maybeAge.is_some() {
    // Inside this block, maybeAge is automatically treated as int
    val age: int = maybeAge;
    print("Age: " + age);  // Works! No need to unwrap manually
}
```

### 2. Pattern Matching with `when`

In pattern matching, the variable in `Some(v)` is automatically the unwrapped value:

```stratos
val maybeScore: Optional<int> = Some(95);

when (maybeScore) {
    Some(score) -> {
        // 'score' is automatically unwrapped to int
        print("Score: " + score);
        print("Grade: " + (score >= 90 ? "A" : "B"));
    }
    None -> {
        print("No score available");
    }
}
```

### 3. Safe Navigation Operator (`?.`)

The safe navigation operator does NOT unwrap - it propagates None through the chain:

```stratos
val user: Optional<User> = Some(User("Alice"));

// If any part of the chain is None, the whole expression returns None
val emailLength: Optional<int> = user?.email?.length();

// This is equivalent to:
val emailLength: Optional<int> = when (user) {
    Some(u) -> when (u.email) {
        Some(e) -> Some(e.length())
        None -> None
    }
    None -> None
};
```

## Safe Access Operators

### Safe Navigation (`?.`)

The safe navigation operator `?.` safely accesses properties/methods on Optional values. If any part of the chain is None, the entire expression returns None:

```stratos
class User {
    var name: string;
    var email: Optional<string>;

    constructor(name: string, email: Optional<string>) {
        this.name = name;
        this.email = email;
    }

    fn getEmailDomain() Optional<string> {
        // Safe navigation - returns None if email is None
        return when (this.email) {
            Some(e) -> Some(e.split("@")[1])
            None -> None
        };
    }
}

fn main() {
    val user1 = User("Alice", Some("alice@example.com"));
    val user2 = User("Bob", None);

    // Safe access returns Optional<int>
    val len1: Optional<int> = user1.email?.length();  // Some(17)
    val len2: Optional<int> = user2.email?.length();  // None

    // Chaining safe calls
    val domain1 = user1.getEmailDomain();  // Some("example.com")
    val domain2 = user2.getEmailDomain();  // None
}
```

### None Coalescing (`??`)

Provide a default value when dealing with Nones:

```stratos
val name: Optional<string> = None;

// If name is None, use "Guest"
val displayName = name ?? "Guest";
print(displayName);  // "Guest"

// With non-None value
val actualName: Optional<string> = Some("Alice");
val display = actualName ?? "Guest";
print(display);  // "Alice"
```

### Combined Usage

```stratos
class Profile {
    var bio: Optional<string>;

    constructor(bio: Optional<string>) {
        this.bio = bio;
    }
}

class User {
    var profile: Optional<Profile>;

    constructor(profile: Optional<Profile>) {
        this.profile = profile;
    }
}

fn main() {
    val user = User(Some(Profile(Some("Software Developer"))));

    // Chain safe navigation with None coalescing
    val bio = user.profile?.bio ?? "No bio available";
    print(bio);  // "Software Developer"

    val userWithoutProfile = User(None);
    val noBio = userWithoutProfile.profile?.bio ?? "No bio available";
    print(noBio);  // "No bio available"
}
```

## Pattern Matching with Optionals

### Basic Pattern Matching

```stratos
fn processOptional(value: Optional<int>) {
    when (value) {
        Some(v) -> {
            // v is automatically unwrapped to int
            print("Got value: " + v);
            print("Doubled: " + (v * 2));
        }
        None -> {
            print("No value present");
        }
    }
}

fn main() {
    processOptional(Some(42));
    // Output:
    // Got value: 42
    // Doubled: 84

    processOptional(None);
    // Output: No value present
}
```

### Pattern Matching with Conditions

```stratos
fn categorizeOptional(value: Optional<int>) string {
    return when (value) {
        Some(v) if v > 100 -> "Large value"
        Some(v) if v > 0 -> "Positive value"
        Some(0) -> "Zero"
        Some(v) -> "Negative value"
        None -> "No value"
    };
}

fn main() {
    print(categorizeOptional(Some(150)));   // "Large value"
    print(categorizeOptional(Some(50)));    // "Positive value"
    print(categorizeOptional(Some(0)));     // "Zero"
    print(categorizeOptional(Some(-10)));   // "Negative value"
    print(categorizeOptional(None));        // "No value"
}
```

::: tip
**Pattern matching with guards**: You can add conditions to patterns using `if`. The variable is still auto-unwrapped in the guard condition and the branch body.
:::

## Working with Functions

### Optional Parameters

```stratos
fn greet(name: string, title: Optional<string>) {
    val fullName = when (title) {
        Some(t) -> t + " " + name
        None -> name
    };

    print("Hello, " + fullName + "!");
}

fn main() {
    greet("Alice", Some("Dr."));  // Hello, Dr. Alice!
    greet("Bob", None);           // Hello, Bob!
}
```

### Optional Return Values

Functions can return Optional values by wrapping the result in `Some()` or returning `None`:

```stratos
fn findUser(id: int) Optional<User> {
    if id <= 0 {
        return None;
    }

    // Simulate database lookup
    // Must wrap return value in Some()
    return Some(User("User" + id));
}

fn main() {
    val user1 = findUser(123);
    if user1.is_some() {
        // user1 is auto-unwrapped inside this block
        print("Found: " + user1.name);
    } else {
        print("User not found");
    }

    val user2 = findUser(-1);
    // Using safe navigation with None coalescing
    val userName = user2?.name ?? "Anonymous";
    print(userName);  // "Anonymous"
}
```

::: info
**Return value wrapping**: When returning from a function with `Optional<T>` return type, you must use `Some(value)` to wrap non-None values, or return `None` directly.
:::

## Optional Collections

### Arrays with Optional Elements

```stratos
val numbers: Array<Optional<int>> = [Some(1), None, Some(3), None, Some(5)];

for (num in numbers) {
    val value = num ?? 0;
    print(value);
}
// Output: 1, 0, 3, 0, 5
```

### Filtering Optionals

```stratos
fn firstPositive(numbers: Array<int>) Optional<int> {
    for (num in numbers) {
        if num > 0 {
            return Some(num);
        }
    }
    return None;
}

fn main() {
    val nums1 = [-1, -2, 3, 4, 5];
    val result1 = firstPositive(nums1);
    print(result1 ?? -999);  // 3

    val nums2 = [-1, -2, -3];
    val result2 = firstPositive(nums2);
    print(result2 ?? -999);  // -999
}
```

## Complete Example: User Management

```stratos
package main;

class Address {
    var street: string;
    var city: string;
    var zipCode: Optional<string>;

    constructor(street: string, city: string, zipCode: Optional<string>) {
        this.street = street;
        this.city = city;
        this.zipCode = zipCode;
    }

    fn getFullAddress() string {
        val zip = this.zipCode ?? "No ZIP";
        return this.street + ", " + this.city + " " + zip;
    }
}

class User {
    var name: string;
    var email: Optional<string>;
    var address: Optional<Address>;
    var age: Optional<int>;

    constructor(name: string) {
        this.name = name;
        this.email = None;
        this.address = None;
        this.age = None;
    }

    fn getContactInfo() string {
        val emailInfo = this.email ?? "No email";
        val addressInfo = when (this.address) {
            Some(addr) -> addr.getFullAddress()
            None -> "No address"
        };

        return "Name: " + this.name + "\n" +
               "Email: " + emailInfo + "\n" +
               "Address: " + addressInfo;
    }

    fn isAdult() bool {
        // If age is None, assume not adult
        return (this.age ?? 0) >= 18;
    }
}

fn findUserById(id: int) Optional<User> {
    // Simulate database lookup
    when (id) {
        1 -> {
            val user = User("Alice");
            user.email = Some("alice@example.com");
            user.age = Some(25);
            return Some(user);
        }
        2 -> {
            val user = User("Bob");
            user.address = Some(Address("123 Main St", "New York", Some("10001")));
            user.age = Some(17);
            return Some(user);
        }
        else -> return None
    };
}

fn main() {
    // User with all info
    val user1 = findUserById(1);
    when (user1) {
        Some(u) -> {
            // u is auto-unwrapped here
            print(u.getContactInfo());
            print("Is adult: " + u.isAdult());
        }
        None -> print("User not found")
    }

    print("\n---\n");

    // User with partial info
    val user2 = findUserById(2);
    if user2.is_some() {
        // user2 is auto-unwrapped here
        print(user2.getContactInfo());
        print("Is adult: " + user2.isAdult());
    }

    print("\n---\n");

    // User not found - using safe navigation
    val user3 = findUserById(999);
    val userName = user3?.name ?? "Unknown User";
    print("Looking for: " + userName);
}
```

## Best Practices

::: tip
**Make nullability explicit**: Use `Optional<T>` to clearly indicate when values can be None. This makes your intent clear and catches errors at compile time.
:::

::: tip
**Prefer safe operators**: Use `?.` and `??` instead of explicit None checks when possible. They make code more concise and readable.
:::

::: tip
**Use pattern matching**: For complex None handling logic, `when` expressions with `Some`/`None` patterns are more expressive than if-else chains.
:::

::: tip
**Leverage auto-unwrapping**: Use `is_some()` guards and pattern matching to automatically unwrap values in safe contexts.
:::

::: info
**Default to non-nullable**: Only make types optional when nullability is a valid state. Overusing optionals can make code harder to work with.
:::

::: info
**Always wrap with Some()**: Remember to wrap values with `Some()` when assigning to Optional types or returning from functions with Optional return types.
:::

## None Safety Quick Reference

| Feature | Syntax | Purpose |
|---------|--------|---------|
| Optional type | `Optional<Type>` | Declare nullable variable |
| Some | `Some(42)` | Wrap value in Optional |
| None | `None` | Empty Optional |
| Check for value | `value.is_some()` | Returns true if Optional has a value |
| Check for None | `value.is_none()` | Returns true if Optional is None |
| Safe navigation | `obj?.method()` | Call method only if obj is not None, returns Optional |
| None coalescing | `value ?? default` | Provide default for None values |
| Pattern matching | `when (opt) { Some(v) -> ..., None -> ... }` | Match on Optional, auto-unwraps in Some branch |
| Auto-unwrap in guard | `if (opt.is_some()) { use opt }` | Automatically unwraps inside guard block |

## Summary

Stratos's None safety system ensures that null pointer exceptions are caught at compile time. Key features include:

- **Explicit Optional types** that make nullability part of the type system
- **Automatic unwrapping** in safe contexts (guards and pattern matching)
- **Safe navigation** (`?.`) that propagates None through call chains
- **None coalescing** (`??`) for providing default values
- **Pattern matching** with automatic unwrapping in `Some` branches

By making None handling explicit and providing powerful operators, Stratos helps you write safer, more maintainable code.