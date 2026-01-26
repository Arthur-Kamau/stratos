// Test: Method calls within the same module (similar to http module structure)

package main;

use log;

// Define a class with method declarations (no bodies - native style)
class Router {
    pub var routes: int;

    pub fn get(path: string) void;
    pub fn post(path: string) void;
}

// Factory function
fn newRouter() Router;

// Function that uses the router - similar to http serve()
fn serve(port: int) {
    val router = newRouter();
    router.get("*");  // This should work
    log.info("Server started on port " + port);
}

fn main() {
    log.info("=== Method Test 2 ===");
    serve(8080);
    log.info("=== Test Complete ===");
}
