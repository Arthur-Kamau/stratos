// MySQL/MariaDB Database Support for Stratos
// Inspired by Go's database/sql and PHP's mysqli
package mysql;

// ============================================================================
// Native function declarations (implemented in C++)
// ============================================================================

fn __mysql_connect(host: string, user: string, password: string, database: string, port: int) any;
fn __mysql_close(handle: any) void;
fn __mysql_exec(handle: any, sql: string) int;
fn __mysql_query(handle: any, sql: string) int;
fn __mysql_prepare(handle: any, sql: string) any;
fn __mysql_stmt_close(handle: any) void;
fn __mysql_begin(handle: any) bool;
fn __mysql_commit(handle: any) bool;
fn __mysql_rollback(handle: any) bool;
fn __mysql_next(resultId: int) bool;
fn __mysql_get_int(resultId: int, col: int) int;
fn __mysql_get_double(resultId: int, col: int) double;
fn __mysql_get_string(resultId: int, col: int) string;
fn __mysql_get_bool(resultId: int, col: int) bool;
fn __mysql_is_null(resultId: int, col: int) bool;
fn __mysql_column_count(resultId: int) int;
fn __mysql_column_name(resultId: int, col: int) string;
fn __mysql_rows_close(resultId: int) void;
fn __mysql_last_insert_id(handle: any) int;
fn __mysql_escape_string(handle: any, str: string) string;

// ============================================================================
// Class Definitions
// ============================================================================

// Rows represents the result set from a query
class Rows {
    var resultId: int;
    var hasRow: bool;

    constructor(id: int) {
        this.resultId = id;
        this.hasRow = false;
    }

    // Move to the next row
    // Returns true if there is a row, false if no more rows
    pub fn next() bool {
        this.hasRow = __mysql_next(this.resultId);
        return this.hasRow;
    }

    // Get an integer column by index (0-based)
    pub fn getInt(index: int) int {
        return __mysql_get_int(this.resultId, index);
    }

    // Get a double column by index
    pub fn getDouble(index: int) double {
        return __mysql_get_double(this.resultId, index);
    }

    // Get a string column by index
    pub fn getString(index: int) string {
        return __mysql_get_string(this.resultId, index);
    }

    // Get a boolean column by index
    pub fn getBool(index: int) bool {
        return __mysql_get_bool(this.resultId, index);
    }

    // Check if a column is NULL
    pub fn isNull(index: int) bool {
        return __mysql_is_null(this.resultId, index);
    }

    // Get the number of columns in the result
    pub fn columnCount() int {
        return __mysql_column_count(this.resultId);
    }

    // Get the name of a column by index
    pub fn columnName(index: int) string {
        return __mysql_column_name(this.resultId, index);
    }

    // Close the rows and free resources
    pub fn close() {
        __mysql_rows_close(this.resultId);
    }
}

// Statement represents a prepared SQL statement
class Statement {
    var handle: any;

    constructor(stmtHandle: any) {
        this.handle = stmtHandle;
    }

    // Close the prepared statement
    pub fn close() {
        __mysql_stmt_close(this.handle);
    }
}

// Transaction represents a database transaction
class Transaction {
    var conn: Connection;
    var committed: bool;

    constructor(connection: Connection) {
        this.conn = connection;
        this.committed = false;
    }

    // Commit the transaction
    pub fn commit() {
        if !this.committed {
            __mysql_commit(this.conn.handle);
            this.committed = true;
        }
    }

    // Rollback the transaction
    pub fn rollback() {
        if !this.committed {
            __mysql_rollback(this.conn.handle);
            this.committed = true;
        }
    }

    // Execute SQL within the transaction
    pub fn exec(sql: string) int {
        return this.conn.exec(sql);
    }

    // Query within the transaction
    pub fn query(sql: string) Rows {
        return this.conn.query(sql);
    }
}

// Connection represents a connection to a MySQL/MariaDB database
class Connection {
    var handle: any;

    constructor(dbHandle: any) {
        this.handle = dbHandle;
    }

    // Execute a SQL statement that doesn't return rows
    // Returns the number of rows affected
    //
    // Example:
    //   conn.exec("CREATE TABLE users (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255))");
    //   val affected = conn.exec("DELETE FROM users WHERE id = 5");
    pub fn exec(sql: string) int {
        return __mysql_exec(this.handle, sql);
    }

    // Query executes a SQL statement that returns rows
    //
    // Example:
    //   val rows = conn.query("SELECT * FROM users");
    //   while rows.next() {
    //       println(rows.getString(1));
    //   }
    pub fn query(sql: string) Rows {
        val resultId = __mysql_query(this.handle, sql);
        return Rows(resultId);
    }

    // Prepare creates a prepared statement
    //
    // Example:
    //   val stmt = conn.prepare("INSERT INTO users (name) VALUES (?)");
    pub fn prepare(sql: string) Statement {
        val stmtHandle = __mysql_prepare(this.handle, sql);
        return Statement(stmtHandle);
    }

    // Begin starts a transaction
    pub fn begin() Transaction {
        __mysql_begin(this.handle);
        return Transaction(this);
    }

    // Close closes the database connection
    pub fn close() {
        __mysql_close(this.handle);
    }

    // Get the last inserted row ID
    pub fn lastInsertId() int {
        return __mysql_last_insert_id(this.handle);
    }

    // Escape a string value for safe inclusion in SQL
    pub fn escapeString(str: string) string {
        return __mysql_escape_string(this.handle, str);
    }
}

// ============================================================================
// Public API Functions
// ============================================================================

// Connect to a MySQL/MariaDB database
//
// Example:
//   val conn = mysql.connect("localhost", "root", "password", "mydb", 3306);
fn connect(host: string, user: string, password: string, database: string, port: int) Connection {
    val handle = __mysql_connect(host, user, password, database, port);
    return Connection(handle);
}

// Connect with default port (3306)
fn connectDefault(host: string, user: string, password: string, database: string) Connection {
    return connect(host, user, password, database, 3306);
}

// Connect to localhost with default port
fn connectLocal(user: string, password: string, database: string) Connection {
    return connect("localhost", user, password, database, 3306);
}
