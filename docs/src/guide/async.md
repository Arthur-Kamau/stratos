# Asynchronous Programming

Stratos has first-class support for asynchronous programming with async/await syntax, making it easy to write concurrent code that's readable and maintainable.

## Understanding Async Functions

When you declare a function as `async`, it automatically returns a `Future` that wraps the return type. The `await` keyword pauses execution until a Future resolves:

```stratos
// This function signature means it returns Future<string>
async fn fetchData(url: string) <string> {
    val response = await http.get(url);
    return await response.text();
}

// Calling this function gives you a Future<string>
val dataFuture = fetchData("https://example.com");
val data = await dataFuture;  // Unwrap the Future
```

::: tip Future Wrapping
Async functions automatically wrap their return value in a `Future`. You don't write `Future<string>` as the return type—just write `string` and the compiler handles the Future wrapping.
:::

## Await Keyword

The `await` keyword pauses execution until a Future resolves:

```stratos
package main;

use net;
use log;
use encoding;

class User {
    var name: string;
    var id: int;
}

async fn getUserData(userId: int) User {
    log.info("Fetching user " + userId);
    val response = await net.get("https://api.example.com/users/" + userId);
    return encoding.json.decode(response.text());
}

async fn main() {
    val user = await getUserData(123);
    log.info("Got user: " + user.name);
}
```

## Async Main Function

The main function can be async to allow top-level await:

```stratos
package main;

use async;
use log;

async fn main() {
    log.info("Starting async program");
    
    await async.sleep(1000);  // Sleep for 1000 milliseconds (1 second)
    
    log.info("Done!");
}
```

## Creating Futures

You can create custom Futures for asynchronous operations:

```stratos
package main;

use async;

fn createFuture() Future<int> {
    return async.Future<int>(fn(resolve, reject) {
        // Simulate async work
        async.sleep(100).then(fn() {
            resolve(42);
        });
    });
}

async fn main() {
    val result = await createFuture();
    print("Result: " + result);
}
```

The `async.Future<T>` constructor takes a callback function with two parameters:
- `resolve`: Call this with a value when the operation succeeds
- `reject`: Call this with an error when the operation fails

## Future Chaining

Chain asynchronous operations using `.then()`:

```stratos
package main;

use async;
use net;

async fn main() {
    val result = await net.get("https://api.example.com/data")
        .then(fn(response) {
            return response.json();
        })
        .then(fn(data) {
            return data.value * 2;
        });
    
    print("Result: " + result);
}
```

## Concurrent Operations

Run multiple async operations concurrently to improve performance:

```stratos
package main;

use async;
use log;

async fn task1() int {
    await async.sleep(100);
    return 1;
}

async fn task2() int {
    await async.sleep(200);
    return 2;
}

async fn task3() int {
    await async.sleep(150);
    return 3;
}

async fn main() {
    // Run all tasks concurrently - they execute in parallel
    val results = await async.all([
        task1(),
        task2(),
        task3()
    ]);
    
    log.info("All tasks completed");
    for (result in results) {
        log.info("Result: " + result);
    }
}
```

::: tip Sequential vs Concurrent
```stratos
// Sequential (slow) - total time: 450ms
val r1 = await task1();  // 100ms
val r2 = await task2();  // 200ms
val r3 = await task3();  // 150ms

// Concurrent (fast) - total time: 200ms (longest task)
val results = await async.all([task1(), task2(), task3()]);
```
:::

## async.all()

Wait for multiple Futures to complete. All Futures run concurrently:

```stratos
package main;

use async;
use net;

async fn main() {
    val futures = [
        net.get("https://api.example.com/users/1"),
        net.get("https://api.example.com/users/2"),
        net.get("https://api.example.com/users/3")
    ];
    
    val responses = await async.all(futures);
    
    for (response in responses) {
        print(response.text());
    }
}
```

## async.race()

Return the result of the first Future that resolves:

```stratos
package main;

use async;

async fn slowTask() string {
    await async.sleep(2000);
    return "Slow";
}

async fn fastTask() string {
    await async.sleep(500);
    return "Fast";
}

async fn main() {
    val result = await async.race([
        slowTask(),
        fastTask()
    ]);
    
    print("Winner: " + result);  // "Fast"
}
```

## async.timeout()

Set a timeout for async operations to prevent them from hanging indefinitely:

```stratos
package main;

use async;
use net;
use log;

async fn fetchWithTimeout(url: string, timeoutMs: int) Result<string, Error> {
    try {
        val result = await async.timeout(
            net.get(url).then(fn(r) { return r.text(); }),
            timeoutMs
        );
        return Ok(result);
    } catch (error) {
        log.error("Request timed out after " + timeoutMs + "ms");
        return Err(error);
    }
}

async fn main() {
    val result = await fetchWithTimeout("https://api.example.com/data", 5000);
    
    match (result) {
        Ok(data) -> log.info("Got data: " + data)
        Err(e) -> log.error("Failed: " + e.message)
    }
}
```

## Error Handling with Result

Stratos uses the `Result<T, E>` type for error handling, similar to Rust. A Result is either `Ok(value)` or `Err(error)`:

```stratos
package main;

use net;
use log;

class User {
    var name: string;
    var id: int;
}

async fn fetchUser(userId: int) Result<User, Error> {
    try {
        val response = await net.get("https://api.example.com/users/" + userId);
        
        if (response.status() == 200) {
            val user = encoding.json.decode(response.text());
            return Ok(user);
        } else {
            return Err(Error("User not found: HTTP " + response.status()));
        }
    } catch (error) {
        log.error("Failed to fetch user: " + error.message);
        return Err(error);
    }
}

async fn main() {
    val result = await fetchUser(123);
    
    match (result) {
        Ok(user) -> log.info("User: " + user.name)
        Err(e) -> log.error("Error: " + e.message)
    }
}
```

::: tip Result vs Exceptions
Use `Result<T, E>` for expected errors (like network failures or invalid input) and exceptions for unexpected errors (like programming bugs). The `try/catch` syntax can catch exceptions, while `match` handles Result values.
:::

## Working with Options

Use `Option<T>` for values that might be absent. An Option is either `Some(value)` or `None`:

```stratos
package main;

use net;
use log;
use encoding;

class User {
    var name: string;
    var id: int;
}

async fn fetchUserSafely(userId: int) Option<User> {
    try {
        val response = await net.get("https://api.example.com/users/" + userId);
        if (response.status() == 200) {
            return Some(encoding.json.decode(response.text()));
        }
        return None;
    } catch (error) {
        log.error("Failed to fetch user: " + error.message);
        return None;
    }
}

async fn main() {
    val user = await fetchUserSafely(123);
    
    match (user) {
        Some(u) -> log.info("User: " + u.name)
        None -> log.warn("User not found")
    }
}
```

## Complete Example: HTTP API Client

```stratos
package main;

use net;
use async;
use log;
use encoding;

class ApiClient {
    var baseUrl: string;
    
    constructor(baseUrl: string) {
        this.baseUrl = baseUrl;
    }
    
    async fn get(endpoint: string) Result<any, Error> {
        val url = this.baseUrl + endpoint;
        log.info("GET " + url);
        
        try {
            val response = await net.get(url);
            
            if (response.status() >= 200 && response.status() < 300) {
                return Ok(encoding.json.decode(response.text()));
            } else {
                return Err(Error("HTTP " + response.status()));
            }
        } catch (error) {
            return Err(error);
        }
    }
    
    async fn post(endpoint: string, data: any) Result<any, Error> {
        val url = this.baseUrl + endpoint;
        val body = encoding.json.encode(data);
        
        log.info("POST " + url);
        
        try {
            val response = await net.post(url, body);
            
            if (response.status() >= 200 && response.status() < 300) {
                return Ok(encoding.json.decode(response.text()));
            } else {
                return Err(Error("HTTP " + response.status()));
            }
        } catch (error) {
            return Err(error);
        }
    }
}

class User {
    var id: int;
    var name: string;
    var email: string;
}

async fn fetchUsers(client: ApiClient) Array<User> {
    val result = await client.get("/users");
    
    match (result) {
        Ok(data) -> return data
        Err(e) -> {
            log.error("Failed to fetch users: " + e.message);
            return [];
        }
    }
}

async fn createUser(client: ApiClient, name: string, email: string) bool {
    val userData = {
        "name": name,
        "email": email
    };
    
    val result = await client.post("/users", userData);
    
    match (result) {
        Ok(data) -> {
            log.info("Created user: " + data.id);
            return true;
        }
        Err(e) -> {
            log.error("Failed to create user: " + e.message);
            return false;
        }
    }
}

async fn main() {
    val client = ApiClient("https://api.example.com");
    
    // Fetch users and create new users concurrently
    log.info("Starting operations...");
    
    val operations = [
        fetchUsers(client),
        createUser(client, "Alice", "alice@example.com"),
        createUser(client, "Bob", "bob@example.com")
    ];
    
    val results = await async.all(operations);
    
    log.info("All operations completed");
    log.info("Fetched " + results[0].length + " users");
}
```

## Best Practices

::: tip Use async/await
Prefer async/await over raw Future handling. It makes code more readable and easier to debug.
:::

::: tip Error handling
Always handle errors explicitly using `Result<T, E>` for expected errors and `try/catch` for unexpected exceptions.
:::

::: tip Concurrent execution
Use `async.all()` to run independent async operations concurrently rather than sequentially with multiple awaits. This can dramatically improve performance.
:::

::: tip Timeouts
Always set timeouts for network operations using `async.timeout()` to prevent your program from hanging indefinitely.
:::

::: warning Blocking operations
Don't use blocking operations (like synchronous I/O) in async functions. They defeat the purpose of asynchronous code and will block the entire async runtime.
:::

::: warning Error propagation
When using `Result<T, E>`, always handle both `Ok` and `Err` cases with `match`. Ignoring errors can lead to silent failures.
:::

## Async Quick Reference

| Feature | Syntax | Purpose |
|---------|--------|---------|
| Async function | `async fn name() T` | Returns `Future<T>` |
| Await | `await future` | Wait for Future to resolve |
| Async main | `async fn main()` | Async entry point |
| Future.all | `await async.all([...])` | Wait for all Futures concurrently |
| Future.race | `await async.race([...])` | First Future wins |
| Timeout | `await async.timeout(future, ms)` | Set operation timeout |
| Sleep | `await async.sleep(ms)` | Async delay (milliseconds) |
| Result | `Result<T, Error>` | Success or error |
| Option | `Option<T>` | Value or None |

## Common Patterns

### Retry Logic

```stratos
async fn fetchWithRetry(url: string, maxRetries: int) Result<string, Error> {
    var attempts = 0;
    
    while (attempts < maxRetries) {
        val result = await net.get(url);
        
        if (result.status() == 200) {
            return Ok(result.text());
        }
        
        attempts += 1;
        await async.sleep(1000 * attempts);  // Exponential backoff
    }
    
    return Err(Error("Max retries exceeded"));
}
```

### Parallel Data Processing

```stratos
async fn processItems(items: Array<string>) Array<Result<any, Error>> {
    val futures = items.map(fn(item) {
        return processItem(item);
    });
    
    return await async.all(futures);
}
```

### Graceful Degradation

```stratos
async fn fetchWithFallback(primaryUrl: string, fallbackUrl: string) string {
    val result = await net.get(primaryUrl);
    
    if (result.status() == 200) {
        return result.text();
    }
    
    log.warn("Primary failed, trying fallback");
    val fallback = await net.get(fallbackUrl);
    return fallback.text();
}
```

## Next Steps

- [Error Handling](/guide/error-handling) - Deep dive into Result and error handling
- [Standard Library - async](/reference/stdlib#async) - Full async module API reference
- [Standard Library - net](/reference/stdlib#net) - Network operations
- [Pattern Matching](/guide/pattern-matching) - Learn more about match expressions