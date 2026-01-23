// HTTP module - HTTP server and client
// Inspired by Go's net/http and Node.js http modules

package http;

use time;
use log;
use io;
use json;
use concurrent;

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
    pub var method: HttpMethod;
    pub var url: string;
    pub var path: string;
    pub var query: Map<string, string>;
    pub var headers: Map<string, string>;
    pub var body: string;
    pub var params: Map<string, string>;
    pub var remoteAddr: string;

    pub fn header(name: string) string;
    pub fn queryParam(name: string) string;
    pub fn param(name: string) string;
    pub fn json() Result<json.JsonValue, Error>;
}

class Response {
    pub var statusCode: int;
    pub var headers: Map<string, string>;
    pub var body: string;
    pub var written: bool;

    pub fn setHeader(name: string, value: string) void;
    pub fn setStatus(code: int) void;
    pub fn write(data: string) void;
    pub fn json(value: json.JsonValue) void;
    pub fn html(content: string) void;
    pub fn send(content: string) void;
    pub fn sendStatus(code: int) void;
    pub fn redirect(url: string) void;
}

class Router {
    pub var routes: Array<Route>;
    pub var middleware: Array<Middleware>;

    pub fn get(path: string, handler: Handler) void;
    pub fn post(path: string, handler: Handler) void;
    pub fn put(path: string, handler: Handler) void;
    pub fn delete(path: string, handler: Handler) void;
    pub fn patch(path: string, handler: Handler) void;
    pub fn use(middleware: Middleware) void;
    pub fn group(prefix: string) Router;
}

class Route {
    pub var method: HttpMethod;
    pub var pattern: string;
    pub var handler: Handler;
    pub var middleware: Array<Middleware>;
}

class Handler = Function<Request, Response, void>;
class Middleware = Function<Request, Response, Function, void>;

class Server {
    pub var addr: string;
    pub var port: int;
    pub var router: Router;
    pub var running: bool;

    pub fn listen(port: int) void;
    pub fn listenAndServe(addr: string, port: int) void;
    pub fn close() void;
}

class Client {
    pub var timeout: int;
    pub var headers: Map<string, string>;

    pub fn get(url: string) async.Promise<Response>;
    pub fn post(url: string, body: string) async.Promise<Response>;
    pub fn put(url: string, body: string) async.Promise<Response>;
    pub fn delete(url: string) async.Promise<Response>;
    pub fn request(method: HttpMethod, url: string, body: string) async.Promise<Response>;
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
fn parseURL(urlStr: string) Result<URL, Error>;

class URL {
    pub var scheme: string;
    pub var host: string;
    pub var port: int;
    pub var path: string;
    pub var query: string;
    pub var fragment: string;
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
fn quickGet(urlStr: string) async.Promise<Response> {
    val httpClient = newClient(30000);
    return httpClient.get(urlStr);
}

// Quick HTTP POST
fn quickPost(urlStr: string, bodyData: string) async.Promise<Response> {
    val httpClient = newClient(30000);
    return httpClient.post(urlStr, bodyData);
}

// Quick HTTP POST JSON
fn postJSON(urlStr: string, data: json.JsonValue) async.Promise<Response> {
    val httpClient = newClient(30000);
    httpClient.headers.put("Content-Type", "application/json");
    return httpClient.post(urlStr, json.stringify(data));
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
    pub var router: Router;
    pub var basePath: string;

    constructor(base: string) {
        this.router = newRouter();
        this.basePath = base;
    }

    pub fn resource(name: string, handlers: ResourceHandlers) void {
        val path = this.basePath + "/" + name;

        // GET /resource - list
        if handlers.list != nil {
            this.router.get(path, handlers.list);
        }

        // POST /resource - create
        if handlers.create != nil {
            this.router.post(path, handlers.create);
        }

        // GET /resource/:id - show
        if handlers.show != nil {
            this.router.get(path + "/:id", handlers.show);
        }

        // PUT /resource/:id - update
        if handlers.update != nil {
            this.router.put(path + "/:id", handlers.update);
        }

        // DELETE /resource/:id - delete
        if handlers.remove != nil {
            this.router.delete(path + "/:id", handlers.remove);
        }
    }
}

class ResourceHandlers {
    pub var list: Handler?;
    pub var create: Handler?;
    pub var show: Handler?;
    pub var update: Handler?;
    pub var remove: Handler?;
}

fn newRestAPI(basePath: string) RestAPI {
    return RestAPI(basePath);
}

// WebSocket upgrade (simplified)
class WebSocket {
    pub var conn: net.Conn;
    pub var onMessage: Function<string, void>;
    pub var onClose: Function;

    pub fn send(message: string) void;
    pub fn close() void;
}

// Server-Sent Events
class SSE {
    pub var clients: Array<Response>;
    pub var mutex: concurrent.Mutex;

    constructor() {
        this.clients = Array<Response>();
        this.mutex = concurrent.newMutex();
    }

    pub fn handler() Handler {
        return (req, res) => {
            res.setHeader("Content-Type", "text/event-stream");
            res.setHeader("Cache-Control", "no-cache");
            res.setHeader("Connection", "keep-alive");

            this.mutex.lock();
            this.clients.add(res);
            this.mutex.unlock();
        };
    }

    pub fn broadcast(event: string, data: string) void {
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
