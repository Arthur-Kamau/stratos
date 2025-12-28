# Prelude Functions Test

This sample demonstrates the new prelude functions added to Stratos.

## New Functions

### 1. panic(message)
Terminates program with error message.
```stratos
panic("Something went wrong");
```

### 2. assert(condition, message)
Runtime assertion - panics if condition is false.
```stratos
assert(x > 0, "x must be positive");
```

### 3. dbg(value)
Debug print with type inspection - returns the value.
```stratos
val x = dbg(42);  // [DEBUG] int = 42
```

## Running Tests

```bash
# Normal tests
stratos run src/main.st

# Test panic (exits with code 1)
stratos run test_panic.st

# Test assert failure (exits with code 1)
stratos run test_assert_fail.st
```
