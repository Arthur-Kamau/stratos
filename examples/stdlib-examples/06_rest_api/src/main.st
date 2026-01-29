// REST API Example
// Demonstrates: CRUD operations, PostgreSQL, validation, rate limiting, API versioning
package main;

use net.http;
use db.psql;
use db.redis;
use encoding.json;
use log;

// ============================================================================
// Configuration
// ============================================================================

val DB_CONNECTION = "host=localhost port=5432 dbname=restapi user=postgres password=postgres";
val REDIS_HOST = "localhost";
val REDIS_PORT = 6379;
val SERVER_PORT = 8080;
val RATE_LIMIT_MAX = 100;      // Max requests per window
val RATE_LIMIT_WINDOW = 60;    // Window in seconds

// ============================================================================
// Models
// ============================================================================

class Product {
    var id: int;
    var name: string;
    var description: string;
    var price: double;
    var stock: int;
    var category: string;
    var createdAt: string;
    var updatedAt: string;
}

class ApiResponse {
    var success: bool;
    var data: any;
    var error: string;
    var meta: map<string, any>;
}

// ============================================================================
// Database Repository
// ============================================================================

class ProductRepository {
    var conn: psql.Connection;

    constructor(connectionString: string) {
        this.conn = psql.connect(connectionString);
        this.initSchema();
    }

    fn initSchema() {
        this.conn.exec("
            CREATE TABLE IF NOT EXISTS products (
                id SERIAL PRIMARY KEY,
                name VARCHAR(200) NOT NULL,
                description TEXT,
                price DECIMAL(10,2) NOT NULL,
                stock INT DEFAULT 0,
                category VARCHAR(100),
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        ");

        // Create index for category searches
        this.conn.exec("
            CREATE INDEX IF NOT EXISTS idx_products_category ON products(category)
        ");
    }

    pub fn findAll(limit: int, offset: int, category: string) array<Product> {
        var products: array<Product> = [];
        var sql = "SELECT id, name, description, price, stock, category, created_at, updated_at FROM products";
        var params: array<string> = [];

        if category != "" {
            sql = sql + " WHERE category = $1 ORDER BY created_at DESC LIMIT $2 OFFSET $3";
            params = [category, limit.toString(), offset.toString()];
        } else {
            sql = sql + " ORDER BY created_at DESC LIMIT $1 OFFSET $2";
            params = [limit.toString(), offset.toString()];
        }

        val rows = this.conn.queryParams(sql, params);

        while rows.next() {
            var product = Product();
            product.id = rows.getInt(0);
            product.name = rows.getString(1);
            product.description = rows.getString(2);
            product.price = rows.getDouble(3);
            product.stock = rows.getInt(4);
            product.category = rows.getString(5);
            product.createdAt = rows.getString(6);
            product.updatedAt = rows.getString(7);
            products.push(product);
        }

        rows.close();
        return products;
    }

    pub fn findById(id: int) Product? {
        val rows = this.conn.queryParams(
            "SELECT id, name, description, price, stock, category, created_at, updated_at FROM products WHERE id = $1",
            [id.toString()]
        );

        if rows.next() {
            var product = Product();
            product.id = rows.getInt(0);
            product.name = rows.getString(1);
            product.description = rows.getString(2);
            product.price = rows.getDouble(3);
            product.stock = rows.getInt(4);
            product.category = rows.getString(5);
            product.createdAt = rows.getString(6);
            product.updatedAt = rows.getString(7);
            rows.close();
            return product;
        }

        rows.close();
        return null;
    }

    pub fn create(name: string, description: string, price: double, stock: int, category: string) Product? {
        val rows = this.conn.queryParams(
            "INSERT INTO products (name, description, price, stock, category) VALUES ($1, $2, $3, $4, $5) RETURNING id, created_at, updated_at",
            [name, description, price.toString(), stock.toString(), category]
        );

        if rows.next() {
            var product = Product();
            product.id = rows.getInt(0);
            product.name = name;
            product.description = description;
            product.price = price;
            product.stock = stock;
            product.category = category;
            product.createdAt = rows.getString(1);
            product.updatedAt = rows.getString(2);
            rows.close();
            return product;
        }

        rows.close();
        return null;
    }

    pub fn update(id: int, name: string, description: string, price: double, stock: int, category: string) Product? {
        val affected = this.conn.execParams(
            "UPDATE products SET name = $1, description = $2, price = $3, stock = $4, category = $5, updated_at = CURRENT_TIMESTAMP WHERE id = $6",
            [name, description, price.toString(), stock.toString(), category, id.toString()]
        );

        if affected > 0 {
            return this.findById(id);
        }

        return null;
    }

    pub fn delete(id: int) bool {
        val affected = this.conn.execParams(
            "DELETE FROM products WHERE id = $1",
            [id.toString()]
        );
        return affected > 0;
    }

    pub fn count(category: string) int {
        var sql = "SELECT COUNT(*) FROM products";
        var params: array<string> = [];

        if category != "" {
            sql = sql + " WHERE category = $1";
            params = [category];
        }

        val rows = this.conn.queryParams(sql, params);
        var count = 0;
        if rows.next() {
            count = rows.getInt(0);
        }
        rows.close();
        return count;
    }

    pub fn close() {
        this.conn.close();
    }
}

// ============================================================================
// Rate Limiter
// ============================================================================

class RateLimiter {
    var client: redis.Client;
    var maxRequests: int;
    var windowSeconds: int;

    constructor(host: string, port: int, max: int, window: int) {
        this.client = redis.connect(host, port);
        this.maxRequests = max;
        this.windowSeconds = window;
    }

    pub fn isAllowed(identifier: string) bool {
        val key = "ratelimit:" + identifier;
        val count = this.client.incr(key);

        if count == 1 {
            this.client.expire(key, this.windowSeconds);
        }

        return count <= this.maxRequests;
    }

    pub fn remaining(identifier: string) int {
        val key = "ratelimit:" + identifier;
        val current = this.client.get(key);
        if current == "" {
            return this.maxRequests;
        }
        return this.maxRequests - current.toInt();
    }

    pub fn reset(identifier: string) int {
        val key = "ratelimit:" + identifier;
        return this.client.ttl(key);
    }

    pub fn close() {
        this.client.close();
    }
}

// ============================================================================
// JSON Helpers
// ============================================================================

fn productToJson(product: Product) string {
    return "{" +
        "\"id\":" + product.id.toString() + "," +
        "\"name\":\"" + escapeJson(product.name) + "\"," +
        "\"description\":\"" + escapeJson(product.description) + "\"," +
        "\"price\":" + product.price.toString() + "," +
        "\"stock\":" + product.stock.toString() + "," +
        "\"category\":\"" + escapeJson(product.category) + "\"," +
        "\"createdAt\":\"" + product.createdAt + "\"," +
        "\"updatedAt\":\"" + product.updatedAt + "\"" +
    "}";
}

fn productsToJson(products: array<Product>) string {
    var items: array<string> = [];
    for product in products {
        items.push(productToJson(product));
    }
    return "[" + items.join(",") + "]";
}

fn escapeJson(str: string) string {
    return str.replace("\\", "\\\\")
              .replace("\"", "\\\"")
              .replace("\n", "\\n")
              .replace("\r", "\\r")
              .replace("\t", "\\t");
}

fn successResponse(data: string) string {
    return "{\"success\":true,\"data\":" + data + "}";
}

fn errorResponse(message: string, status: int) string {
    return "{\"success\":false,\"error\":{\"message\":\"" + escapeJson(message) + "\",\"status\":" + status.toString() + "}}";
}

fn paginatedResponse(data: string, total: int, limit: int, offset: int) string {
    val totalPages = (total + limit - 1) / limit;
    val currentPage = (offset / limit) + 1;
    return "{\"success\":true,\"data\":" + data + ",\"meta\":{\"total\":" + total.toString() +
           ",\"limit\":" + limit.toString() + ",\"offset\":" + offset.toString() +
           ",\"totalPages\":" + totalPages.toString() + ",\"currentPage\":" + currentPage.toString() + "}}";
}

// ============================================================================
// Request Validation
// ============================================================================

class ValidationError {
    var field: string;
    var message: string;
}

fn validateProduct(body: string) array<ValidationError> {
    var errors: array<ValidationError> = [];

    // Parse JSON (simplified - in production use proper JSON parser)
    val data = json.decode(body);

    // Validate name
    val name = data["name"] ?? "";
    if name == "" {
        var err = ValidationError();
        err.field = "name";
        err.message = "Name is required";
        errors.push(err);
    } else if name.length() > 200 {
        var err = ValidationError();
        err.field = "name";
        err.message = "Name must be 200 characters or less";
        errors.push(err);
    }

    // Validate price
    val priceStr = data["price"] ?? "";
    if priceStr == "" {
        var err = ValidationError();
        err.field = "price";
        err.message = "Price is required";
        errors.push(err);
    } else {
        val price = priceStr.toDouble();
        if price < 0 {
            var err = ValidationError();
            err.field = "price";
            err.message = "Price must be non-negative";
            errors.push(err);
        }
    }

    // Validate stock
    val stockStr = data["stock"] ?? "0";
    val stock = stockStr.toInt();
    if stock < 0 {
        var err = ValidationError();
        err.field = "stock";
        err.message = "Stock must be non-negative";
        errors.push(err);
    }

    return errors;
}

fn validationErrorsToJson(errors: array<ValidationError>) string {
    var items: array<string> = [];
    for err in errors {
        items.push("{\"field\":\"" + err.field + "\",\"message\":\"" + escapeJson(err.message) + "\"}");
    }
    return "{\"success\":false,\"error\":{\"message\":\"Validation failed\",\"status\":400,\"details\":[" + items.join(",") + "]}}";
}

// ============================================================================
// Middleware
// ============================================================================

var repo: ProductRepository;
var limiter: RateLimiter;

fn rateLimitMiddleware(req: http.Request, res: http.Response) bool {
    // Use client IP as identifier (simplified)
    val clientId = req.header("X-Forwarded-For");
    val identifier = clientId != "" ? clientId : "default";

    if !limiter.isAllowed(identifier) {
        res.status(429);
        res.header("Content-Type", "application/json");
        res.header("X-RateLimit-Remaining", "0");
        res.header("X-RateLimit-Reset", limiter.reset(identifier).toString());
        res.send(errorResponse("Rate limit exceeded. Please try again later.", 429));
        return false;
    }

    // Add rate limit headers
    res.header("X-RateLimit-Limit", RATE_LIMIT_MAX.toString());
    res.header("X-RateLimit-Remaining", limiter.remaining(identifier).toString());
    res.header("X-RateLimit-Reset", limiter.reset(identifier).toString());

    return true;
}

fn corsMiddleware(req: http.Request, res: http.Response) bool {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.header("Access-Control-Allow-Headers", "Content-Type, Authorization");

    // Handle preflight
    if req.method() == "OPTIONS" {
        res.status(204);
        res.send("");
        return false;
    }

    return true;
}

fn jsonContentTypeMiddleware(req: http.Request, res: http.Response) bool {
    res.header("Content-Type", "application/json");
    return true;
}

// ============================================================================
// API Handlers - v1
// ============================================================================

fn handleListProducts(req: http.Request, res: http.Response) {
    // Parse query parameters
    val limitStr = req.query("limit");
    val offsetStr = req.query("offset");
    val category = req.query("category");

    val limit = limitStr != "" ? limitStr.toInt() : 20;
    val offset = offsetStr != "" ? offsetStr.toInt() : 0;

    // Validate limit
    val actualLimit = limit > 100 ? 100 : (limit < 1 ? 20 : limit);

    val products = repo.findAll(actualLimit, offset, category);
    val total = repo.count(category);

    res.send(paginatedResponse(productsToJson(products), total, actualLimit, offset));
}

fn handleGetProduct(req: http.Request, res: http.Response) {
    val id = req.param("id").toInt();

    if id <= 0 {
        res.status(400);
        res.send(errorResponse("Invalid product ID", 400));
        return;
    }

    val product = repo.findById(id);

    if product == null {
        res.status(404);
        res.send(errorResponse("Product not found", 404));
        return;
    }

    res.send(successResponse(productToJson(product)));
}

fn handleCreateProduct(req: http.Request, res: http.Response) {
    val body = req.body();

    // Validate
    val errors = validateProduct(body);
    if errors.length() > 0 {
        res.status(400);
        res.send(validationErrorsToJson(errors));
        return;
    }

    // Parse body
    val data = json.decode(body);
    val name = data["name"] ?? "";
    val description = data["description"] ?? "";
    val price = (data["price"] ?? "0").toDouble();
    val stock = (data["stock"] ?? "0").toInt();
    val category = data["category"] ?? "";

    val product = repo.create(name, description, price, stock, category);

    if product == null {
        res.status(500);
        res.send(errorResponse("Failed to create product", 500));
        return;
    }

    res.status(201);
    res.header("Location", "/api/v1/products/" + product.id.toString());
    res.send(successResponse(productToJson(product)));
}

fn handleUpdateProduct(req: http.Request, res: http.Response) {
    val id = req.param("id").toInt();

    if id <= 0 {
        res.status(400);
        res.send(errorResponse("Invalid product ID", 400));
        return;
    }

    // Check if exists
    val existing = repo.findById(id);
    if existing == null {
        res.status(404);
        res.send(errorResponse("Product not found", 404));
        return;
    }

    val body = req.body();

    // Validate
    val errors = validateProduct(body);
    if errors.length() > 0 {
        res.status(400);
        res.send(validationErrorsToJson(errors));
        return;
    }

    // Parse body
    val data = json.decode(body);
    val name = data["name"] ?? existing.name;
    val description = data["description"] ?? existing.description;
    val price = (data["price"] ?? existing.price.toString()).toDouble();
    val stock = (data["stock"] ?? existing.stock.toString()).toInt();
    val category = data["category"] ?? existing.category;

    val product = repo.update(id, name, description, price, stock, category);

    if product == null {
        res.status(500);
        res.send(errorResponse("Failed to update product", 500));
        return;
    }

    res.send(successResponse(productToJson(product)));
}

fn handleDeleteProduct(req: http.Request, res: http.Response) {
    val id = req.param("id").toInt();

    if id <= 0 {
        res.status(400);
        res.send(errorResponse("Invalid product ID", 400));
        return;
    }

    val deleted = repo.delete(id);

    if !deleted {
        res.status(404);
        res.send(errorResponse("Product not found", 404));
        return;
    }

    res.status(204);
    res.send("");
}

// ============================================================================
// Health Check
// ============================================================================

fn handleHealth(req: http.Request, res: http.Response) {
    res.send("{\"status\":\"healthy\",\"version\":\"1.0.0\"}");
}

// ============================================================================
// Main
// ============================================================================

fn main() {
    log.info("Starting REST API Server...");

    // Initialize database and rate limiter
    repo = ProductRepository(DB_CONNECTION);
    limiter = RateLimiter(REDIS_HOST, REDIS_PORT, RATE_LIMIT_MAX, RATE_LIMIT_WINDOW);

    // Create router
    val router = http.newRouter();

    // Global middleware
    router.use(corsMiddleware);
    router.use(rateLimitMiddleware);
    router.use(jsonContentTypeMiddleware);

    // Health check
    router.get("/health", handleHealth);

    // API v1 routes
    val v1 = router.group("/api/v1");

    // Products CRUD
    v1.get("/products", handleListProducts);
    v1.get("/products/:id", handleGetProduct);
    v1.post("/products", handleCreateProduct);
    v1.put("/products/:id", handleUpdateProduct);
    v1.delete("/products/:id", handleDeleteProduct);

    // Create and start server
    val server = http.newServer(router);

    log.info("REST API listening on port " + SERVER_PORT.toString());
    log.info("API Documentation:");
    log.info("  GET    /health              - Health check");
    log.info("  GET    /api/v1/products     - List products");
    log.info("  GET    /api/v1/products/:id - Get product by ID");
    log.info("  POST   /api/v1/products     - Create product");
    log.info("  PUT    /api/v1/products/:id - Update product");
    log.info("  DELETE /api/v1/products/:id - Delete product");

    server.listen(SERVER_PORT);

    // Cleanup
    repo.close();
    limiter.close();
}
