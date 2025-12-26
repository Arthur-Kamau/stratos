// Example: HTTP Server
// Demonstrates the net/http module

package main;

use http;
use json;
use log;
use collections;

fn main() {
    log.info("=== HTTP Server Example ===");

    // Create router
    val router = http.newRouter();

    // Add middleware
    router.use(http.loggerMiddleware());
    router.use(http.corsMiddleware("*"));

    // Simple GET endpoint
    router.get("/", (req, res) => {
        res.json(json.jsonObject({
            message: json.jsonString("Welcome to Stratos HTTP Server!"),
            version: json.jsonString("1.0.0"),
            endpoints: json.jsonArray([
                json.jsonString("GET /"),
                json.jsonString("GET /api/users"),
                json.jsonString("POST /api/users"),
                json.jsonString("GET /api/users/:id"),
                json.jsonString("GET /health")
            ])
        }));
    });

    // Health check endpoint
    router.get("/health", (req, res) => {
        res.json(json.jsonObject({
            status: json.jsonString("healthy"),
            timestamp: json.jsonNumber(time.now().unix())
        }));
    });

    // In-memory user store
    var users = collections.newMap<string, json.JsonValue>();
    var nextId = 1;

    // REST API endpoints
    val api = router.group("/api");

    // GET /api/users - List all users
    api.get("/users", (req, res) => {
        val userList = collections.newList<json.JsonValue>();

        users.values().forEach((user) => {
            userList.add(user);
        });

        res.json(json.jsonObject({
            users: json.jsonArray(userList.items),
            count: json.jsonNumber(users.length())
        }));
    });

    // POST /api/users - Create user
    api.post("/users", (req, res) => {
        val bodyResult = req.json();

        if !bodyResult.ok() {
            res.setStatus(400);
            res.json(json.jsonObject({
                error: json.jsonString("Invalid JSON body")
            }));
            return;
        }

        val body = bodyResult.unwrap();

        // Validate required fields
        val name = json.getStringPath(body, "name");
        val email = json.getStringPath(body, "email");

        if name == "" || email == "" {
            res.setStatus(400);
            res.json(json.jsonObject({
                error: json.jsonString("Name and email are required")
            }));
            return;
        }

        // Create user
        val id = nextId;
        nextId = nextId + 1;

        val user = json.jsonObject({
            id: json.jsonNumber(id),
            name: json.jsonString(name),
            email: json.jsonString(email),
            createdAt: json.jsonNumber(time.now().unix())
        });

        users.put(id, user);

        res.setStatus(201);
        res.json(user);

        log.infoWith("User created", {
            id: id,
            name: name,
            email: email
        });
    });

    // GET /api/users/:id - Get user by ID
    api.get("/users/:id", (req, res) => {
        val id = req.param("id");

        if users.containsKey(id) {
            res.json(users.get(id));
        } else {
            res.setStatus(404);
            res.json(json.jsonObject({
                error: json.jsonString("User not found")
            }));
        }
    });

    // DELETE /api/users/:id - Delete user
    api.delete("/users/:id", (req, res) => {
        val id = req.param("id");

        if users.containsKey(id) {
            users.remove(id);
            res.setStatus(204);
            res.send("");
        } else {
            res.setStatus(404);
            res.json(json.jsonObject({
                error: json.jsonString("User not found")
            }));
        }
    });

    // Rate-limited endpoint
    router.use(http.rateLimitMiddleware(60)); // 60 requests per minute
    router.get("/api/limited", (req, res) => {
        res.json(json.jsonObject({
            message: json.jsonString("This endpoint is rate-limited")
        }));
    });

    // Create and start server
    val server = http.newServer(router);

    log.info("Starting server on http://localhost:8080");
    log.info("Try these endpoints:");
    log.info("  curl http://localhost:8080/");
    log.info("  curl http://localhost:8080/health");
    log.info("  curl -X POST http://localhost:8080/api/users -H 'Content-Type: application/json' -d '{\"name\":\"Alice\",\"email\":\"alice@example.com\"}'");
    log.info("  curl http://localhost:8080/api/users");
    log.info("  curl http://localhost:8080/api/users/1");

    server.listen(8080);
}
