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
    fn next() bool {
        this.hasRow = __sqlite_step(this.handle);
        return this.hasRow;
    }

    // Get an integer column by index (0-based)
    fn getInt(index: int) int {
        return __sqlite_column_int(this.handle, index);
    }

    // Get a double column by index
    fn getDouble(index: int) double {
        return __sqlite_column_double(this.handle, index);
    }

    // Get a string column by index
    fn getString(index: int) string {
        return __sqlite_column_string(this.handle, index);
    }

    // Get a column value by name
    // Returns the value as a string (convert as needed)
    fn get(columnName: string) string {
        val index = __sqlite_column_index(this.handle, columnName);
        return this.getString(index);
    }

    // Check if a column is NULL
    fn isNull(index: int) bool {
        return __sqlite_column_is_null(this.handle, index);
    }

    // Get the number of columns in the result
    fn columnCount() int {
        return __sqlite_column_count(this.handle);
    }

    // Get the name of a column by index
    fn columnName(index: int) string {
        return __sqlite_column_name(this.handle, index);
    }

    // Close the rows
    fn close() {
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
    fn bind(index: int, value: any) {
        __sqlite_bind(this.handle, index, value);
    }

    // Bind an integer parameter
    fn bindInt(index: int, value: int) {
        __sqlite_bind_int(this.handle, index, value);
    }

    // Bind a double parameter
    fn bindDouble(index: int, value: double) {
        __sqlite_bind_double(this.handle, index, value);
    }

    // Bind a string parameter
    fn bindString(index: int, value: string) {
        __sqlite_bind_string(this.handle, index, value);
    }

    // Bind NULL parameter
    fn bindNull(index: int) {
        __sqlite_bind_null(this.handle, index);
    }

    // Execute the prepared statement (for INSERT, UPDATE, DELETE)
    // Returns the number of rows affected
    fn execute() int {
        return __sqlite_stmt_exec(this.handle);
    }

    // Query executes the prepared statement and returns rows
    fn query() Rows {
        return Rows(this.handle);
    }

    // Reset the statement to be executed again
    fn reset() {
        __sqlite_stmt_reset(this.handle);
    }

    // Close the statement
    fn close() {
        __sqlite_stmt_close(this.handle);
    }
}

// Transaction represents a database transaction
class Transaction {
    var db: any;
    var committed: bool;

    constructor(database: any) {
        this.db = database;
        this.committed = false;
    }

    // Commit the transaction
    fn commit() {
        if !this.committed {
            this.db.exec("COMMIT");
            this.committed = true;
        }
    }

    // Rollback the transaction
    fn rollback() {
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
    fn exec(sql: string) int {
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
    fn query(sql: string) Rows {
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
    fn prepare(sql: string) Statement {
        val stmtHandle = __sqlite_prepare(this.handle, sql);
        return Statement(stmtHandle);
    }

    // Begin starts a transaction
    // Returns a Transaction object
    fn begin() Transaction {
        this.exec("BEGIN TRANSACTION");
        return Transaction(this);
    }

    // Close closes the database connection
    // Should always be called when done with the database
    fn close() {
        __sqlite_close(this.handle);
    }

    // Get the last inserted row ID
    fn lastInsertId() int {
        return __sqlite_last_insert_id(this.handle);
    }

    // Get the number of changes made by the last statement
    fn changes() int {
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
