// SQLite Database Support for Stratos
// Inspired by Go's database/sql and Kotlin's JDBC patterns
package db;

// ============================================================================
// Native function declarations (implemented in C++)
// ============================================================================

fn __sqlite_open(path: string) any;
fn __sqlite_close(handle: any) void;
fn __sqlite_exec(handle: any, sql: string) int;
fn __sqlite_query(handle: any, sql: string) any;
fn __sqlite_prepare(handle: any, sql: string) any;
fn __sqlite_bind(handle: any, index: int, value: any) void;
fn __sqlite_bind_int(handle: any, index: int, value: int) void;
fn __sqlite_bind_double(handle: any, index: int, value: double) void;
fn __sqlite_bind_string(handle: any, index: int, value: string) void;
fn __sqlite_bind_null(handle: any, index: int) void;
fn __sqlite_stmt_exec(handle: any) int;
fn __sqlite_stmt_reset(handle: any) void;
fn __sqlite_stmt_close(handle: any) void;
fn __sqlite_step(handle: any) bool;
fn __sqlite_column_int(handle: any, index: int) int;
fn __sqlite_column_double(handle: any, index: int) double;
fn __sqlite_column_string(handle: any, index: int) string;
fn __sqlite_column_index(handle: any, name: string) int;
fn __sqlite_column_is_null(handle: any, index: int) bool;
fn __sqlite_column_count(handle: any) int;
fn __sqlite_column_name(handle: any, index: int) string;
fn __sqlite_rows_close(handle: any) void;
fn __sqlite_last_insert_id(handle: any) int;
fn __sqlite_changes(handle: any) int;

// ============================================================================
// Class Definitions
// ============================================================================

// Rows represents the result set from a query
class Rows {
    var handle: any;
    var hasRow: bool;

    constructor(stmtHandle: any) {
        this.handle = stmtHandle;
        this.hasRow = false;
    }

    // Move to the next row
    // Returns true if there is a row, false if no more rows
    pub fn next() bool {
        this.hasRow = __sqlite_step(this.handle);
        return this.hasRow;
    }

    // Get an integer column by index (0-based)
    pub fn getInt(index: int) int {
        return __sqlite_column_int(this.handle, index);
    }

    // Get a double column by index
    pub fn getDouble(index: int) double {
        return __sqlite_column_double(this.handle, index);
    }

    // Get a string column by index
    pub fn getString(index: int) string {
        return __sqlite_column_string(this.handle, index);
    }

    // Get a column value by name
    // Returns the value as a string (convert as needed)
    pub fn get(columnName: string) string {
        val index = __sqlite_column_index(this.handle, columnName);
        return this.getString(index);
    }

    // Check if a column is NULL
    pub fn isNull(index: int) bool {
        return __sqlite_column_is_null(this.handle, index);
    }

    // Get the number of columns in the result
    pub fn columnCount() int {
        return __sqlite_column_count(this.handle);
    }

    // Get the name of a column by index
    pub fn columnName(index: int) string {
        return __sqlite_column_name(this.handle, index);
    }

    // Close the rows
    pub fn close() {
        __sqlite_rows_close(this.handle);
    }
}

// Statement represents a prepared SQL statement
class Statement {
    var handle: any;

    constructor(stmtHandle: any) {
        this.handle = stmtHandle;
    }

    // Bind a value to a parameter (1-indexed)
    // Supports int, double, string, and null
    //
    // Example:
    //   stmt.bind(1, "Alice");
    //   stmt.bind(2, 25);
    pub fn bind(index: int, value: any) {
        __sqlite_bind(this.handle, index, value);
    }

    // Bind an integer parameter
    pub fn bindInt(index: int, value: int) {
        __sqlite_bind_int(this.handle, index, value);
    }

    // Bind a double parameter
    pub fn bindDouble(index: int, value: double) {
        __sqlite_bind_double(this.handle, index, value);
    }

    // Bind a string parameter
    pub fn bindString(index: int, value: string) {
        __sqlite_bind_string(this.handle, index, value);
    }

    // Bind NULL parameter
    pub fn bindNull(index: int) {
        __sqlite_bind_null(this.handle, index);
    }

    // Execute the prepared statement (for INSERT, UPDATE, DELETE)
    // Returns the number of rows affected
    pub fn execute() int {
        return __sqlite_stmt_exec(this.handle);
    }

    // Query executes the prepared statement and returns rows
    pub fn query() Rows {
        return Rows(this.handle);
    }

    // Reset the statement to be executed again
    pub fn reset() {
        __sqlite_stmt_reset(this.handle);
    }

    // Close the statement
    pub fn close() {
        __sqlite_stmt_close(this.handle);
    }
}

// Transaction represents a database transaction
class Transaction {
    var db: Database;
    var committed: bool;

    constructor(database: Database) {
        this.db = database;
        this.committed = false;
    }

    // Commit the transaction
    pub fn commit() {
        if !this.committed {
            this.db.exec("COMMIT");
            this.committed = true;
        }
    }

    // Rollback the transaction
    pub fn rollback() {
        if !this.committed {
            this.db.exec("ROLLBACK");
            this.committed = true;
        }
    }
}

// Database represents a connection to a SQLite database
class Database {
    var handle: any;

    constructor(dbHandle: any) {
        this.handle = dbHandle;
    }

    // Execute a SQL statement that doesn't return rows (DDL, INSERT, UPDATE, DELETE)
    // Returns the number of rows affected
    //
    // Example:
    //   db.exec("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    //   val affected = db.exec("DELETE FROM users WHERE id = 5");
    pub fn exec(sql: string) int {
        return __sqlite_exec(this.handle, sql);
    }

    // Query executes a SQL statement that returns rows
    // Returns a Rows object for iterating results
    //
    // Example:
    //   val rows = db.query("SELECT * FROM users");
    //   while rows.next() {
    //       println(rows.getString("name"));
    //   }
    pub fn query(sql: string) Rows {
        val stmtHandle = __sqlite_query(this.handle, sql);
        return Rows(stmtHandle);
    }

    // Prepare creates a prepared statement for later execution
    // Use this for repeated queries or when binding parameters
    //
    // Example:
    //   val stmt = db.prepare("INSERT INTO users (name, age) VALUES (?, ?)");
    //   stmt.bind(1, "Alice");
    //   stmt.bind(2, 25);
    //   stmt.execute();
    pub fn prepare(sql: string) Statement {
        val stmtHandle = __sqlite_prepare(this.handle, sql);
        return Statement(stmtHandle);
    }

    // Begin starts a transaction
    // Returns a Transaction object
    pub fn begin() Transaction {
        this.exec("BEGIN TRANSACTION");
        return Transaction(this);
    }

    // Close closes the database connection
    // Should always be called when done with the database
    pub fn close() {
        __sqlite_close(this.handle);
    }

    // Get the last inserted row ID
    pub fn lastInsertId() int {
        return __sqlite_last_insert_id(this.handle);
    }

    // Get the number of changes made by the last statement
    pub fn changes() int {
        return __sqlite_changes(this.handle);
    }
}

// ============================================================================
// Public API Functions
// ============================================================================

// Open a SQLite database connection
// Returns a Database handle or throws an error if connection fails
//
// Examples:
//   val db = db.open("mydb.db");              // File database
//   val db = db.open(":memory:");              // In-memory database
fn open(path: string) Database {
    val handle = __sqlite_open(path);
    return Database(handle);
}
