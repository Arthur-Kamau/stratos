package main;

use log;
use http;

// Network monitoring demo: watch HTTP requests in the Network tab

fn main() {
    log.info("Network monitoring demo starting");
    log.info("Open DevTools Network tab to see requests");

    // Create a simple HTTP server
    val router = http.newRouter();

    router.get("/api/status", (req, res) => {
        log.debug("Status endpoint hit");
        res.json({"status": "ok", "uptime": 42});
    });

    router.get("/api/users", (req, res) => {
        log.debug("Users endpoint hit");
        res.json([
            {"name": "Alice", "role": "admin"},
            {"name": "Bob", "role": "user"}
        ]);
    });

    router.post("/api/echo", (req, res) => {
        log.debug("Echo endpoint hit");
        res.json({"echo": req.body});
    });

    log.info("Starting server on port 3000");
    log.info("Try: curl http://localhost:3000/api/status");
    val server = http.newServer(router);
    server.listen(3000);
}
