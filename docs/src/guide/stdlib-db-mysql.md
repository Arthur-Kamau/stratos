# MySQL/MariaDB Database

Stratos provides native MySQL and MariaDB support through the `mysql` module. MariaDB is fully compatible as it uses the same wire protocol.

## Getting Started

Import the module and create a connection:

```stratos
package main;

use db.mysql;

fn main() {
    // Connect to MySQL
    val conn = mysql.connect("localhost", "root", "password", "mydb", 3306);

    // Or use helper for default port
    val conn2 = mysql.connectDefault("localhost", "root", "password", "mydb");

    // Or connect to localhost with defaults
    val conn3 = mysql.connectLocal("root", "password", "mydb");

    conn.close();
}
```

## Connection Options

```stratos
// Full connection with all parameters
val conn = mysql.connect(
    "localhost",     // host
    "username",      // user
    "password",      // password
    "database",      // database name
    3306            // port
);

// Connect with default port (3306)
val conn = mysql.connectDefault("localhost", "user", "pass", "mydb");

// Connect to localhost
val conn = mysql.connectLocal("user", "pass", "mydb");
```

## Executing Queries

### Simple Execution

```stratos
// Create a table
conn.exec("CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(255) UNIQUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)");

// Insert data
val affected = conn.exec("INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com')");
println("Inserted " + affected + " rows");

// Get the last insert ID
val newId = conn.lastInsertId();
println("New user ID: " + newId);
```

### Safe String Escaping

When you need to include user data in queries:

```stratos
val userInput = "O'Malley";
val safe = conn.escapeString(userInput);
conn.exec("INSERT INTO users (name) VALUES ('" + safe + "')");
```

::: warning
Always escape user input or use application-level parameter binding to prevent SQL injection.
:::

## Querying Data

### Basic Queries

```stratos
val rows = conn.query("SELECT id, name, email FROM users");

while rows.next() {
    val id = rows.getInt(0);
    val name = rows.getString(1);
    val email = rows.getString(2);
    println("User " + id + ": " + name + " <" + email + ">");
}

rows.close();
```

### Column Access Methods

| Method | Description |
|--------|-------------|
| `getInt(index)` | Get integer value |
| `getDouble(index)` | Get double/float value |
| `getString(index)` | Get string value |
| `getBool(index)` | Get boolean value |
| `isNull(index)` | Check if value is NULL |
| `columnCount()` | Get number of columns |
| `columnName(index)` | Get column name |

### Working with Results

```stratos
val rows = conn.query("SELECT * FROM products WHERE category = 'electronics'");

// Check column metadata
println("Columns: " + rows.columnCount());
for i in 0..rows.columnCount() {
    println("  " + rows.columnName(i));
}

// Iterate through results
while rows.next() {
    if rows.isNull(3) {
        println("Description is null");
    } else {
        println(rows.getString(3));
    }
}

rows.close();
```

## Prepared Statements

```stratos
// Prepare a statement
val stmt = conn.prepare("SELECT * FROM users WHERE id = ?");

// Use it multiple times (note: binding happens via query construction)
// For now, use escapeString for safe values
val userId = conn.escapeString(userInputId);
val rows = conn.query("SELECT * FROM users WHERE id = " + userId);

rows.close();
stmt.close();
```

## Transactions

Use transactions to ensure data consistency:

```stratos
// Start a transaction
val tx = conn.begin();

try {
    tx.exec("INSERT INTO orders (user_id, total) VALUES (1, 99.99)");
    tx.exec("UPDATE inventory SET stock = stock - 1 WHERE product_id = 123");
    tx.exec("INSERT INTO order_items (order_id, product_id) VALUES (LAST_INSERT_ID(), 123)");

    // Commit on success
    tx.commit();
    println("Order placed successfully");
} catch (e) {
    // Rollback on any error
    tx.rollback();
    println("Order failed: " + e.message);
}
```

### Transaction Methods

| Method | Description |
|--------|-------------|
| `commit()` | Commit all changes |
| `rollback()` | Undo all changes |
| `exec(sql)` | Execute SQL in transaction |
| `query(sql)` | Query in transaction |

## Complete Example

```stratos
package main;

use db.mysql;
use log;

fn main() {
    // Connect to database
    val conn = mysql.connectLocal("root", "password", "shop");

    // Create tables
    conn.exec("
        CREATE TABLE IF NOT EXISTS categories (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(50) NOT NULL
        )
    ");

    conn.exec("
        CREATE TABLE IF NOT EXISTS products (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(100) NOT NULL,
            category_id INT,
            price DECIMAL(10,2) NOT NULL,
            FOREIGN KEY (category_id) REFERENCES categories(id)
        )
    ");

    // Insert categories
    conn.exec("INSERT INTO categories (name) VALUES ('Electronics'), ('Books'), ('Clothing')");

    // Insert products with proper escaping
    val products = [
        ["Laptop", "1", "999.99"],
        ["JavaScript Guide", "2", "39.99"],
        ["T-Shirt", "3", "19.99"]
    ];

    for p in products {
        val name = conn.escapeString(p[0]);
        val catId = p[1];
        val price = p[2];
        conn.exec("INSERT INTO products (name, category_id, price) VALUES ('" +
                  name + "', " + catId + ", " + price + ")");
    }

    // Query with JOIN
    val rows = conn.query("
        SELECT p.name, c.name, p.price
        FROM products p
        JOIN categories c ON p.category_id = c.id
        ORDER BY p.price DESC
    ");

    println("Products by price (highest first):");
    while rows.next() {
        val product = rows.getString(0);
        val category = rows.getString(1);
        val price = rows.getDouble(2);
        println("  " + product + " (" + category + "): $" + price);
    }
    rows.close();

    conn.close();
}
```

## MariaDB Compatibility

The `mysql` module works seamlessly with MariaDB:

```stratos
// MariaDB uses the same connection method
val conn = mysql.connect("localhost", "root", "password", "mydb", 3306);

// All operations work identically
conn.exec("CREATE TABLE test (id INT PRIMARY KEY)");
```

## Error Handling

```stratos
try {
    val conn = mysql.connect("invalid-host", "user", "pass", "db", 3306);
} catch (e) {
    log.error("Connection failed: " + e.message);
}

try {
    conn.exec("INVALID SQL SYNTAX");
} catch (e) {
    log.error("Query error: " + e.message);
}
```

## Best Practices

1. **Always escape user input** - Use `conn.escapeString()` for any user-provided data
2. **Close connections and results** - Prevent resource leaks
3. **Use transactions** - For multiple related operations
4. **Handle errors** - Wrap database operations in try-catch
5. **Limit result sets** - Use `LIMIT` clause for large tables
