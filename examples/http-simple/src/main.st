// Example: Simple HTTP Server
// Basic demonstration of the http module

package main;

use http;

fn main() {
    println("=== Simple HTTP Server ===");

    // Create router
    val router = http.newRouter();

    // Simple GET endpoint at root
    router.get("/", (req, res) => {
        res.text("Hello from Stratos HTTP Server!");
    });

    // JSON endpoint
    router.get("/api/hello", (req, res) => {
        res.json("{\"message\": \"Hello, World!\", \"status\": \"ok\"}");
    });

    // Echo endpoint - returns the request path
    router.get("/echo/:message", (req, res) => {
        val message = req.getParam("message");
        res.text("You said: " + message);
    });

    // POST endpoint
    router.post("/api/data", (req, res) => {
        val body = req.body;
        res.status(201);
        res.text("Received: " + body);
    });

    // Create and start server
    val server = http.newServer(router);

    println("Starting server on http://localhost:8080");
    println("Try these endpoints:");
    println("  curl http://localhost:8080/");
    println("  curl http://localhost:8080/api/hello");
    println("  curl http://localhost:8080/echo/test");
    println("  curl -X POST http://localhost:8080/api/data -d 'Hello'");

    server.listen(8080);
}
