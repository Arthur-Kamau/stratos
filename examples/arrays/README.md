# Array Operations Example

This example demonstrates array operations and manipulation in Stratos, showcasing various ways to work with arrays.

## Features Demonstrated

### 1. **Array Creation**
- Creating arrays using `string.split()` method
- Working with different delimiters (comma, space, newline, pipe)
- Handling edge cases (empty elements, single items)

### 2. **Array Access**
- Indexing arrays with `array[index]` syntax
- Accessing first, last, and arbitrary elements
- Safe array access within bounds

### 3. **Array Methods**
- `length()` - Get the number of elements in an array
- Returns an integer representing array size

### 4. **Practical Applications**
- CSV data parsing and processing
- File content manipulation
- Path and version string parsing
- Tag and metadata processing

## Running the Example

```bash
# From the project root
stratos run examples/arrays

# Or from the arrays directory
cd examples/arrays
stratos run .
```

## Output

The example provides detailed output showing:
- Array creation and initialization
- Element access patterns
- Length calculations
- Real-world use cases
- CSV data processing
- File-based array operations

## Array Types

Currently, Stratos supports:
- `array<string>` - Arrays of strings

Future support planned for:
- `array<int>` - Arrays of integers
- `array<double>` - Arrays of floating-point numbers
- `array<bool>` - Arrays of booleans
- Multi-dimensional arrays

## Key Concepts

### Creating Arrays
```stratos
val fruits = "apple,banana,cherry".split(",");
// Creates: ["apple", "banana", "cherry"]
```

### Accessing Elements
```stratos
val first = fruits[0];   // "apple"
val second = fruits[1];  // "banana"
val last = fruits[2];    // "cherry"
```

### Getting Length
```stratos
val count = fruits.length();  // 3
```

### Utility Methods

**Check if empty:**
```stratos
val isEmpty = fruits.isEmpty();  // false
```

**Get first/last element:**
```stratos
val first = fruits.first();  // "apple"
val last = fruits.last();    // "cherry"
```

**Search for values:**
```stratos
val hasApple = fruits.contains("apple");  // true
val index = fruits.indexOf("banana");     // 1 (or -1 if not found)
```

**Reverse array:**
```stratos
val reversed = fruits.reverse();  // ["cherry", "banana", "apple"]
```

**Join into string:**
```stratos
val csv = fruits.join(",");      // "apple,banana,cherry"
val spaced = fruits.join(" ");   // "apple banana cherry"
```

**Clear array:**
```stratos
val empty = fruits.clear();  // Returns empty array
```

## Error Handling

The example demonstrates safe array operations:
- Proper bounds checking
- Handling empty arrays
- Managing arrays with empty elements

## Learning Outcomes

After running this example, you'll understand:
1. How to create arrays from strings
2. How to access array elements by index
3. How to get the length of an array
4. Practical use cases for arrays in data processing
5. How arrays work with file I/O operations

## Next Steps

Try modifying the example to:
- Parse different types of data
- Process your own CSV files
- Implement array search functionality
- Create array manipulation functions

## Related Data Structures

### Maps (HashMaps/Dictionaries)
For key-value pair storage, check out the **maps** example:
- Store data with string keys: `map.set("name", "Alice")`
- Retrieve values quickly: `map.get("name")` or `map["name"]`
- Perfect for configuration, caching, and lookup tables
- See `examples/maps` for comprehensive map operations

**When to use Arrays vs Maps:**
- Use **Arrays** when you have ordered, indexed data (lists, sequences)
- Use **Maps** when you need to look up data by a specific key (dictionaries, records)

## Notes

- Array indices start at 0
- Arrays are bounds-checked at runtime
- The `split()` method is the primary way to create arrays from strings
- Empty delimiter matches will create empty string elements in the array
