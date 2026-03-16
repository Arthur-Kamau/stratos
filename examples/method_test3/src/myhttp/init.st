// HTTP-like module to test method calls

package myhttp;

use log;

// Class with method implementations
class Router {
    pub var routes: int;

    constructor() {
        this.routes = 0;
    }

    pub fn get(path: string, handler: string) void {
        this.routes = this.routes + 1;
        log.info("Registered GET " + path + " -> " + handler);
    }

    pub fn post(path: string, handler: string) void {
        this.routes = this.routes + 1;
        log.info("Registered POST " + path + " -> " + handler);
    }
}

// Factory function
pub fn newRouter() Router {
    return Router();
}

// Function that uses the router (like http.serve)
pub fn serve(port: int, handler: string) void {
    val router = newRouter();
    router.get("*", handler);
    log.info("Serving on port " + port);
}

// Quick function (like http.quickGet)
pub fn quickGet(urlStr: string) string {
    val client = newRouter();
    client.get(urlStr, "default");
    return "response";
}
