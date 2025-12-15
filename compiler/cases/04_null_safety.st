// 04_null_safety.st
// Optionals and Null Safety

// Optional Declaration
val maybeNumber: Optional<int> = Some(42);
val empty: Optional<int> = None;

// Safe Access ?.
// Assuming User class exists with nullable name
// val name = user?.name; 

// Null Coalescing ?:
// val displayName = name ?: "Guest";

// Pattern Matching on Optionals
when (maybeNumber) {
    Some(v) -> { print("Value is " + v); }
    None -> { print("No value"); }
}
