### Error Handling

Stratos uses `Optional` types and pattern matching with `when` for error handling instead of traditional try/catch blocks.

### Error Handling with Optional

Functions that can fail return `Optional<T>` types. Use pattern matching to handle both success and error cases.

**Example: File Operations**

```stratos
val fileData: Optional<string> = readFile("~/me.txt");

when (fileData) {
    Some(data) -> {
        print("File contents: " + data);
    }
    None -> {
        print("Error: Could not read file");
    }
}
```

**Example: Using .is_some() and .is_none() in conditions**

```stratos
val result: Optional<int> = compute();

if (result.is_some()) {
    // Safe to use result
    print("Success!");
} else {
    // Handle None case
    print("Operation failed");
}
```

### Chaining Optional Operations

Use safe call operator `?.` to chain operations on Optional values:

```stratos
val path: Optional<Path> = Path("~/me.txt");
val content: Optional<string> = path?.readAll();

val text = content ?: "File not found";
print(text);
```

### Best Practices

* Functions that can fail should return `Optional<T>` types
* Use `when` for exhaustive pattern matching on Optional values
* Use `.is_some()` and `.is_none()` for simple conditional checks
* Use `?.` for safe chaining of operations
* Use `?:` (null coalescing) to provide default values
