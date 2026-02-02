---
title: Maps Module
description: Immutable key-value map operations in Stratos
---

# Maps Module

The `maps` module provides immutable key-value map operations for storing and retrieving data by keys.

## Importing

```stratos
use maps;
```

## Creating Maps

### maps.create

Creates a new empty map.

```stratos
use maps;

fn main() {
    val emptyMap = maps.create();
}
```

## Adding and Updating Values

### Map.put

Adds a key-value pair to the map, returning a new map with the addition.

**Signature:**
```stratos
fn put(key: string, value: string) Map
```

**Example:**
```stratos
use maps;

fn main() {
    val config = maps.create();
    val config2 = config.put("host", "localhost");
    val config3 = config2.put("port", "8080");
    val config4 = config3.put("debug", "true");

    print(config4.get("host"));  // "localhost"
}
```

::: info Immutability
Maps are immutable. The `put` method returns a new map with the added key-value pair, leaving the original map unchanged.
:::

## Accessing Values

### Map.get

Retrieves the value associated with a key.

**Signature:**
```stratos
fn get(key: string) string
```

**Example:**
```stratos
use maps;

fn main() {
    val config = maps.create()
        .put("host", "localhost")
        .put("port", "8080");

    val host = config.get("host");
    print("Host: $host");  // "Host: localhost"

    // Index syntax also works
    val port = config["port"];
    print("Port: $port");  // "Port: 8080"
}
```

### Map.has

Checks if a key exists in the map.

**Signature:**
```stratos
fn has(key: string) bool
```

**Example:**
```stratos
use maps;

fn main() {
    val config = maps.create()
        .put("host", "localhost");

    print(config.has("host"));     // true
    print(config.has("timeout"));  // false
}
```

## Map Information

### Map.size

Returns the number of key-value pairs in the map.

**Signature:**
```stratos
fn size() int
```

### Map.isEmpty

Checks if the map is empty.

**Signature:**
```stratos
fn isEmpty() bool
```

**Example:**
```stratos
use maps;

fn main() {
    val empty = maps.create();
    val config = empty.put("key", "value");

    print(empty.isEmpty());   // true
    print(empty.size());      // 0
    print(config.isEmpty());  // false
    print(config.size());     // 1
}
```

## Removing Values

### Map.remove

Removes a key-value pair, returning a new map without that key.

**Signature:**
```stratos
fn remove(key: string) Map
```

### Map.clear

Returns an empty map.

**Signature:**
```stratos
fn clear() Map
```

**Example:**
```stratos
use maps;

fn main() {
    val config = maps.create()
        .put("a", "1")
        .put("b", "2")
        .put("c", "3");

    val withoutB = config.remove("b");
    print(withoutB.has("b"));  // false
    print(withoutB.size());    // 2

    val empty = config.clear();
    print(empty.isEmpty());    // true
}
```

## Iteration

### Map.keys

Returns an array of all keys in the map.

**Signature:**
```stratos
fn keys() Array<string>
```

### Map.values

Returns an array of all values in the map.

**Signature:**
```stratos
fn values() Array<string>
```

### Map.entries

Returns an array of key-value pair objects.

**Signature:**
```stratos
fn entries() Array<Entry>
```

**Example:**
```stratos
use maps;

fn main() {
    val config = maps.create()
        .put("host", "localhost")
        .put("port", "8080")
        .put("debug", "true");

    // Iterate over keys
    val keys = config.keys();
    for val key in keys {
        print("Key: $key");
    }

    // Iterate over values
    val values = config.values();
    for val value in values {
        print("Value: $value");
    }

    // Iterate over entries
    val entries = config.entries();
    for val entry in entries {
        print("${entry.first} = ${entry.second}");
    }
}
```

## Complete Example

```stratos
package main;

use maps;
use log;

fn main() {
    // Create a configuration map
    var config = maps.create();

    // Add configuration values
    config = config.put("database.host", "localhost");
    config = config.put("database.port", "5432");
    config = config.put("database.name", "myapp");
    config = config.put("cache.enabled", "true");
    config = config.put("cache.ttl", "3600");

    log.info("Configuration loaded with ${config.size()} settings");

    // Check and retrieve values
    if (config.has("database.host")) {
        val host = config.get("database.host");
        val port = config.get("database.port");
        log.info("Database: $host:$port");
    }

    // Print all settings
    log.info("\nAll configuration:");
    for val entry in config.entries() {
        log.info("  ${entry.first}: ${entry.second}");
    }

    // Remove a setting
    config = config.remove("cache.enabled");
    log.info("\nAfter removing cache.enabled: ${config.size()} settings");
}
```

## Best Practices

::: tip Chain Operations
Use method chaining for cleaner code when building maps:
```stratos
val config = maps.create()
    .put("key1", "value1")
    .put("key2", "value2")
    .put("key3", "value3");
```
:::

::: tip Check Before Access
Always check if a key exists before accessing it to avoid errors:
```stratos
if (map.has("key")) {
    val value = map.get("key");
    // Use value...
}
```
:::

::: info Immutability Benefits
Immutable maps are thread-safe and prevent accidental modifications. Always assign the result of `put` or `remove` to capture the new map.
:::

## API Reference

| Method | Description | Returns |
|--------|-------------|---------|
| `maps.create()` | Create empty map | `Map` |
| `map.put(key, value)` | Add key-value pair | `Map` |
| `map.get(key)` | Get value by key | `string` |
| `map.has(key)` | Check if key exists | `bool` |
| `map.remove(key)` | Remove key-value pair | `Map` |
| `map.clear()` | Remove all entries | `Map` |
| `map.size()` | Get entry count | `int` |
| `map.isEmpty()` | Check if empty | `bool` |
| `map.keys()` | Get all keys | `Array<string>` |
| `map.values()` | Get all values | `Array<string>` |
| `map.entries()` | Get all entries | `Array<Entry>` |

## See Also

- [Collections Module](/stdlib/collections) - Lists, sets, and other data structures
- [JSON Module](/stdlib/json) - Convert maps to/from JSON
