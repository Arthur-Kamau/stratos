---
title: Null Safety
description: Optional types and safe value handling in Stratos
---

# Null Safety

Stratos provides robust null safety features to prevent null pointer exceptions and make code more reliable. Learn how to work with optional values safely.

## The Problem with Null

Traditional languages allow variables to be `null`, leading to runtime errors:

```stratos
// In languages without null safety:
val user = getUser(123);
print(user.name);  // ❌ Crashes if user is null!
```

Stratos solves this with **Optional types** that make nullability explicit in the type system.

## Optional Types

### Declaration Syntax

```stratos
// Non-nullable (cannot be null)
val name: string = "Alice";
// name = null;  // ❌ Compile error!

// Nullable (can be null)
val maybeName: string? = null;  // ✅ Allowed
val userName: string? = "Bob";   // ✅ Also allowed
```

The `?` suffix indicates a type is optional (nullable).

## Creating Optional Values

### Using Optional\<T>

```stratos
// Explicit Optional type
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

### Using Nullable Types

```stratos
// More concise syntax
val number: int? = 42;
val noNumber: int? = null;

if (number != null) {
    print("Has value: " + number);
} else {
    print("No value");
}
```

## Safe Access Operators

### Safe Navigation (?)

The safe navigation operator `?.` safely accesses properties/methods on nullable values:

```stratos
class User {
    var name: string;
    var email: string?;

    constructor(name: string, email: string?) {
        this.name = name;
        this.email = email;
    }

    fn getEmailDomain() string? {
        // Safe navigation - returns null if email is null
        return this.email?.split("@")[1];
    }
}

fn main() {
    val user1 = User("Alice", "alice@example.com");
    val user2 = User("Bob", null);

    // Safe access
    print(user1.email?.length());  // 17
    print(user2.email?.length());  // null

    // Chaining safe calls
    val domain1 = user1.getEmailDomain();  // "example.com"
    val domain2 = user2.getEmailDomain();  // null
}
```

### Null Coalescing (??)

Provide a default value when dealing with nulls:

```stratos
val name: string? = null;

// If name is null, use "Guest"
val displayName = name ?? "Guest";
print(displayName);  // "Guest"

// With non-null value
val actualName: string? = "Alice";
val display = actualName ?? "Guest";
print(display);  // "Alice"
```

### Combined Usage

```stratos
class Profile {
    var bio: string?;

    constructor(bio: string?) {
        this.bio = bio;
    }
}

class User {
    var profile: Profile?;

    constructor(profile: Profile?) {
        this.profile = profile;
    }
}

fn main() {
    val user = User(Profile("Software Developer"));

    // Chain safe navigation with null coalescing
    val bio = user.profile?.bio ?? "No bio available";
    print(bio);  // "Software Developer"

    val userWithoutProfile = User(null);
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

## Unwrapping Optionals

### Safe Unwrapping with If

```stratos
val maybeAge: int? = 25;

if (maybeAge != null) {
    // Inside this block, maybeAge is treated as non-null
    val age: int = maybeAge;
    print("Age: " + age);
}
```

### Force Unwrapping (Use with Caution)

```stratos
val number: int? = 42;

// Force unwrap with ! operator
val value: int = number!;  // ✅ Works because number is not null

val nullValue: int? = null;
// val forced: int = nullValue!;  // ❌ Runtime error!
```

::: warning
**Avoid force unwrapping**: Only use `!` when you're absolutely certain the value is not null. Prefer safe unwrapping with `if` or `when`.
:::

## Working with Functions

### Optional Parameters

```stratos
fn greet(name: string, title: string?) {
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

```stratos
fn findUser(id: int) User? {
    if (id <= 0) {
        return null;
    }

    // Simulate database lookup
    return User("User" + id);
}

fn main() {
    val user1 = findUser(123);
    if (user1 != null) {
        print("Found: " + user1.name);
    } else {
        print("User not found");
    }

    val user2 = findUser(-1);
    print(user2?.name ?? "Anonymous");
}
```

## Optional Collections

### Arrays with Optional Elements

```stratos
val numbers: Array<int?> = [1, null, 3, null, 5];

for (num in numbers) {
    val value = num ?? 0;
    print(value);
}
// Output: 1, 0, 3, 0, 5
```

### Filtering Optionals

```stratos
fn firstPositive(numbers: Array<int>) int? {
    for (num in numbers) {
        if (num > 0) {
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
    var zipCode: string?;

    constructor(street: string, city: string, zipCode: string?) {
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
    var email: string?;
    var address: Address?;
    var age: int?;

    constructor(name: string) {
        this.name = name;
        this.email = null;
        this.address = null;
        this.age = null;
    }

    fn getContactInfo() string {
        val emailInfo = this.email ?? "No email";
        val addressInfo = this.address?.getFullAddress() ?? "No address";

        return "Name: " + this.name + "\n" +
               "Email: " + emailInfo + "\n" +
               "Address: " + addressInfo;
    }

    fn isAdult() bool {
        // If age is null, assume not adult
        return (this.age ?? 0) >= 18;
    }
}

fn findUserById(id: int) User? {
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
            print(u.getContactInfo());
            print("Is adult: " + u.isAdult());
        }
        None -> print("User not found")
    }

    print("\n---\n");

    // User with partial info
    val user2 = findUserById(2);
    if (user2 != null) {
        print(user2.getContactInfo());
        print("Is adult: " + user2.isAdult());
    }

    print("\n---\n");

    // User not found
    val user3 = findUserById(999);
    val userName = user3?.name ?? "Unknown User";
    print("Looking for: " + userName);
}
```

## Best Practices

::: tip
**Make nullability explicit**: Use `?` to clearly indicate when values can be null. This makes your intent clear and catches errors at compile time.
:::

::: tip
**Prefer safe operators**: Use `?.` and `??` instead of explicit null checks when possible. They make code more concise and readable.
:::

::: tip
**Use pattern matching**: For complex null handling logic, `when` expressions with `Some`/`None` patterns are more expressive than if-else chains.
:::

::: warning
**Avoid force unwrapping**: The `!` operator bypasses null safety. Use it sparingly and only when you're absolutely certain the value exists.
:::

::: info
**Default to non-null**: Only make types optional when nullability is a valid state. Overusing optionals can make code harder to work with.
:::

## Null Safety Quick Reference

| Feature | Syntax | Purpose |
|---------|--------|---------|
| Optional type | `Type?` | Declare nullable variable |
| Optional\<T> | `Optional<int>` | Explicit optional type |
| Some | `Some(42)` | Wrap value in Optional |
| None | `None` | Empty Optional |
| Safe navigation | `obj?.method()` | Call method only if obj is not null |
| Null coalescing | `value ?? default` | Provide default for null values |
| Force unwrap | `value!` | Unwrap, crash if null (avoid!) |
| Pattern matching | `when (opt) { Some(v) -> ..., None -> ... }` | Match on Optional |

## Next Steps


