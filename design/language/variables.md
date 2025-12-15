
## Variables

Variables can be declared as immutable (`val`) or mutable (`var`).
Nullability is handled by the type system using the `Optional<Type>` wrapper.

### 1. Immutable (`val`)
Cannot be reassigned after initialization. This is the default recommendation.
*   `val myInt: int = 33` (Non-nullable)
*   `val myOptionalInt: Optional<int> = None` (Nullable, can hold `Some(value)` or `None`)

### 2. Mutable (`var`)
Can be reassigned.
*   `var myInt: int = 33` (Non-nullable, can change value but not to `None`)
*   `var myOptional: Optional<int> = Some(33)` (Nullable, can change value to `Some(int)` or `None`)

### Examples

1. **Non-nullable immutable integer:**
    `val myInt = 33` (Type inferred as `int`)

2. **Non-nullable mutable integer:**
    `var myCounter = 0`

3. **Nullable mutable integer:**
    `var myOptional: Optional<int> = None`
    `myOptional = Some(5)`

### Safety with Nullable Types (Optional)

Accessing `Optional` types requires safety checks.

* **Safe Call `?.`**
  `user?.name` (If `user` is `None`, `name` access results in `None`; otherwise, `Optional<string>` containing the name)

* **Null Coalescing `?:`**
  `val name = user?.name ?: "Unknown"` (If `user?.name` results in `None`, use "Unknown")

* **Pattern Matching `when`**
`when(my_value){ `<br/>
 &emsp; `None -> { report_err() } `<br/>
 &emsp; `Some(v) -> { some_operation(v) } `<br/>
`}`

### Optional Methods

The `Optional<T>` type provides built-in methods for checking its state:

* **`.is_some()`** - Returns `true` if the Optional contains a value (Some), `false` otherwise
* **`.is_none()`** - Returns `true` if the Optional is None, `false` otherwise

**Examples:**

```stratos
val empty: Optional<int> = None;
val filled: Optional<int> = Some(42);

// Using is_some()
if (filled.is_some()) {
    print("Has value!");
}

// Using is_none()
if (empty.is_none()) {
    print("No value");
}

// Common pattern: early return
val data: Optional<string> = fetchData();
if (data.is_none()) {
    return;  // Exit early if no data
}
// Continue with data processing
```

**When to use:**
- Use `.is_some()` / `.is_none()` for simple boolean checks in conditions
- Use `when` pattern matching when you need to extract and use the value
- Use `?.` and `?:` for chaining and providing defaults
