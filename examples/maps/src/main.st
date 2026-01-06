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
    printHeader("Stratos HashMap/Dictionary Demo");

    // =================================================================
    // 1. Creating Maps
    // =================================================================
    printSection(1, "Creating and Initializing Maps");

    val userMap = maps.create();
    println("✓ Created empty map");
    println("  Size: " + userMap.size());
    println("  Is empty: " + userMap.isEmpty());

    // =================================================================
    // 2. Setting Values
    // =================================================================
    printSection(2, "Setting Key-Value Pairs");

    val userData = maps.create();
    val userData2 = userData.set("name", "Alice");
    val userData3 = userData2.set("age", "30");
    val userData4 = userData3.set("city", "New York");
    val userData5 = userData4.set("occupation", "Engineer");

    println("✓ Added 4 entries to map");
    println("  Map size: " + userData5.size());
    println("  Is empty: " + userData5.isEmpty());

    // =================================================================
    // 3. Getting Values
    // =================================================================
    printSection(3, "Retrieving Values by Key");

    println("User Information:");
    println("  Name: " + userData5.get("name"));
    println("  Age: " + userData5.get("age"));
    println("  City: " + userData5.get("city"));
    println("  Occupation: " + userData5.get("occupation"));

    println("\nTrying to get non-existent key:");
    println("  Country: '" + userData5.get("country") + "' (returns empty string)");

    // =================================================================
    // 4. Map Indexing with []
    // =================================================================
    printSection(4, "Using Index Notation map[key]");

    println("Accessing values with [] notation:");
    println("  userData5[\"name\"] = " + userData5["name"]);
    println("  userData5[\"city\"] = " + userData5["city"]);
    println("  userData5[\"age\"] = " + userData5["age"]);

    // =================================================================
    // 5. Checking Key Existence
    // =================================================================
    printSection(5, "Checking if Keys Exist");

    println("Checking key existence:");
    println("  has(\"name\"): " + userData5.has("name"));
    println("  has(\"age\"): " + userData5.has("age"));
    println("  has(\"email\"): " + userData5.has("email"));
    println("  has(\"salary\"): " + userData5.has("salary"));

    // =================================================================
    // 6. Getting Keys and Values
    // =================================================================
    printSection(6, "Extracting All Keys and Values");

    val keys = userData5.keys();
    val values = userData5.values();

    println("All keys (" + keys.length() + " total):");
    println("  [0]: " + keys[0]);
    println("  [1]: " + keys[1]);
    println("  [2]: " + keys[2]);
    println("  [3]: " + keys[3]);

    println("\nAll values (" + values.length() + " total):");
    println("  [0]: " + values[0]);
    println("  [1]: " + values[1]);
    println("  [2]: " + values[2]);
    println("  [3]: " + values[3]);

    // =================================================================
    // 7. Removing Entries
    // =================================================================
    printSection(7, "Removing Key-Value Pairs");

    println("Before removal:");
    println("  Size: " + userData5.size());
    println("  has(\"city\"): " + userData5.has("city"));

    val userData6 = userData5.remove("city");

    println("\nAfter removing 'city':");
    println("  Size: " + userData6.size());
    println("  has(\"city\"): " + userData6.has("city"));
    println("  Remaining keys: " + userData6.keys().length());

    // =================================================================
    // 8. Clearing Maps
    // =================================================================
    printSection(8, "Clearing All Entries");

    println("Before clear:");
    println("  Size: " + userData6.size());
    println("  Is empty: " + userData6.isEmpty());

    val emptyMap = userData6.clear();

    println("\nAfter clear:");
    println("  Size: " + emptyMap.size());
    println("  Is empty: " + emptyMap.isEmpty());

    // =================================================================
    // 9. Practical Example: Configuration Storage
    // =================================================================
    printSection(9, "Practical Example: Configuration Map");

    val config = maps.create();
    val config2 = config.set("host", "localhost");
    val config3 = config2.set("port", "8080");
    val config4 = config3.set("debug", "true");
    val config5 = config4.set("log_level", "info");
    val config6 = config5.set("max_connections", "100");

    println("Server Configuration:");
    println("  Host: " + config6["host"]);
    println("  Port: " + config6["port"]);
    println("  Debug Mode: " + config6["debug"]);
    println("  Log Level: " + config6["log_level"]);
    println("  Max Connections: " + config6["max_connections"]);

    // =================================================================
    // 10. Practical Example: User Profiles
    // =================================================================
    printSection(10, "Practical Example: Multiple User Profiles");

    // User 1
    val user1 = maps.create();
    val user1a = user1.set("username", "alice123");
    val user1b = user1a.set("email", "alice@example.com");
    val user1c = user1b.set("role", "admin");

    // User 2
    val user2 = maps.create();
    val user2a = user2.set("username", "bob456");
    val user2b = user2a.set("email", "bob@example.com");
    val user2c = user2b.set("role", "user");

    println("User Profiles:");
    println("\nUser 1:");
    println("  Username: " + user1c["username"]);
    println("  Email: " + user1c["email"]);
    println("  Role: " + user1c["role"]);

    println("\nUser 2:");
    println("  Username: " + user2c["username"]);
    println("  Email: " + user2c["email"]);
    println("  Role: " + user2c["role"]);

    // =================================================================
    // 11. Practical Example: HTTP Headers
    // =================================================================
    printSection(11, "Practical Example: HTTP Headers Map");

    val headers = maps.create();
    val headers2 = headers.set("Content-Type", "application/json");
    val headers3 = headers2.set("Authorization", "Bearer token123");
    val headers4 = headers3.set("User-Agent", "Stratos/1.0");
    val headers5 = headers4.set("Accept", "application/json");

    println("HTTP Request Headers:");
    val headerKeys = headers5.keys();
    println("  " + headerKeys[0] + ": " + headers5[headerKeys[0]]);
    println("  " + headerKeys[1] + ": " + headers5[headerKeys[1]]);
    println("  " + headerKeys[2] + ": " + headers5[headerKeys[2]]);
    println("  " + headerKeys[3] + ": " + headers5[headerKeys[3]]);

    // =================================================================
    // 12. Map Persistence with Files
    // =================================================================
    printSection(12, "Saving and Loading Maps");

    val appSettings = maps.create();
    val appSettings2 = appSettings.set("theme", "dark");
    val appSettings3 = appSettings2.set("language", "en");
    val appSettings4 = appSettings3.set("notifications", "enabled");

    // Save to file (simple format: key=value)
    val settingsKeys = appSettings4.keys();
    val settingsValues = appSettings4.values();
    val fileContent = settingsKeys[0] + "=" + settingsValues[0] + "\n" +
                      settingsKeys[1] + "=" + settingsValues[1] + "\n" +
                      settingsKeys[2] + "=" + settingsValues[2];

    io.writeFile("settings.txt", fileContent);
    println("✓ Saved settings to settings.txt");

    // Read back
    val loadedContent = io.readFile("settings.txt");
    val lines = loadedContent.split("\n");
    println("✓ Loaded " + lines.length() + " settings from file");
    println("  First setting: " + lines[0]);

    // Cleanup
    io.remove("settings.txt");
    println("✓ Cleaned up settings.txt");

    // =================================================================
    // Summary
    // =================================================================
    printHeader("Map Operations Summary");

    println("\n✓ Map Creation: maps.create()");
    println("✓ Setting Values: map.set(key, value)");
    println("✓ Getting Values: map.get(key) or map[key]");
    println("✓ Key Existence: map.has(key)");
    println("✓ Size Check: map.size()");
    println("✓ Empty Check: map.isEmpty()");
    println("✓ Get All Keys: map.keys() -> array");
    println("✓ Get All Values: map.values() -> array");
    println("✓ Remove Entry: map.remove(key)");
    println("✓ Clear All: map.clear()");

    println("\n" + "=".repeat(50));
    println("All map operations completed successfully!");
    println("=".repeat(50));
}
