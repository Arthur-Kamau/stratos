// HTTP-like module to test issue

package myhttp;

use log;

// Class with method declarations
class Router {
    pub var routes: int;

    pub fn get(path: string, handler: string) void;
    pub fn post(path: string, handler: string) void;
}

// Factory function
pub fn newRouter() Router;

// Function that uses the router (like http.serve)
pub fn serve(port: int, handler: string) void {
    val router = newRouter();
    router.get("*", handler);
    log.info("Serving on port " + port);
}

// Quick function (like http.quickGet)
pub fn quickGet(urlStr: string) string {
    val client = newRouter();
    return client.get(urlStr, "default");
}
