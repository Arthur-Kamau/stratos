package main;

use db;

fn main() {
    println("Testing SQLite database...");

    // Open database
    val database = db.open("./examples/database/test.db");
    println("Database opened!");

    // Create table
    database.exec("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, name TEXT)");
    println("Table created!");

    // Insert data
    database.exec("INSERT INTO test (name) VALUES ('Hello SQLite')");
    println("Data inserted!");

    // Query data
    val rows = database.query("SELECT * FROM test");
    println("Query prepared!");

    while rows.next() {
        val id = rows.getInt(0);
        val name = rows.getString(1);
        println("Found row: id=" + id + ", name=" + name);
    }

    rows.close();
    database.close();

    println("Test complete!");
}
