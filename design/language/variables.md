
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
