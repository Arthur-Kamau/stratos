// SQLite Database Example for Stratos
// Demonstrates database operations inspired by Go's database/sql and Kotlin's JDBC

package main;

use db;

fn main() {
    println("=== Stratos SQLite Database Example ===\n");

    // Open a database (creates if doesn't exist)
    println("Opening database...");
    val dbHandle = db.__sqlite_open("test.db");
    println("Database opened successfully!\n");

    // Create a table
    println("Creating users table...");
    db.exec("DROP TABLE IF EXISTS users");
    db.exec("CREATE TABLE users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, age INTEGER, email TEXT)");
    println("Table created!\n");

    // Insert data using exec
    println("Inserting users...");
    db.exec("INSERT INTO users (name, age, email) VALUES ('Alice', 25, 'alice@example.com')");
    db.exec("INSERT INTO users (name, age, email) VALUES ('Bob', 30, 'bob@example.com')");
    db.exec("INSERT INTO users (name, age, email) VALUES ('Charlie', 35, 'charlie@example.com')");
    println("Users inserted!\n");

    // Insert using prepared statement
    println("Inserting user with prepared statement...");
    val stmt = db.prepare("INSERT INTO users (name, age, email) VALUES (?, ?, ?)");
    stmt.bindString(1, "Diana");
    stmt.bindInt(2, 28);
    stmt.bindString(3, "diana@example.com");
    stmt.execute();
    stmt.close();
    println("User inserted via prepared statement!\n");

    // Query all users
    println("Querying all users:");
    println("ID | Name    | Age | Email");
    println("---+---------|-----|-------------------");

    val rows = db.query("SELECT id, name, age, email FROM users ORDER BY id");
    while rows.next() {
        val id = rows.getInt(0);
        val name = rows.getString(1);
        val age = rows.getInt(2);
        val email = rows.getString(3);
        println(id + "  | " + name + " | " + age + "  | " + email);
    }
    rows.close();
    println("");

    // Update a record
    println("Updating Bob's age to 31...");
    val affected = db.exec("UPDATE users SET age = 31 WHERE name = 'Bob'");
    println("Rows affected: " + affected + "\n");

    // Query with prepared statement
    println("Finding users over 30:");
    val queryStmt = db.prepare("SELECT name, age FROM users WHERE age > ?");
    queryStmt.bindInt(1, 30);
    val resultRows = queryStmt.query();

    while resultRows.next() {
        val name = resultRows.getString(0);
        val age = resultRows.getInt(1);
        println("- " + name + " (age " + age + ")");
    }
    resultRows.close();
    queryStmt.close();
    println("");

    // Transaction example
    println("Testing transaction...");
    val tx = db.begin();
    db.exec("INSERT INTO users (name, age) VALUES ('Eve', 22)");
    db.exec("INSERT INTO users (name, age) VALUES ('Frank', 27)");
    tx.commit();
    println("Transaction committed!\n");

    // Count total users
    val countRows = db.query("SELECT COUNT(*) as count FROM users");
    if countRows.next() {
        val count = countRows.getInt(0);
        println("Total users in database: " + count + "\n");
    }
    countRows.close();

    // Delete a user
    println("Deleting user Charlie...");
    db.exec("DELETE FROM users WHERE name = 'Charlie'");
    println("User deleted!\n");

    // Final count
    val finalCountRows = db.query("SELECT COUNT(*) FROM users");
    if finalCountRows.next() {
        println("Final user count: " + finalCountRows.getInt(0));
    }
    finalCountRows.close();

    // Close database
    db.close();
    println("\nDatabase closed. Example complete!");
}
