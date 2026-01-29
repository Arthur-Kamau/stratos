# PostgreSQL Database

Stratos provides native PostgreSQL support through the `psql` module, offering a type-safe API for connecting to and querying PostgreSQL databases.

## Getting Started

Import the module and create a connection:

```stratos
package main;

use db.psql;

fn main() {
    // Connect using connection string
    val conn = psql.connect("host=localhost port=5432 dbname=mydb user=postgres password=secret");

    // Or use URI format
    val conn2 = psql.connect("postgresql://postgres:password@localhost/mydb");

    // Don't forget to close when done
    conn.close();
}
```

## Connection Strings

PostgreSQL supports two connection string formats:

**Key-Value Format:**
```stratos
val conn = psql.connect("host=localhost port=5432 dbname=mydb user=postgres password=secret");
```

**URI Format:**
```stratos
val conn = psql.connect("postgresql://user:password@host:port/database");
```

You can also use the helper function:

```stratos
val connStr = psql.connectionString("localhost", 5432, "mydb", "postgres", "secret");
val conn = psql.connect(connStr);
```

## Executing Queries

### Simple Execution (DDL, INSERT, UPDATE, DELETE)

Use `exec()` for statements that don't return rows:

```stratos
// Create a table
conn.exec("CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(255) UNIQUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)");

// Insert data
val affected = conn.exec("INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com')");
println("Inserted " + affected + " rows");

// Update data
val updated = conn.exec("UPDATE users SET name = 'Bob' WHERE id = 1");
```

### Parameterized Queries

Always use parameterized queries for user input to prevent SQL injection:

```stratos
// Insert with parameters (use $1, $2, etc.)
conn.execParams(
    "INSERT INTO users (name, email) VALUES ($1, $2)",
    ["Alice", "alice@example.com"]
);

// Update with parameters
conn.execParams(
    "UPDATE users SET email = $1 WHERE id = $2",
    ["newemail@example.com", "1"]
);
```

::: warning SQL Injection Prevention
Always use parameterized queries when including user input. Never concatenate user data directly into SQL strings.
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

rows.close();  // Always close result sets
```

### Parameterized Queries

```stratos
val rows = conn.queryParams(
    "SELECT * FROM users WHERE created_at > $1",
    ["2024-01-01"]
);

while rows.next() {
    println(rows.getString(1));  // name column
}

rows.close();
```

### Column Access

Access columns by index (0-based) or by name:

```stratos
val rows = conn.query("SELECT id, name, email FROM users");

while rows.next() {
    // By index
    val id = rows.getInt(0);
    val name = rows.getString(1);

    // By name
    val email = rows.get("email");

    // Check for NULL values
    if rows.isNull(2) {
        println("Email is null");
    }
}

rows.close();
```

### Available Column Methods

| Method | Description |
|--------|-------------|
| `getInt(index)` | Get integer value |
| `getDouble(index)` | Get double/float value |
| `getString(index)` | Get string value |
| `getBool(index)` | Get boolean value |
| `get(columnName)` | Get value by column name (as string) |
| `isNull(index)` | Check if value is NULL |
| `columnCount()` | Get number of columns |
| `columnName(index)` | Get column name by index |
| `rowCount()` | Get total row count |

## Prepared Statements

For frequently executed queries, use prepared statements:

```stratos
// Prepare the statement with a name
conn.prepare("get_user", "SELECT * FROM users WHERE id = $1");
conn.prepare("insert_user", "INSERT INTO users (name, email) VALUES ($1, $2) RETURNING id");

// Execute prepared statements
val rows = conn.execPrepared("get_user", ["1"]);
while rows.next() {
    println(rows.getString(1));
}
rows.close();

// Insert and get the new ID
val insertResult = conn.execPrepared("insert_user", ["Charlie", "charlie@example.com"]);
if insertResult.next() {
    println("New user ID: " + insertResult.getInt(0));
}
insertResult.close();
```

## Transactions

Use transactions for operations that should be atomic:

```stratos
// Start a transaction
val tx = conn.begin();

try {
    tx.execParams("INSERT INTO accounts (user_id, balance) VALUES ($1, $2)", ["1", "1000"]);
    tx.execParams("UPDATE users SET has_account = true WHERE id = $1", ["1"]);

    // Commit if all operations succeed
    tx.commit();
} catch (e) {
    // Rollback on error
    tx.rollback();
    println("Transaction failed: " + e.message);
}
```

### Transaction Methods

| Method | Description |
|--------|-------------|
| `commit()` | Commit the transaction |
| `rollback()` | Rollback the transaction |
| `exec(sql)` | Execute SQL within transaction |
| `execParams(sql, params)` | Execute parameterized SQL |
| `query(sql)` | Query within transaction |
| `queryParams(sql, params)` | Parameterized query |

## String Escaping

When you can't use parameterized queries, escape strings properly:

```stratos
val userInput = "O'Brien";
val safe = conn.escapeString(userInput);  // Returns 'O''Brien'

// For table/column names (identifiers)
val tableName = "user data";
val safeTable = conn.escapeIdentifier(tableName);  // Returns "user data"
```

::: tip Prefer Parameters
Always prefer parameterized queries over string escaping when possible.
:::

## Connection Status

Check if a connection is still active:

```stratos
val status = conn.status();
if status == "connected" {
    println("Connection is active");
} else {
    println("Connection lost: " + status);
}
```

## Complete Example

Here's a complete example demonstrating common patterns:

```stratos
package main;

use db.psql;
use log;

fn main() {
    // Connect to database
    val conn = psql.connect("host=localhost dbname=myapp user=postgres password=secret");

    // Create schema
    conn.exec("
        CREATE TABLE IF NOT EXISTS products (
            id SERIAL PRIMARY KEY,
            name VARCHAR(100) NOT NULL,
            price DECIMAL(10,2) NOT NULL,
            stock INT DEFAULT 0
        )
    ");

    // Insert products
    val products = [
        ["Laptop", "999.99", "10"],
        ["Mouse", "29.99", "100"],
        ["Keyboard", "79.99", "50"]
    ];

    for product in products {
        conn.execParams(
            "INSERT INTO products (name, price, stock) VALUES ($1, $2, $3)",
            product
        );
    }

    // Query with filtering
    val rows = conn.queryParams(
        "SELECT name, price, stock FROM products WHERE price < $1 ORDER BY price",
        ["100"]
    );

    println("Products under $100:");
    while rows.next() {
        val name = rows.getString(0);
        val price = rows.getDouble(1);
        val stock = rows.getInt(2);
        println("  " + name + ": $" + price + " (" + stock + " in stock)");
    }
    rows.close();

    // Update stock in a transaction
    val tx = conn.begin();
    tx.execParams("UPDATE products SET stock = stock - 1 WHERE name = $1", ["Mouse"]);
    tx.execParams("INSERT INTO orders (product_name) VALUES ($1)", ["Mouse"]);
    tx.commit();

    conn.close();
}
```

## Error Handling

PostgreSQL operations throw exceptions on errors:

```stratos
try {
    val conn = psql.connect("invalid connection string");
} catch (e) {
    log.error("Connection failed: " + e.message);
}

try {
    conn.exec("SELECT * FROM nonexistent_table");
} catch (e) {
    log.error("Query failed: " + e.message);
}
```

## Best Practices

1. **Always close connections and result sets** - Use `conn.close()` and `rows.close()`
2. **Use parameterized queries** - Prevents SQL injection attacks
3. **Use transactions for related operations** - Ensures data consistency
4. **Handle errors gracefully** - Wrap database operations in try-catch
5. **Use connection pooling** - For high-traffic applications (implement at application level)
