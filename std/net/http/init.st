// HTTP module - HTTP server and client
// Inspired by Go's net/http and Node.js http modules

package http;

// ============================================================================
// Types
// ============================================================================

enum HttpMethod {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS
}

class Request {
    var method: HttpMethod;
    var url: string;
    var path: string;
    var query: Map<string, string>;
    var headers: Map<string, string>;
    var body: string;
    var params: Map<string, string>;
    var remoteAddr: string;

    fn header(name: string) string;
    fn queryParam(name: string) string;
    fn param(name: string) string;
    fn json() Result<json.JsonValue, Error>;
}

class Response {
    var statusCode: int;
    var headers: Map<string, string>;
    var body: string;
    var written: bool;

    fn setHeader(name: string, value: string) void;
    fn setStatus(code: int) void;
    fn write(data: string) void;
    fn json(value: json.JsonValue) void;
    fn html(content: string) void;
    fn send(content: string) void;
    fn sendStatus(code: int) void;
    fn redirect(url: string) void;
}

class Router {
    var routes: Array<Route>;
    var middleware: Array<Middleware>;

    fn get(path: string, handler: Handler) void;
    fn post(path: string, handler: Handler) void;
    fn put(path: string, handler: Handler) void;
    fn delete(path: string, handler: Handler) void;
    fn patch(path: string, handler: Handler) void;
    fn use(middleware: Middleware) void;
    fn group(prefix: string) Router;
}

class Route {
    var method: HttpMethod;
    var pattern: string;
    var handler: Handler;
    var middleware: Array<Middleware>;
}

class Handler = Function<Request, Response, void>;
class Middleware = Function<Request, Response, Function, void>;

class Server {
    var addr: string;
    var port: int;
    var router: Router;
    var running: bool;

    fn listen(port: int) void;
    fn listenAndServe(addr: string, port: int) void;
    fn close() void;
}

class Client {
    var timeout: int;
    var headers: Map<string, string>;

    fn get(url: string) async.Promise<Response>;
    fn post(url: string, body: string) async.Promise<Response>;
    fn put(url: string, body: string) async.Promise<Response>;
    fn delete(url: string) async.Promise<Response>;
    fn request(method: HttpMethod, url: string, body: string) async.Promise<Response>;
}

// ============================================================================
// Native function declarations
// ============================================================================

// Server creation
fn newServer(router: Router) Server;
fn newRouter() Router;

// Client creation
fn newClient(timeout: int) Client;

// Request parsing
fn parseRequest(raw: string) Result<Request, Error>;

// Response building
fn buildResponse(response: Response) string;

// URL parsing
fn parseURL(url: string) Result<URL, Error>;

class URL {
    var scheme: string;
    var host: string;
    var port: int;
    var path: string;
    var query: string;
    var fragment: string;
}

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Quick server setup
fn serve(port: int, handler: Handler) void {
    val router = newRouter();
    router.get("*", handler);

    val server = newServer(router);
    server.listen(port);
}

// Quick HTTP GET
fn get(url: string) async.Promise<Response> {
    val client = newClient(30000);
    return client.get(url);
}

// Quick HTTP POST
fn post(url: string, body: string) async.Promise<Response> {
    val client = newClient(30000);
    return client.post(url, body);
}

// Quick HTTP POST JSON
fn postJSON(url: string, data: json.JsonValue) async.Promise<Response> {
    val client = newClient(30000);
    client.headers.put("Content-Type", "application/json");
    return client.post(url, json.stringify(data));
}

// Middleware: Logger
fn loggerMiddleware() Middleware {
    return (req, res, next) => {
        val start = time.now();

        log.infoWith("HTTP Request", {
            method: req.method,
            path: req.path,
            remote: req.remoteAddr
        });

        next();

        val duration = time.now().sub(start);

        log.infoWith("HTTP Response", {
            method: req.method,
            path: req.path,
            status: res.statusCode,
            duration_ms: duration.milliseconds()
        });
    };
}

// Middleware: CORS
fn corsMiddleware(allowOrigin: string) Middleware {
    return (req, res, next) => {
        res.setHeader("Access-Control-Allow-Origin", allowOrigin);
        res.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

        if req.method == HttpMethod.OPTIONS {
            res.sendStatus(204);
        } else {
            next();
        }
    };
}

// Middleware: Authentication
fn authMiddleware(validateToken: Function<string, bool>) Middleware {
    return (req, res, next) => {
        val token = req.header("Authorization");

        if token == "" {
            res.setStatus(401);
            res.json(json.jsonObject({
                error: json.jsonString("Missing authorization token")
            }));
            return;
        }

        if !validateToken(token) {
            res.setStatus(403);
            res.json(json.jsonObject({
                error: json.jsonString("Invalid authorization token")
            }));
            return;
        }

        next();
    };
}

// Middleware: Rate limiting
fn rateLimitMiddleware(requestsPerMinute: int) Middleware {
    val limiter = concurrent.newRateLimiter(requestsPerMinute / 60);

    return (req, res, next) => {
        if limiter.tryAcquire() {
            next();
        } else {
            res.setStatus(429);
            res.json(json.jsonObject({
                error: json.jsonString("Rate limit exceeded")
            }));
        }
    };
}

// Middleware: Request body parser (JSON)
fn jsonBodyMiddleware() Middleware {
    return (req, res, next) => {
        if req.header("Content-Type") == "application/json" {
            val parsed = json.parse(req.body);
            if parsed.ok() {
                // Store parsed JSON in request (would need extension)
                next();
            } else {
                res.setStatus(400);
                res.json(json.jsonObject({
                    error: json.jsonString("Invalid JSON body")
                }));
            }
        } else {
            next();
        }
    };
}

// Static file server
fn staticMiddleware(directory: string) Middleware {
    return (req, res, next) => {
        if req.method == HttpMethod.GET {
            val filePath = io.join([directory, req.path]);

            if io.exists(filePath) && io.isFile(filePath) {
                val content = io.readFile(filePath);

                if content.ok() {
                    // Set content type based on extension
                    val ext = io.extension(filePath);
                    if ext == ".html" {
                        res.setHeader("Content-Type", "text/html");
                    } else if ext == ".css" {
                        res.setHeader("Content-Type", "text/css");
                    } else if ext == ".js" {
                        res.setHeader("Content-Type", "application/javascript");
                    } else if ext == ".json" {
                        res.setHeader("Content-Type", "application/json");
                    }

                    res.send(content.unwrap());
                    return;
                }
            }
        }

        next();
    };
}

// RESTful API helpers
class RestAPI {
    var router: Router;
    var basePath: string;

    constructor(base: string) {
        this.router = newRouter();
        this.basePath = base;
    }

    fn resource(name: string, handlers: ResourceHandlers) void {
        val path = this.basePath + "/" + name;

        // GET /resource - list
        if handlers.list != null {
            this.router.get(path, handlers.list);
        }

        // POST /resource - create
        if handlers.create != null {
            this.router.post(path, handlers.create);
        }

        // GET /resource/:id - show
        if handlers.show != null {
            this.router.get(path + "/:id", handlers.show);
        }

        // PUT /resource/:id - update
        if handlers.update != null {
            this.router.put(path + "/:id", handlers.update);
        }

        // DELETE /resource/:id - delete
        if handlers.delete != null {
            this.router.delete(path + "/:id", handlers.delete);
        }
    }
}

class ResourceHandlers {
    var list: Handler?;
    var create: Handler?;
    var show: Handler?;
    var update: Handler?;
    var delete: Handler?;
}

fn newRestAPI(basePath: string) RestAPI {
    return RestAPI(basePath);
}

// WebSocket upgrade (simplified)
class WebSocket {
    var conn: net.Conn;
    var onMessage: Function<string, void>;
    var onClose: Function;

    fn send(message: string) void;
    fn close() void;
}

// Server-Sent Events
class SSE {
    var clients: Array<Response>;
    var mutex: concurrent.Mutex;

    constructor() {
        this.clients = Array<Response>();
        this.mutex = concurrent.newMutex();
    }

    fn handler() Handler {
        return (req, res) => {
            res.setHeader("Content-Type", "text/event-stream");
            res.setHeader("Cache-Control", "no-cache");
            res.setHeader("Connection", "keep-alive");

            this.mutex.lock();
            this.clients.add(res);
            this.mutex.unlock();
        };
    }

    fn broadcast(event: string, data: string) void {
        this.mutex.lock();

        for val client in this.clients {
            client.write("event: " + event + "\n");
            client.write("data: " + data + "\n\n");
        }

        this.mutex.unlock();
    }
}

fn newSSE() SSE {
    return SSE();
}

// HTTP status codes
val StatusOK = 200;
val StatusCreated = 201;
val StatusNoContent = 204;
val StatusBadRequest = 400;
val StatusUnauthorized = 401;
val StatusForbidden = 403;
val StatusNotFound = 404;
val StatusMethodNotAllowed = 405;
val StatusConflict = 409;
val StatusInternalServerError = 500;
val StatusNotImplemented = 501;
val StatusServiceUnavailable = 503;
