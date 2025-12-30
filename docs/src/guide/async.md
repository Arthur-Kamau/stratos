# Asynchronous Programming

Stratos has first-class support for asynchronous programming with async/await syntax, making it easy to write concurrent code that's readable and maintainable.

## Async Functions

Functions can be declared as `async`, which means they return a Promise and can use the `await` keyword:

```stratos
async fn fetchData(url: string) string {
    val response = await http.get(url);
    return await response.text();
}
```

## Await Keyword

The `await` keyword pauses execution until a Promise resolves:

```stratos
use net;
use log;

async fn getUserData(userId: int) User {
    log.info("Fetching user " + userId);
    val response = await net.get("https://api.example.com/users/" + userId);
    return json.decode(response.text());
}

fn main() async {
    val user = await getUserData(123);
    log.info("Got user: " + user.name);
}
```

## Async Main Function

The main function can be async:

```stratos
use async;
use log;

fn main() async {
    log.info("Starting async program");

    await async.sleep(1000);  // Sleep for 1 second

    log.info("Done!");
}
```

## Promises

Stratos uses Promises for asynchronous operations:

```stratos
use async;

fn createPromise() Promise<int> {
    return async.Promise<int>(fn(resolve, reject) {
        // Simulate async work
        async.sleep(100).then(fn() {
            resolve(42);
        });
    });
}

fn main() async {
    val result = await createPromise();
    print("Result: " + result);
}
```

## Promise Chaining

Chain asynchronous operations:

```stratos
use async;
use net;

fn main() async {
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

Run multiple async operations concurrently:

```stratos
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

fn main() async {
    // Run all tasks concurrently
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

## Async.all()

Wait for multiple Promises to complete:

```stratos
use async;
use net;

fn main() async {
    val promises = [
        net.get("https://api.example.com/users/1"),
        net.get("https://api.example.com/users/2"),
        net.get("https://api.example.com/users/3")
    ];

    val responses = await async.all(promises);

    for (response in responses) {
        print(response.text());
    }
}
```

## Async.race()

Return the first Promise that resolves:

```stratos
use async;

async fn slowTask() string {
    await async.sleep(2000);
    return "Slow";
}

async fn fastTask() string {
    await async.sleep(500);
    return "Fast";
}

fn main() async {
    val result = await async.race([
        slowTask(),
        fastTask()
    ]);

    print("Winner: " + result);  // "Fast"
}
```

## Error Handling

Handle errors in async code with try/catch:

```stratos
use net;
use log;

async fn fetchUserSafely(userId: int) User? {
    try {
        val response = await net.get("https://api.example.com/users/" + userId);
        if (response.status() == 200) {
            return Some(json.decode(response.text()));
        }
        return None;
    } catch (error) {
        log.error("Failed to fetch user: " + error.message);
        return None;
    }
}

fn main() async {
    val user = await fetchUserSafely(123);

    when (user) {
        Some(u) -> log.info("User: " + u.name)
        None -> log.warn("User not found")
    }
}
```

## Complete Example: HTTP Client

```stratos
package main;

use net;
use async;
use log;
use encoding/json;

class ApiClient {
    var baseUrl: string;

    constructor(baseUrl: string) {
        this.baseUrl = baseUrl;
    }

    async fn get(endpoint: string) any {
        val url = this.baseUrl + endpoint;
        log.info("GET " + url);

        val response = await net.get(url);

        if (response.status() >= 200 && response.status() < 300) {
            return json.decode(response.text());
        } else {
            throw Error("HTTP " + response.status());
        }
    }

    async fn post(endpoint: string, data: any) any {
        val url = this.baseUrl + endpoint;
        val body = json.encode(data);

        log.info("POST " + url);

        val response = await net.post(url, body);

        if (response.status() >= 200 && response.status() < 300) {
            return json.decode(response.text());
        } else {
            throw Error("HTTP " + response.status());
        }
    }
}

async fn fetchUsers(client: ApiClient) Array<User> {
    try {
        val data = await client.get("/users");
        return data;
    } catch (error) {
        log.error("Failed to fetch users: " + error.message);
        return [];
    }
}

async fn createUser(client: ApiClient, name: string, email: string) bool {
    try {
        val userData = {
            "name": name,
            "email": email
        };

        val result = await client.post("/users", userData);
        log.info("Created user: " + result.id);
        return true;
    } catch (error) {
        log.error("Failed to create user: " + error.message);
        return false;
    }
}

fn main() async {
    val client = ApiClient("https://api.example.com");

    // Fetch users concurrently
    log.info("Fetching data...");

    val promises = [
        fetchUsers(client),
        createUser(client, "Alice", "alice@example.com"),
        createUser(client, "Bob", "bob@example.com")
    ];

    val results = await async.all(promises);

    log.info("All operations completed");
}
```

## Best Practices

::: tip Use async/await
Prefer async/await over raw Promise handling. It makes code more readable and easier to debug.
:::

::: tip Error handling
Always use try/catch in async functions to handle potential errors gracefully.
:::

::: tip Concurrent execution
Use `async.all()` to run independent async operations concurrently rather than sequentially with multiple awaits.
:::

::: warning Blocking operations
Don't use blocking operations (like synchronous I/O) in async functions. They defeat the purpose of asynchronous code.
:::

## Async Quick Reference

| Feature | Syntax | Purpose |
|---------|--------|---------|
| Async function | `async fn name()` | Declare async function |
| Await | `await promise` | Wait for Promise |
| Async main | `fn main() async` | Async entry point |
| Promise.all | `await async.all([...])` | Wait for all Promises |
| Promise.race | `await async.race([...])` | First Promise wins |
| Sleep | `await async.sleep(ms)` | Async delay |

## Next Steps

- [Error Handling](/guide/error-handling) - Handle errors effectively
- [Standard Library - async](/reference/stdlib#async) - Async module API
- [Standard Library - net](/reference/stdlib#net) - Network operations
