// PostgreSQL Database Support for Stratos
// Inspired by Go's database/sql and Rust's tokio-postgres
package psql;

// ============================================================================
// Native function declarations (implemented in C++)
// ============================================================================

fn __psql_connect(connectionString: string) any;
fn __psql_close(handle: any) void;
fn __psql_exec(handle: any, sql: string) int;
fn __psql_query(handle: any, sql: string) any;
fn __psql_query_params(handle: any, sql: string, params: array<string>) any;
fn __psql_exec_params(handle: any, sql: string, params: array<string>) int;
fn __psql_prepare(handle: any, name: string, sql: string) bool;
fn __psql_exec_prepared(handle: any, name: string, params: array<string>) any;
fn __psql_begin(handle: any) bool;
fn __psql_commit(handle: any) bool;
fn __psql_rollback(handle: any) bool;
fn __psql_next(resultId: int) bool;
fn __psql_get_int(resultId: int, col: int) int;
fn __psql_get_double(resultId: int, col: int) double;
fn __psql_get_string(resultId: int, col: int) string;
fn __psql_get_bool(resultId: int, col: int) bool;
fn __psql_is_null(resultId: int, col: int) bool;
fn __psql_column_count(resultId: int) int;
fn __psql_column_name(resultId: int, col: int) string;
fn __psql_column_index(resultId: int, name: string) int;
fn __psql_row_count(resultId: int) int;
fn __psql_rows_close(resultId: int) void;
fn __psql_escape_string(handle: any, str: string) string;
fn __psql_escape_identifier(handle: any, str: string) string;
fn __psql_status(handle: any) string;

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
        this.hasRow = __psql_next(this.resultId);
        return this.hasRow;
    }

    // Get an integer column by index (0-based)
    pub fn getInt(index: int) int {
        return __psql_get_int(this.resultId, index);
    }

    // Get a double column by index
    pub fn getDouble(index: int) double {
        return __psql_get_double(this.resultId, index);
    }

    // Get a string column by index
    pub fn getString(index: int) string {
        return __psql_get_string(this.resultId, index);
    }

    // Get a boolean column by index
    pub fn getBool(index: int) bool {
        return __psql_get_bool(this.resultId, index);
    }

    // Get a column value by name
    pub fn get(columnName: string) string {
        val index = __psql_column_index(this.resultId, columnName);
        return this.getString(index);
    }

    // Check if a column is NULL
    pub fn isNull(index: int) bool {
        return __psql_is_null(this.resultId, index);
    }

    // Get the number of columns in the result
    pub fn columnCount() int {
        return __psql_column_count(this.resultId);
    }

    // Get the name of a column by index
    pub fn columnName(index: int) string {
        return __psql_column_name(this.resultId, index);
    }

    // Get the total number of rows in the result
    pub fn rowCount() int {
        return __psql_row_count(this.resultId);
    }

    // Close the rows and free resources
    pub fn close() {
        __psql_rows_close(this.resultId);
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
            __psql_commit(this.conn.handle);
            this.committed = true;
        }
    }

    // Rollback the transaction
    pub fn rollback() {
        if !this.committed {
            __psql_rollback(this.conn.handle);
            this.committed = true;
        }
    }

    // Execute SQL within the transaction
    pub fn exec(sql: string) int {
        return this.conn.exec(sql);
    }

    // Execute parameterized SQL within the transaction
    pub fn execParams(sql: string, params: array<string>) int {
        return this.conn.execParams(sql, params);
    }

    // Query within the transaction
    pub fn query(sql: string) Rows {
        return this.conn.query(sql);
    }

    // Parameterized query within the transaction
    pub fn queryParams(sql: string, params: array<string>) Rows {
        return this.conn.queryParams(sql, params);
    }
}

// Connection represents a connection to a PostgreSQL database
class Connection {
    var handle: any;

    constructor(dbHandle: any) {
        this.handle = dbHandle;
    }

    // Execute a SQL statement that doesn't return rows
    // Returns the number of rows affected
    //
    // Example:
    //   conn.exec("CREATE TABLE users (id SERIAL PRIMARY KEY, name TEXT)");
    //   val affected = conn.exec("DELETE FROM users WHERE id = 5");
    pub fn exec(sql: string) int {
        return __psql_exec(this.handle, sql);
    }

    // Execute a parameterized SQL statement
    // Parameters use $1, $2, etc. placeholders
    //
    // Example:
    //   conn.execParams("INSERT INTO users (name, email) VALUES ($1, $2)", ["Alice", "alice@example.com"]);
    pub fn execParams(sql: string, params: array<string>) int {
        return __psql_exec_params(this.handle, sql, params);
    }

    // Query executes a SQL statement that returns rows
    //
    // Example:
    //   val rows = conn.query("SELECT * FROM users");
    //   while rows.next() {
    //       println(rows.getString(1));
    //   }
    pub fn query(sql: string) Rows {
        val resultId = __psql_query(this.handle, sql);
        return Rows(resultId);
    }

    // Execute a parameterized query
    //
    // Example:
    //   val rows = conn.queryParams("SELECT * FROM users WHERE age > $1", ["25"]);
    pub fn queryParams(sql: string, params: array<string>) Rows {
        val resultId = __psql_query_params(this.handle, sql, params);
        return Rows(resultId);
    }

    // Prepare a named statement for later execution
    //
    // Example:
    //   conn.prepare("get_user", "SELECT * FROM users WHERE id = $1");
    //   val rows = conn.execPrepared("get_user", ["1"]);
    pub fn prepare(name: string, sql: string) bool {
        return __psql_prepare(this.handle, name, sql);
    }

    // Execute a prepared statement
    pub fn execPrepared(name: string, params: array<string>) Rows {
        val resultId = __psql_exec_prepared(this.handle, name, params);
        return Rows(resultId);
    }

    // Begin starts a transaction
    pub fn begin() Transaction {
        __psql_begin(this.handle);
        return Transaction(this);
    }

    // Close closes the database connection
    pub fn close() {
        __psql_close(this.handle);
    }

    // Escape a string value for safe inclusion in SQL
    pub fn escapeString(str: string) string {
        return __psql_escape_string(this.handle, str);
    }

    // Escape an identifier (table/column name)
    pub fn escapeIdentifier(str: string) string {
        return __psql_escape_identifier(this.handle, str);
    }

    // Get the connection status
    pub fn status() string {
        return __psql_status(this.handle);
    }
}

// ============================================================================
// Public API Functions
// ============================================================================

// Connect to a PostgreSQL database
// Connection string format: "host=localhost port=5432 dbname=mydb user=postgres password=secret"
// or URI format: "postgresql://user:password@localhost:5432/mydb"
//
// Examples:
//   val conn = psql.connect("host=localhost dbname=test user=postgres");
//   val conn = psql.connect("postgresql://postgres:password@localhost/mydb");
fn connect(connectionString: string) Connection {
    val handle = __psql_connect(connectionString);
    return Connection(handle);
}

// ============================================================================
// Helper Functions
// ============================================================================

// Build a connection string from individual parameters
fn connectionString(host: string, port: int, database: string, user: string, password: string) string {
    return "host=" + host + " port=" + port + " dbname=" + database + " user=" + user + " password=" + password;
}
