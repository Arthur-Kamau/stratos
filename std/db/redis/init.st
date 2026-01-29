// Redis Support for Stratos
// Inspired by Go's go-redis and Node.js ioredis
package redis;

// ============================================================================
// Native function declarations (implemented in C++)
// ============================================================================

// Connection
fn __redis_connect(host: string, port: int) any;
fn __redis_connect_auth(host: string, port: int, password: string) any;
fn __redis_close(handle: any) void;
fn __redis_ping(handle: any) bool;
fn __redis_select(handle: any, db: int) bool;
fn __redis_flushdb(handle: any) bool;

// String operations
fn __redis_get(handle: any, key: string) string;
fn __redis_set(handle: any, key: string, value: string) bool;
fn __redis_setex(handle: any, key: string, value: string, seconds: int) bool;
fn __redis_del(handle: any, key: string) int;
fn __redis_exists(handle: any, key: string) bool;
fn __redis_expire(handle: any, key: string, seconds: int) bool;
fn __redis_ttl(handle: any, key: string) int;
fn __redis_incr(handle: any, key: string) int;
fn __redis_decr(handle: any, key: string) int;
fn __redis_keys(handle: any, pattern: string) array<string>;

// Hash operations
fn __redis_hget(handle: any, key: string, field: string) string;
fn __redis_hset(handle: any, key: string, field: string, value: string) bool;
fn __redis_hdel(handle: any, key: string, field: string) int;
fn __redis_hexists(handle: any, key: string, field: string) bool;
fn __redis_hgetall(handle: any, key: string) array<string>;
fn __redis_hkeys(handle: any, key: string) array<string>;

// List operations
fn __redis_lpush(handle: any, key: string, value: string) int;
fn __redis_rpush(handle: any, key: string, value: string) int;
fn __redis_lpop(handle: any, key: string) string;
fn __redis_rpop(handle: any, key: string) string;
fn __redis_lrange(handle: any, key: string, start: int, stop: int) array<string>;
fn __redis_llen(handle: any, key: string) int;

// Set operations
fn __redis_sadd(handle: any, key: string, member: string) int;
fn __redis_srem(handle: any, key: string, member: string) int;
fn __redis_sismember(handle: any, key: string, member: string) bool;
fn __redis_smembers(handle: any, key: string) array<string>;

// Pub/Sub
fn __redis_publish(handle: any, channel: string, message: string) int;

// ============================================================================
// Class Definitions
// ============================================================================

// Client represents a Redis connection
class Client {
    var handle: any;

    constructor(connHandle: any) {
        this.handle = connHandle;
    }

    // ========================================================================
    // Connection Methods
    // ========================================================================

    // Close the Redis connection
    pub fn close() {
        __redis_close(this.handle);
    }

    // Ping the Redis server
    pub fn ping() bool {
        return __redis_ping(this.handle);
    }

    // Select a database by index
    pub fn select(db: int) bool {
        return __redis_select(this.handle, db);
    }

    // Flush the current database
    pub fn flushDb() bool {
        return __redis_flushdb(this.handle);
    }

    // ========================================================================
    // String Operations
    // ========================================================================

    // Get a string value
    //
    // Example:
    //   val value = client.get("mykey");
    pub fn get(key: string) string {
        return __redis_get(this.handle, key);
    }

    // Set a string value
    //
    // Example:
    //   client.set("mykey", "myvalue");
    pub fn set(key: string, value: string) bool {
        return __redis_set(this.handle, key, value);
    }

    // Set a string value with expiration in seconds
    //
    // Example:
    //   client.setEx("session:123", "data", 3600); // Expires in 1 hour
    pub fn setEx(key: string, value: string, seconds: int) bool {
        return __redis_setex(this.handle, key, value, seconds);
    }

    // Delete a key
    // Returns the number of keys deleted
    pub fn del(key: string) int {
        return __redis_del(this.handle, key);
    }

    // Check if a key exists
    pub fn exists(key: string) bool {
        return __redis_exists(this.handle, key);
    }

    // Set expiration on a key
    pub fn expire(key: string, seconds: int) bool {
        return __redis_expire(this.handle, key, seconds);
    }

    // Get time to live for a key
    // Returns -2 if key doesn't exist, -1 if no expiration
    pub fn ttl(key: string) int {
        return __redis_ttl(this.handle, key);
    }

    // Increment a numeric value
    pub fn incr(key: string) int {
        return __redis_incr(this.handle, key);
    }

    // Decrement a numeric value
    pub fn decr(key: string) int {
        return __redis_decr(this.handle, key);
    }

    // Find keys matching a pattern
    //
    // Example:
    //   val keys = client.keys("user:*");
    pub fn keys(pattern: string) array<string> {
        return __redis_keys(this.handle, pattern);
    }

    // ========================================================================
    // Hash Operations
    // ========================================================================

    // Get a hash field value
    //
    // Example:
    //   val name = client.hget("user:1", "name");
    pub fn hget(key: string, field: string) string {
        return __redis_hget(this.handle, key, field);
    }

    // Set a hash field value
    //
    // Example:
    //   client.hset("user:1", "name", "Alice");
    pub fn hset(key: string, field: string, value: string) bool {
        return __redis_hset(this.handle, key, field, value);
    }

    // Delete a hash field
    pub fn hdel(key: string, field: string) int {
        return __redis_hdel(this.handle, key, field);
    }

    // Check if a hash field exists
    pub fn hexists(key: string, field: string) bool {
        return __redis_hexists(this.handle, key, field);
    }

    // Get all fields and values in a hash
    // Returns alternating field/value pairs: [field1, value1, field2, value2, ...]
    //
    // Example:
    //   val pairs = client.hgetall("user:1");
    //   // pairs = ["name", "Alice", "email", "alice@example.com"]
    pub fn hgetall(key: string) array<string> {
        return __redis_hgetall(this.handle, key);
    }

    // Get all field names in a hash
    pub fn hkeys(key: string) array<string> {
        return __redis_hkeys(this.handle, key);
    }

    // ========================================================================
    // List Operations
    // ========================================================================

    // Push a value to the front of a list
    // Returns the length of the list after push
    pub fn lpush(key: string, value: string) int {
        return __redis_lpush(this.handle, key, value);
    }

    // Push a value to the back of a list
    // Returns the length of the list after push
    pub fn rpush(key: string, value: string) int {
        return __redis_rpush(this.handle, key, value);
    }

    // Pop a value from the front of a list
    pub fn lpop(key: string) string {
        return __redis_lpop(this.handle, key);
    }

    // Pop a value from the back of a list
    pub fn rpop(key: string) string {
        return __redis_rpop(this.handle, key);
    }

    // Get a range of elements from a list
    // Use -1 for the last element, -2 for second to last, etc.
    //
    // Example:
    //   val items = client.lrange("mylist", 0, -1); // Get all
    //   val first3 = client.lrange("mylist", 0, 2); // Get first 3
    pub fn lrange(key: string, start: int, stop: int) array<string> {
        return __redis_lrange(this.handle, key, start, stop);
    }

    // Get the length of a list
    pub fn llen(key: string) int {
        return __redis_llen(this.handle, key);
    }

    // ========================================================================
    // Set Operations
    // ========================================================================

    // Add a member to a set
    // Returns 1 if added, 0 if already exists
    pub fn sadd(key: string, member: string) int {
        return __redis_sadd(this.handle, key, member);
    }

    // Remove a member from a set
    // Returns 1 if removed, 0 if didn't exist
    pub fn srem(key: string, member: string) int {
        return __redis_srem(this.handle, key, member);
    }

    // Check if a value is a member of a set
    pub fn sismember(key: string, member: string) bool {
        return __redis_sismember(this.handle, key, member);
    }

    // Get all members of a set
    pub fn smembers(key: string) array<string> {
        return __redis_smembers(this.handle, key);
    }

    // ========================================================================
    // Pub/Sub Operations
    // ========================================================================

    // Publish a message to a channel
    // Returns the number of subscribers that received the message
    pub fn publish(channel: string, message: string) int {
        return __redis_publish(this.handle, channel, message);
    }
}

// ============================================================================
// Public API Functions
// ============================================================================

// Connect to a Redis server
//
// Example:
//   val client = redis.connect("localhost", 6379);
fn connect(host: string, port: int) Client {
    val handle = __redis_connect(host, port);
    return Client(handle);
}

// Connect to a Redis server with authentication
//
// Example:
//   val client = redis.connectAuth("localhost", 6379, "mypassword");
fn connectAuth(host: string, port: int, password: string) Client {
    val handle = __redis_connect_auth(host, port, password);
    return Client(handle);
}

// Connect to localhost on default port (6379)
fn connectLocal() Client {
    return connect("localhost", 6379);
}

// ============================================================================
// Helper Functions
// ============================================================================

// Parse HGETALL result into key-value pairs
// Returns array where even indices are keys and odd indices are values
fn parseHashPairs(pairs: array<string>) array<array<string>> {
    var result: array<array<string>> = [];
    var i = 0;
    while i < pairs.length() - 1 {
        result.push([pairs[i], pairs[i + 1]]);
        i = i + 2;
    }
    return result;
}
