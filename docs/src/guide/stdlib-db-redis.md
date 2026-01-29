# Redis

Stratos provides native Redis support through the `redis` module for high-performance key-value storage, caching, and pub/sub messaging.

## Getting Started

```stratos
package main;

use db.redis;

fn main() {
    // Connect to Redis
    val client = redis.connect("localhost", 6379);

    // Or connect to localhost with default port
    val client2 = redis.connectLocal();

    // Connect with authentication
    val client3 = redis.connectAuth("localhost", 6379, "mypassword");

    // Test connection
    if client.ping() {
        println("Connected to Redis!");
    }

    client.close();
}
```

## String Operations

The most common Redis operations work with string values:

```stratos
// Set and get values
client.set("name", "Alice");
val name = client.get("name");
println("Hello, " + name);

// Set with expiration (in seconds)
client.setEx("session:abc123", "user_data", 3600);  // Expires in 1 hour

// Check if key exists
if client.exists("name") {
    println("Key exists");
}

// Delete a key
client.del("name");

// Get TTL (time to live)
val ttl = client.ttl("session:abc123");  // Returns seconds remaining, -1 if no expiry, -2 if doesn't exist
```

### Atomic Counters

```stratos
// Increment/decrement numeric values
client.set("visitors", "0");
client.incr("visitors");  // Returns 1
client.incr("visitors");  // Returns 2
client.decr("visitors");  // Returns 1

// Perfect for rate limiting, counters, etc.
val requestCount = client.incr("rate:" + userId);
if requestCount > 100 {
    println("Rate limit exceeded");
}
```

### Key Expiration

```stratos
// Set expiration on existing key
client.set("temp_data", "value");
client.expire("temp_data", 60);  // Expires in 60 seconds

// Check remaining time
val remaining = client.ttl("temp_data");
println("Expires in " + remaining + " seconds");
```

### Finding Keys

```stratos
// Find keys matching a pattern
val userKeys = client.keys("user:*");
for key in userKeys {
    println(key);
}

// Common patterns:
// user:*     - All keys starting with "user:"
// *:session  - All keys ending with ":session"
// user:?     - Single character wildcard
```

::: warning
Avoid using `keys()` in production with large datasets. It can block the Redis server. Consider using SCAN instead (not yet implemented).
:::

## Hash Operations

Hashes are perfect for storing objects:

```stratos
// Store user data as a hash
client.hset("user:1", "name", "Alice");
client.hset("user:1", "email", "alice@example.com");
client.hset("user:1", "role", "admin");

// Get a single field
val name = client.hget("user:1", "name");
println("User: " + name);

// Check if field exists
if client.hexists("user:1", "email") {
    println("Has email");
}

// Get all fields
val keys = client.hkeys("user:1");
for key in keys {
    println(key + ": " + client.hget("user:1", key));
}

// Get all field-value pairs
val pairs = client.hgetall("user:1");
// pairs = ["name", "Alice", "email", "alice@example.com", "role", "admin"]
var i = 0;
while i < pairs.length() {
    println(pairs[i] + " = " + pairs[i + 1]);
    i = i + 2;
}

// Delete a field
client.hdel("user:1", "role");
```

## List Operations

Lists are great for queues, recent items, and timelines:

```stratos
// Add items to a list
client.rpush("tasks", "task1");  // Push to right (end)
client.rpush("tasks", "task2");
client.rpush("tasks", "task3");
client.lpush("tasks", "task0");  // Push to left (front)

// Get list length
val len = client.llen("tasks");
println("Tasks in queue: " + len);

// Get a range of items (0 to -1 means all)
val allTasks = client.lrange("tasks", 0, -1);
for task in allTasks {
    println(task);
}

// Get first 2 items
val first2 = client.lrange("tasks", 0, 1);

// Pop items (FIFO queue)
val next = client.lpop("tasks");  // Get from front
println("Processing: " + next);

// Pop from back (LIFO stack)
val last = client.rpop("tasks");
```

### Queue Pattern

```stratos
// Producer adds tasks
client.rpush("job_queue", "process_image:123");
client.rpush("job_queue", "send_email:456");

// Worker processes tasks
while true {
    val job = client.lpop("job_queue");
    if job == "" {
        break;  // Queue empty
    }
    println("Processing: " + job);
}
```

## Set Operations

Sets store unique values with fast membership testing:

```stratos
// Add members to a set
client.sadd("tags:post:1", "programming");
client.sadd("tags:post:1", "rust");
client.sadd("tags:post:1", "tutorial");
client.sadd("tags:post:1", "programming");  // Duplicate, ignored

// Check membership
if client.sismember("tags:post:1", "rust") {
    println("Post is tagged with rust");
}

// Get all members
val tags = client.smembers("tags:post:1");
for tag in tags {
    println("Tag: " + tag);
}

// Remove a member
client.srem("tags:post:1", "tutorial");
```

### Online Users Example

```stratos
// Track online users
fn userOnline(userId: string) {
    client.sadd("online_users", userId);
    client.expire("online_users", 300);  // Refresh every 5 min
}

fn isUserOnline(userId: string) bool {
    return client.sismember("online_users", userId);
}

fn getOnlineUsers() array<string> {
    return client.smembers("online_users");
}
```

## Pub/Sub Messaging

Redis can broadcast messages to subscribers:

```stratos
// Publisher
val subscribers = client.publish("news", "Breaking: New Stratos release!");
println("Message sent to " + subscribers + " subscribers");

// For subscribing, you would typically use a separate connection
// and a blocking subscribe call (async pattern recommended)
```

## Database Selection

Redis supports multiple databases (0-15 by default):

```stratos
// Select database 1
client.select(1);

// Operations now affect database 1
client.set("key", "value");

// Switch back to database 0
client.select(0);

// Flush current database (careful!)
client.flushDb();
```

## Complete Example: Session Store

```stratos
package main;

use db.redis;
use encoding.json;
use crypto;

class Session {
    var userId: int;
    var username: string;
    var createdAt: string;
}

class SessionStore {
    var client: redis.Client;
    var ttl: int;

    constructor(redisClient: redis.Client, sessionTtl: int) {
        this.client = redisClient;
        this.ttl = sessionTtl;
    }

    pub fn create(userId: int, username: string) string {
        // Generate session ID
        val sessionId = crypto.randomHex(32);

        // Store session data
        val key = "session:" + sessionId;
        this.client.hset(key, "userId", userId.toString());
        this.client.hset(key, "username", username);
        this.client.hset(key, "createdAt", time.now().toString());
        this.client.expire(key, this.ttl);

        return sessionId;
    }

    pub fn get(sessionId: string) Session? {
        val key = "session:" + sessionId;

        if !this.client.exists(key) {
            return null;
        }

        val session = Session();
        session.userId = this.client.hget(key, "userId").toInt();
        session.username = this.client.hget(key, "username");
        session.createdAt = this.client.hget(key, "createdAt");

        // Refresh TTL on access
        this.client.expire(key, this.ttl);

        return session;
    }

    pub fn destroy(sessionId: string) {
        this.client.del("session:" + sessionId);
    }
}

fn main() {
    val client = redis.connectLocal();
    val sessions = SessionStore(client, 3600);  // 1 hour TTL

    // Create a session
    val sessionId = sessions.create(123, "alice");
    println("Session created: " + sessionId);

    // Retrieve session
    val session = sessions.get(sessionId);
    if session != null {
        println("User: " + session.username);
    }

    // Destroy session
    sessions.destroy(sessionId);

    client.close();
}
```

## Complete Example: Rate Limiter

```stratos
package main;

use db.redis;

class RateLimiter {
    var client: redis.Client;
    var maxRequests: int;
    var windowSeconds: int;

    constructor(redisClient: redis.Client, max: int, window: int) {
        this.client = redisClient;
        this.maxRequests = max;
        this.windowSeconds = window;
    }

    pub fn isAllowed(identifier: string) bool {
        val key = "ratelimit:" + identifier;
        val count = this.client.incr(key);

        // Set expiry on first request
        if count == 1 {
            this.client.expire(key, this.windowSeconds);
        }

        return count <= this.maxRequests;
    }

    pub fn remaining(identifier: string) int {
        val key = "ratelimit:" + identifier;
        val current = this.client.get(key);
        if current == "" {
            return this.maxRequests;
        }
        return this.maxRequests - current.toInt();
    }
}

fn main() {
    val client = redis.connectLocal();
    val limiter = RateLimiter(client, 100, 60);  // 100 requests per minute

    val userId = "user:123";

    if limiter.isAllowed(userId) {
        println("Request allowed. Remaining: " + limiter.remaining(userId));
    } else {
        println("Rate limit exceeded!");
    }

    client.close();
}
```

## Best Practices

1. **Use meaningful key names** - Use colons as separators: `user:123:profile`
2. **Set expiration on temporary data** - Prevent memory leaks
3. **Use hashes for objects** - More memory efficient than separate keys
4. **Use sets for unique collections** - Fast membership testing
5. **Use lists for queues** - `lpush`/`rpop` for FIFO
6. **Close connections** - Use `client.close()` when done
7. **Handle missing keys** - `get()` returns empty string for missing keys
