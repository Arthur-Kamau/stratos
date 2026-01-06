# Loop Examples

This example demonstrates various loop patterns in Stratos using both `for` loops and `while` loops.

## Running the Example

```bash
stratos run examples/loops

# Or from the loops directory
cd examples/loops
stratos run .
```

## Loop Types in Stratos

### 1. For Loops (For-In)
**Best for**: Iterating over collections (arrays, maps)

```stratos
val fruits = "apple,banana,cherry".split(",");

for val fruit in fruits {
    println(fruit);
}
```

**Syntax:**
- `for val variable in collection { ... }` - immutable loop variable
- `for var variable in collection { ... }` - mutable loop variable

**Works with:**
- Arrays: `for val item in myArray { ... }`
- Maps: `for val key in myMap { ... }` (iterates over keys)

### 2. While Loops
**Best for**: Condition-based iteration with complex logic

```stratos
var i = 1;
while i <= 10 {
    println(i);
    i = i + 1;
}
```

**Syntax:**
- `while condition { ... }` - loops while condition is true

## For Loop Examples Demonstrated

### 1. **Basic Array Iteration**
Iterate over array elements with clean syntax:
```stratos
val fruits = "apple,banana,cherry".split(",");
for val fruit in fruits {
    println(fruit);
}
```

### 2. **For Loop with Counter**
Track iteration count while processing:
```stratos
var count = 0;
for val item in items {
    count = count + 1;
    println(count + ". " + item);
}
```

### 3. **Processing & Accumulation**
Sum, transform, or aggregate values:
```stratos
var sum = 0;
for val numStr in numbers {
    sum = sum + numStr.toInt();
}
```

### 4. **Search Pattern**
Find elements in collections:
```stratos
var found = false;
for val item in items {
    if item == searchFor {
        found = true;
    }
}
```

### 5. **Nested For Loops**
Process multi-dimensional data:
```stratos
for val row in rows {
    for val cell in row {
        println(cell);
    }
}
```

### 6. **Iterating Over Maps**
Access map keys in for loops:
```stratos
val config = maps.create().set("host", "localhost").set("port", "8080");

for val key in config {
    val value = config[key];
    println(key + " = " + value);
}
```

### 7. **File Processing**
Read and process file lines:
```stratos
val lines = io.readFile("data.txt").split("\n");
for val line in lines {
    println(line);
}
```

## While Loop Examples Demonstrated

### 8. **Simple Counter**
Basic counting with while:
```stratos
var i = 1;
while i <= 5 {
    println(i);
    i = i + 1;
}
```

### 9. **Until Condition Met**
Loop until reaching a target:
```stratos
var value = 2;
while value < 100 {
    value = value * 2;
}
```

### 10. **Complex Calculations**
Factorial and other iterative algorithms:
```stratos
var result = 1;
var i = 1;
while i <= n {
    result = result * i;
    i = i + 1;
}
```

## For vs While: When to Use Each

### Use FOR loops when:
✓ Iterating over arrays, lists, or collections
✓ You want clean, readable iteration syntax
✓ You don't need complex loop control
✓ Processing all elements in a collection
✓ You don't need manual index management

**Example:** Processing every file in a directory, every line in a file, every item in a list

### Use WHILE loops when:
✓ Iteration depends on a complex condition
✓ Number of iterations is unknown beforehand
✓ Need precise control over loop logic
✓ Implementing algorithms (factorial, fibonacci, etc.)
✓ Looping until a specific state is reached

**Example:** Reading until EOF, doubling until threshold, finding a solution

## Comparison Example

**Same task with FOR:**
```stratos
val numbers = "10,20,30".split(",");
for val num in numbers {
    println(num);
}
```

**Same task with WHILE:**
```stratos
val numbers = "10,20,30".split(",");
var i = 0;
while i < numbers.length() {
    println(numbers[i]);
    i = i + 1;
}
```

FOR loop is cleaner for this use case!

## Key Concepts

### Loop Variables
- Use `val` for immutable loop variables (most common)
- Use `var` for mutable loop variables (when you need to modify them)

### Scope
- Loop variables are scoped to the loop body
- Variables declared inside loops are not accessible outside

### Type Annotations (Optional)
```stratos
for val item: string in items {
    // item is explicitly typed as string
}
```

## Current Limitations

- **Break/Continue**: Not yet implemented (coming soon)
- **Range syntax**: `for i in 0..10` not yet supported
- **Array iteration only**: For loops currently work with arrays and maps
- **Numeric for loops**: C-style `for (i = 0; i < 10; i++)` not supported (use while loops instead)

## Performance Notes

- For loops over arrays: O(n) iteration
- While loops: Performance depends on condition evaluation
- For loops have slightly less overhead than equivalent while loops with manual indexing

## Tips

1. **Choose for loops** when iterating collections - cleaner and more readable
2. **Use descriptive variable names** in for loops (e.g., `for val user in users`)
3. **Keep loop bodies simple** - extract complex logic into functions
4. **Avoid modifying collections** while iterating over them
5. **Use while loops** for complex iteration logic or unknown iteration counts

## Related Examples

- **arrays** - Learn about array operations (see `examples/arrays`)
- **maps** - Learn about map/dictionary operations (see `examples/maps`)
- Arrays and maps are the primary collections you'll iterate over with for loops

## Learning Outcomes

After running this example, you'll understand:
1. How to use for-in loops to iterate over arrays
2. How to use for loops with maps (iterating over keys)
3. When to use for loops vs while loops
4. Loop variable scoping and mutability
5. Common loop patterns (counting, searching, accumulation)
6. Nested loop structures
7. Practical applications in file processing and data manipulation

## Next Steps

Try implementing:
- A function that filters an array based on a condition
- A word counter using for loops
- A CSV parser using loops
- A simple data aggregation function
- A search function that finds all matching elements
