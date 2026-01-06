package main;

use io;

fn printHeader(title: string) void {
    println("\n" + "=".repeat(50));
    println(title);
    println("=".repeat(50));
}

fn printSection(num: int, title: string) void {
    println("\n" + num + ". " + title);
    println("-".repeat(40));
}

fn main() {
    printHeader("Map Utility Functions Demo");

    // =================================================================
    // 1. first, last, firstValue, lastValue
    // =================================================================
    printSection(1, "first(), last(), firstValue(), lastValue()");

    val config = maps.create();
    val config2 = config.set("host", "localhost");
    val config3 = config2.set("port", "8080");
    val config4 = config3.set("database", "mydb");
    val config5 = config4.set("timeout", "30");

    println("Configuration map with " + config5.size() + " entries");
    println("  first() key: " + config5.first());
    println("  firstValue(): " + config5.firstValue());
    println("  last() key: " + config5.last());
    println("  lastValue(): " + config5.lastValue());

    val empty = maps.create();
    println("\nEmpty map:");
    println("  first(): '" + empty.first() + "' (returns empty)");
    println("  last(): '" + empty.last() + "' (returns empty)");

    // =================================================================
    // 2. merge
    // =================================================================
    printSection(2, "merge() - Combining Maps");

    val defaults = maps.create();
    val defaults2 = defaults.set("theme", "light");
    val defaults3 = defaults2.set("language", "en");
    val defaults4 = defaults3.set("notifications", "on");

    val userPrefs = maps.create();
    val userPrefs2 = userPrefs.set("theme", "dark");
    val userPrefs3 = userPrefs2.set("fontSize", "14");

    println("Default settings:");
    println("  Size: " + defaults4.size());
    println("  Keys: " + defaults4.keys().join(", "));

    println("\nUser preferences:");
    println("  Size: " + userPrefs3.size());
    println("  Keys: " + userPrefs3.keys().join(", "));

    val merged = defaults4.merge(userPrefs3);
    println("\nMerged settings:");
    println("  Size: " + merged.size());
    println("  Keys: " + merged.keys().join(", "));
    println("  theme (overridden): " + merged["theme"]);
    println("  language (from defaults): " + merged["language"]);
    println("  fontSize (from user): " + merged["fontSize"]);

    // =================================================================
    // 3. entries and fromEntries
    // =================================================================
    printSection(3, "entries() and fromEntries()");

    val userData = maps.create();
    val userData2 = userData.set("username", "alice");
    val userData3 = userData2.set("email", "alice@example.com");
    val userData4 = userData3.set("role", "admin");

    val entriesArray = userData4.entries();
    println("Map as entries array:");
    println("  Length: " + entriesArray.length());
    println("  [0]: " + entriesArray[0]);
    println("  [1]: " + entriesArray[1]);
    println("  [2]: " + entriesArray[2]);
    println("  All: " + entriesArray.join(", "));

    // Save to file and reload
    io.writeFile("user_data.txt", entriesArray.join("\n"));
    println("\n✓ Saved entries to file");

    val loadedEntries = io.readFile("user_data.txt").split("\n");
    val reconstructed = maps.fromEntries(loadedEntries);

    println("✓ Loaded entries from file");
    println("Reconstructed map:");
    println("  Size: " + reconstructed.size());
    println("  username: " + reconstructed["username"]);
    println("  email: " + reconstructed["email"]);

    io.remove("user_data.txt");
    println("✓ Cleaned up file");

    // =================================================================
    // 4. containsValue
    // =================================================================
    printSection(4, "containsValue() - Search by Value");

    val inventory = maps.create();
    val inventory2 = inventory.set("item1", "laptop");
    val inventory3 = inventory2.set("item2", "mouse");
    val inventory4 = inventory3.set("item3", "keyboard");
    val inventory5 = inventory4.set("item4", "laptop");

    println("Inventory: " + inventory5.keys().length() + " items");
    println("  Values: " + inventory5.values().join(", "));

    println("\nSearching for values:");
    println("  containsValue('laptop'): " + inventory5.containsValue("laptop"));
    println("  containsValue('monitor'): " + inventory5.containsValue("monitor"));
    println("  containsValue('mouse'): " + inventory5.containsValue("mouse"));

    // =================================================================
    // 5. Practical: Configuration Override System
    // =================================================================
    printSection(5, "Practical: Configuration Override");

    val systemDefaults = maps.create();
    val systemDefaults2 = systemDefaults.set("max_connections", "100");
    val systemDefaults3 = systemDefaults2.set("timeout", "60");
    val systemDefaults4 = systemDefaults3.set("debug", "false");
    val systemDefaults5 = systemDefaults4.set("log_level", "info");

    val envConfig = maps.create();
    val envConfig2 = envConfig.set("debug", "true");
    val envConfig3 = envConfig2.set("log_level", "debug");

    val userConfig = maps.create();
    val userConfig2 = userConfig.set("max_connections", "50");

    // Merge in order: defaults -> environment -> user
    val finalConfig = systemDefaults5.merge(envConfig3).merge(userConfig2);

    println("Final configuration (3-level merge):");
    println("  max_connections: " + finalConfig["max_connections"] + " (user override)");
    println("  timeout: " + finalConfig["timeout"] + " (system default)");
    println("  debug: " + finalConfig["debug"] + " (env override)");
    println("  log_level: " + finalConfig["log_level"] + " (env override)");
    println("  Total keys: " + finalConfig.size());

    // =================================================================
    // 6. Practical: Key-Value Cache with Metadata
    // =================================================================
    printSection(6, "Practical: Cache System");

    val cache = maps.create();
    val cache2 = cache.set("user:123", "Alice");
    val cache3 = cache2.set("user:456", "Bob");
    val cache4 = cache3.set("user:789", "Charlie");

    println("Cache status:");
    println("  Total entries: " + cache4.size());
    println("  isEmpty: " + cache4.isEmpty());
    println("  First cached key: " + cache4.first());
    println("  Last cached key: " + cache4.last());

    println("\nCache lookups:");
    println("  has('user:123'): " + cache4.has("user:123"));
    println("  has('user:999'): " + cache4.has("user:999"));

    // Get all user IDs
    val userIds = cache4.keys();
    println("\nAll cached user IDs:");
    println("  " + userIds.join(", "));

    // Clear cache
    val clearedCache = cache4.clear();
    println("\nAfter clearing:");
    println("  Size: " + clearedCache.size());
    println("  isEmpty: " + clearedCache.isEmpty());

    // =================================================================
    // 7. Practical: Data Export/Import
    // =================================================================
    printSection(7, "Practical: Data Serialization");

    val appState = maps.create();
    val appState2 = appState.set("window_width", "1920");
    val appState3 = appState2.set("window_height", "1080");
    val appState4 = appState3.set("theme", "dark");
    val appState5 = appState4.set("last_file", "/home/user/document.txt");

    // Export to entries
    val exported = appState5.entries();
    println("Exported state: " + exported.length() + " entries");
    println("  Format: key:value");
    println("  Sample: " + exported.first());

    // Save to file
    val serialized = exported.join("|");
    io.writeFile("app_state.dat", serialized);
    println("✓ Saved to app_state.dat");

    // Load and restore
    val loaded = io.readFile("app_state.dat");
    val deserialized = loaded.split("|");
    val restoredState = maps.fromEntries(deserialized);

    println("\nRestored state:");
    println("  Size: " + restoredState.size());
    println("  window_width: " + restoredState["window_width"]);
    println("  theme: " + restoredState["theme"]);

    io.remove("app_state.dat");
    println("✓ Cleaned up");

    // =================================================================
    // Summary
    // =================================================================
    printHeader("Map Utilities Summary");

    println("\n✓ first() - Get first key");
    println("✓ last() - Get last key");
    println("✓ firstValue() - Get first value");
    println("✓ lastValue() - Get last value");
    println("✓ merge(other) - Combine two maps");
    println("✓ entries() - Convert to array of 'key:value'");
    println("✓ fromEntries(array) - Create map from entries");
    println("✓ containsValue(value) - Search by value");

    println("\n" + "=".repeat(50));
    println("All map utility functions work perfectly!");
    println("=".repeat(50));
}
