# HashMap/Dictionary Operations Example

This example demonstrates HashMap (dictionary/associative array) operations in Stratos, showcasing key-value pair storage and manipulation.

## Features Demonstrated

### 1. **Map Creation**
- Creating empty maps with `maps.create()`
- Initializing maps for various use cases

### 2. **Setting Values**
- Adding key-value pairs with `map.set(key, value)`
- Chaining multiple set operations
- Building maps incrementally

### 3. **Getting Values**
- Retrieving values with `map.get(key)`
- Using index notation `map[key]`
- Handling missing keys (returns empty string)

### 4. **Key Operations**
- Checking if keys exist with `map.has(key)`
- Getting all keys as an array with `map.keys()`
- Getting all values as an array with `map.values()`

### 5. **Map Modification**
- Removing entries with `map.remove(key)`
- Clearing all entries with `map.clear()`
- Checking size with `map.size()`
- Checking if empty with `map.isEmpty()`

### 6. **Practical Applications**
- Configuration management
- User profile storage
- HTTP headers
- Settings persistence with files

## Running the Example

```bash
# From the project root
stratos run examples/maps

# Or from the maps directory
cd examples/maps
stratos run .
```

## API Reference

### Creating Maps

```stratos
use maps;

val myMap = maps.create();
```

### Setting Values

```stratos
val map1 = myMap.set("key", "value");
val map2 = map1.set("name", "Alice");
val map3 = map2.set("age", "30");
```

### Getting Values

```stratos
// Method syntax
val name = map3.get("name");  // "Alice"

// Index syntax
val age = map3["age"];  // "30"

// Missing key returns empty string
val missing = map3.get("nonexistent");  // ""
```

### Checking Keys

```stratos
if map3.has("name") {
    println("Name exists!");
}
```

### Getting All Keys/Values

```stratos
val keys = map3.keys();      // Returns array<string>
val values = map3.values();  // Returns array<string>

println("First key: " + keys[0]);
println("Total keys: " + keys.length());
```

### Map Size

```stratos
val count = map3.size();        // Returns int
val empty = map3.isEmpty();     // Returns bool
```

### Removing Entries

```stratos
val map4 = map3.remove("age");
```

### Clearing Map

```stratos
val emptyMap = map3.clear();
```

### Utility Methods

**Get first/last keys and values:**
```stratos
val firstKey = map.first();         // Get first key
val lastKey = map.last();           // Get last key
val firstVal = map.firstValue();    // Get first value
val lastVal = map.lastValue();      // Get last value
```

**Merge maps:**
```stratos
val map1 = maps.create().set("a", "1").set("b", "2");
val map2 = maps.create().set("b", "3").set("c", "4");
val merged = map1.merge(map2);  // b=3 (map2 overwrites), has a, b, c
```

**Convert to/from entries:**
```stratos
val entries = map.entries();           // Returns array like ["key1:value1", "key2:value2"]
val newMap = maps.fromEntries(entries); // Reconstruct map from entries array
```

**Search by value:**
```stratos
val hasValue = map.containsValue("Alice");  // Check if any value equals "Alice"
```

## Map Types

Currently, Stratos supports:
- `map<string,string>` - String keys and string values

Future support planned for:
- `map<string,int>` - String keys, integer values
- `map<string,any>` - String keys, any value type
- Generic map types

## Important Notes

### Immutability Pattern
Maps in Stratos follow an immutable pattern. Operations like `set()`, `remove()`, and `clear()` return a new map:

```stratos
val map1 = maps.create();
val map2 = map1.set("key", "value");  // map2 is new map
// map1 is unchanged
```

### Missing Keys
When accessing a non-existent key:
- `map.get(key)` returns empty string `""`
- `map[key]` returns empty string `""`
- `map.has(key)` returns `false`

### Type Safety
- All keys must be strings
- All values must be strings
- Runtime type checking ensures safety

## Use Cases

### 1. Configuration Management
```stratos
val config = maps.create();
val config2 = config.set("host", "localhost");
val config3 = config2.set("port", "8080");
val config4 = config3.set("debug", "true");
```

### 2. User Data Storage
```stratos
val user = maps.create();
val user2 = user.set("username", "alice");
val user3 = user2.set("email", "alice@example.com");
val user4 = user3.set("role", "admin");
```

### 3. HTTP Headers
```stratos
val headers = maps.create();
val headers2 = headers.set("Content-Type", "application/json");
val headers3 = headers2.set("Authorization", "Bearer token");
```

### 4. Caching
```stratos
val cache = maps.create();
val cache2 = cache.set("user:123", "Alice");
val cache3 = cache2.set("user:456", "Bob");

if cache3.has("user:123") {
    val user = cache3.get("user:123");
}
```

## Learning Outcomes

After running this example, you'll understand:
1. How to create and initialize maps
2. How to set and get key-value pairs
3. How to check for key existence
4. How to iterate over keys and values
5. How to modify and clear maps
6. Practical applications of maps in real-world scenarios

## Next Steps

Try modifying the example to:
- Build a simple in-memory database
- Create a command-line key-value store
- Implement a configuration file parser
- Build a simple cache system

## Related Examples

- **arrays** - Learn about array operations (see `examples/arrays`)
- Arrays and maps work great together - maps can store arrays, and array of maps is powerful for structured data

## Performance Notes

- Map operations are O(1) average case (hash table)
- Keys() and values() operations are O(n)
- Maps use string hashing for fast lookups
- Currently limited to string keys and values for simplicity
