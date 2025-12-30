# Loops Example

This example demonstrates various loop patterns in Stratos using `while` loops.

## Running the Example

```bash
stratos run ./src/main.st
```

## Loop Patterns Demonstrated

### 1. Simple While Loop
Basic counting loop from 1 to 5.

### 2. Count Down
Counting down from 10 to 1 with a message at the end.

### 3. Sum Numbers
Accumulating a sum of numbers from 1 to 10 (result: 55).

### 4. Find Maximum
Finding the maximum value in a series of computed values (squares of 1-5).

### 5. Nested Loops
Demonstrating nested while loops to create a 3×3 multiplication table.

### 6. Loop with Condition
Doubling a value until it reaches or exceeds 100, counting iterations.

## Key Concepts

- **While loops**: `while (condition) { ... }`
- **Loop variables**: Use `var` for mutable counter variables
- **Variable assignment**: `i = i + 1` (increment pattern)
- **Nested loops**: Loops within loops for multi-dimensional iteration
- **Accumulation**: Building up values across iterations (sum, max)

## Expected Output

```
=== Simple While Loop ===
1
2
3
4
5

=== Count Down ===
10
9
...
1
Liftoff!

=== Sum Numbers ===
Sum of 1 to 10:
55

=== Find Maximum ===
1
4
9
16
25
Maximum value:
25

=== Nested Loops ===
1
2
3
2
4
6
3
6
9

=== Loop with Condition ===
4
8
16
32
64
128
Reached 100 in iterations:
6
```

## Notes

- Stratos currently supports `while` loops
- The `for` keyword is reserved for future use
- Loop variables must be declared as `var` (mutable)
- Functions must be declared before they are called
