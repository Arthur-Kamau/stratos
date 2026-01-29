// Full-Stack Web Application Example
// Demonstrates: PostgreSQL, Redis sessions, HTML templating, middleware, forms
package main;

use net.http;
use db.psql;
use db.redis;
use template;
use encoding.json;
use crypto;
use log;

// ============================================================================
// Configuration
// ============================================================================

val DB_CONNECTION = "host=localhost port=5432 dbname=webapp user=postgres password=postgres";
val REDIS_HOST = "localhost";
val REDIS_PORT = 6379;
val SESSION_TTL = 3600;  // 1 hour
val SERVER_PORT = 8080;

// ============================================================================
// Models
// ============================================================================

class User {
    var id: int;
    var username: string;
    var email: string;
    var passwordHash: string;
    var createdAt: string;
}

class Post {
    var id: int;
    var userId: int;
    var title: string;
    var content: string;
    var createdAt: string;
    var authorName: string;
}

// ============================================================================
// Database Layer
// ============================================================================

class Database {
    var conn: psql.Connection;

    constructor(connectionString: string) {
        this.conn = psql.connect(connectionString);
        this.initSchema();
    }

    fn initSchema() {
        this.conn.exec("
            CREATE TABLE IF NOT EXISTS users (
                id SERIAL PRIMARY KEY,
                username VARCHAR(50) UNIQUE NOT NULL,
                email VARCHAR(255) UNIQUE NOT NULL,
                password_hash VARCHAR(255) NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        ");

        this.conn.exec("
            CREATE TABLE IF NOT EXISTS posts (
                id SERIAL PRIMARY KEY,
                user_id INT REFERENCES users(id),
                title VARCHAR(200) NOT NULL,
                content TEXT NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        ");
    }

    pub fn createUser(username: string, email: string, passwordHash: string) int {
        val rows = this.conn.queryParams(
            "INSERT INTO users (username, email, password_hash) VALUES ($1, $2, $3) RETURNING id",
            [username, email, passwordHash]
        );
        var id = 0;
        if rows.next() {
            id = rows.getInt(0);
        }
        rows.close();
        return id;
    }

    pub fn getUserByUsername(username: string) User? {
        val rows = this.conn.queryParams(
            "SELECT id, username, email, password_hash, created_at FROM users WHERE username = $1",
            [username]
        );

        if rows.next() {
            var user = User();
            user.id = rows.getInt(0);
            user.username = rows.getString(1);
            user.email = rows.getString(2);
            user.passwordHash = rows.getString(3);
            user.createdAt = rows.getString(4);
            rows.close();
            return user;
        }

        rows.close();
        return null;
    }

    pub fn getUserById(id: int) User? {
        val rows = this.conn.queryParams(
            "SELECT id, username, email, password_hash, created_at FROM users WHERE id = $1",
            [id.toString()]
        );

        if rows.next() {
            var user = User();
            user.id = rows.getInt(0);
            user.username = rows.getString(1);
            user.email = rows.getString(2);
            user.passwordHash = rows.getString(3);
            user.createdAt = rows.getString(4);
            rows.close();
            return user;
        }

        rows.close();
        return null;
    }

    pub fn createPost(userId: int, title: string, content: string) int {
        val rows = this.conn.queryParams(
            "INSERT INTO posts (user_id, title, content) VALUES ($1, $2, $3) RETURNING id",
            [userId.toString(), title, content]
        );
        var id = 0;
        if rows.next() {
            id = rows.getInt(0);
        }
        rows.close();
        return id;
    }

    pub fn getPosts(limit: int) array<Post> {
        var posts: array<Post> = [];
        val rows = this.conn.queryParams(
            "SELECT p.id, p.user_id, p.title, p.content, p.created_at, u.username
             FROM posts p
             JOIN users u ON p.user_id = u.id
             ORDER BY p.created_at DESC
             LIMIT $1",
            [limit.toString()]
        );

        while rows.next() {
            var post = Post();
            post.id = rows.getInt(0);
            post.userId = rows.getInt(1);
            post.title = rows.getString(2);
            post.content = rows.getString(3);
            post.createdAt = rows.getString(4);
            post.authorName = rows.getString(5);
            posts.push(post);
        }

        rows.close();
        return posts;
    }

    pub fn getPostById(id: int) Post? {
        val rows = this.conn.queryParams(
            "SELECT p.id, p.user_id, p.title, p.content, p.created_at, u.username
             FROM posts p
             JOIN users u ON p.user_id = u.id
             WHERE p.id = $1",
            [id.toString()]
        );

        if rows.next() {
            var post = Post();
            post.id = rows.getInt(0);
            post.userId = rows.getInt(1);
            post.title = rows.getString(2);
            post.content = rows.getString(3);
            post.createdAt = rows.getString(4);
            post.authorName = rows.getString(5);
            rows.close();
            return post;
        }

        rows.close();
        return null;
    }

    pub fn close() {
        this.conn.close();
    }
}

// ============================================================================
// Session Management (Redis)
// ============================================================================

class SessionManager {
    var client: redis.Client;
    var ttl: int;

    constructor(host: string, port: int, sessionTtl: int) {
        this.client = redis.connect(host, port);
        this.ttl = sessionTtl;
    }

    pub fn create(userId: int, username: string) string {
        val sessionId = crypto.randomHex(32);
        val key = "session:" + sessionId;

        this.client.hset(key, "userId", userId.toString());
        this.client.hset(key, "username", username);
        this.client.expire(key, this.ttl);

        return sessionId;
    }

    pub fn get(sessionId: string) map<string, string>? {
        val key = "session:" + sessionId;

        if !this.client.exists(key) {
            return null;
        }

        var session: map<string, string> = {};
        session["userId"] = this.client.hget(key, "userId");
        session["username"] = this.client.hget(key, "username");

        // Refresh TTL
        this.client.expire(key, this.ttl);

        return session;
    }

    pub fn destroy(sessionId: string) {
        this.client.del("session:" + sessionId);
    }

    pub fn close() {
        this.client.close();
    }
}

// ============================================================================
// Template Rendering
// ============================================================================

fn renderLayout(title: string, content: string, user: User?) string {
    val ctx = template.context()
        .set("title", title)
        .set("content", content)
        .setBool("isLoggedIn", user != null);

    if user != null {
        ctx.set("username", user.username);
    }

    val html = template.render("
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset=\"UTF-8\">
            <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">
            <title>{{ title }} - Stratos Blog</title>
            <style>
                body { font-family: system-ui, sans-serif; max-width: 800px; margin: 0 auto; padding: 20px; }
                nav { background: #333; padding: 10px 20px; margin: -20px -20px 20px; }
                nav a { color: white; text-decoration: none; margin-right: 15px; }
                nav a:hover { text-decoration: underline; }
                .post { border: 1px solid #ddd; padding: 15px; margin-bottom: 15px; border-radius: 5px; }
                .post h2 { margin-top: 0; }
                .post .meta { color: #666; font-size: 0.9em; }
                form { max-width: 400px; }
                input, textarea { width: 100%; padding: 8px; margin-bottom: 10px; border: 1px solid #ddd; border-radius: 4px; }
                button { background: #007bff; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; }
                button:hover { background: #0056b3; }
                .error { color: red; padding: 10px; background: #ffe0e0; border-radius: 4px; margin-bottom: 10px; }
                .success { color: green; padding: 10px; background: #e0ffe0; border-radius: 4px; margin-bottom: 10px; }
            </style>
        </head>
        <body>
            <nav>
                <a href=\"/\">Home</a>
                {% if isLoggedIn %}
                    <a href=\"/posts/new\">New Post</a>
                    <a href=\"/logout\">Logout ({{ username }})</a>
                {% else %}
                    <a href=\"/login\">Login</a>
                    <a href=\"/register\">Register</a>
                {% endif %}
            </nav>
            {{{ content }}}
        </body>
        </html>
    ", ctx);

    ctx.free();
    return html;
}

fn renderHomePage(posts: array<Post>, user: User?) string {
    val ctx = template.context()
        .setBool("hasPosts", posts.length() > 0);

    // Convert posts to template-friendly format
    var postTitles: array<string> = [];
    var postContents: array<string> = [];
    for post in posts {
        postTitles.push(post.title);
        postContents.push("<div class=\"post\"><h2><a href=\"/posts/" + post.id.toString() + "\">" +
            template.escapeHtml(post.title) + "</a></h2><p class=\"meta\">By " +
            template.escapeHtml(post.authorName) + " on " + post.createdAt + "</p><p>" +
            template.escapeHtml(post.content.substring(0, 200)) + "...</p></div>");
    }
    ctx.setArray("posts", postContents);

    val content = template.render("
        <h1>Latest Posts</h1>
        {% if hasPosts %}
            {% for post in posts %}
                {{{ post }}}
            {% endfor %}
        {% else %}
            <p>No posts yet. Be the first to write one!</p>
        {% endif %}
    ", ctx);

    ctx.free();
    return renderLayout("Home", content, user);
}

fn renderLoginPage(error: string) string {
    val ctx = template.context()
        .set("error", error)
        .setBool("hasError", error != "");

    val content = template.render("
        <h1>Login</h1>
        {% if hasError %}
            <div class=\"error\">{{ error }}</div>
        {% endif %}
        <form method=\"POST\" action=\"/login\">
            <input type=\"text\" name=\"username\" placeholder=\"Username\" required>
            <input type=\"password\" name=\"password\" placeholder=\"Password\" required>
            <button type=\"submit\">Login</button>
        </form>
        <p>Don't have an account? <a href=\"/register\">Register here</a></p>
    ", ctx);

    ctx.free();
    return renderLayout("Login", content, null);
}

fn renderRegisterPage(error: string) string {
    val ctx = template.context()
        .set("error", error)
        .setBool("hasError", error != "");

    val content = template.render("
        <h1>Register</h1>
        {% if hasError %}
            <div class=\"error\">{{ error }}</div>
        {% endif %}
        <form method=\"POST\" action=\"/register\">
            <input type=\"text\" name=\"username\" placeholder=\"Username\" required>
            <input type=\"email\" name=\"email\" placeholder=\"Email\" required>
            <input type=\"password\" name=\"password\" placeholder=\"Password\" required>
            <input type=\"password\" name=\"confirm\" placeholder=\"Confirm Password\" required>
            <button type=\"submit\">Register</button>
        </form>
        <p>Already have an account? <a href=\"/login\">Login here</a></p>
    ", ctx);

    ctx.free();
    return renderLayout("Register", content, null);
}

fn renderNewPostPage(user: User) string {
    val content = "
        <h1>Create New Post</h1>
        <form method=\"POST\" action=\"/posts/new\">
            <input type=\"text\" name=\"title\" placeholder=\"Post Title\" required>
            <textarea name=\"content\" placeholder=\"Write your post...\" rows=\"10\" required></textarea>
            <button type=\"submit\">Publish</button>
        </form>
    ";
    return renderLayout("New Post", content, user);
}

fn renderPostPage(post: Post, user: User?) string {
    val ctx = template.context()
        .set("title", post.title)
        .set("author", post.authorName)
        .set("date", post.createdAt)
        .set("content", post.content);

    val content = template.render("
        <article>
            <h1>{{ title }}</h1>
            <p class=\"meta\">By {{ author }} on {{ date }}</p>
            <div class=\"content\">
                {{ content }}
            </div>
        </article>
        <p><a href=\"/\">&larr; Back to Home</a></p>
    ", ctx);

    ctx.free();
    return renderLayout(post.title, content, user);
}

// ============================================================================
// Form Parsing Helper
// ============================================================================

fn parseFormData(body: string) map<string, string> {
    var data: map<string, string> = {};
    val pairs = body.split("&");
    for pair in pairs {
        val parts = pair.split("=");
        if parts.length() == 2 {
            // URL decode (simplified)
            var key = parts[0];
            var value = parts[1].replace("+", " ");
            data[key] = value;
        }
    }
    return data;
}

// ============================================================================
// Middleware
// ============================================================================

// Global state (would be injected in a real app)
var db: Database;
var sessions: SessionManager;

fn authMiddleware(req: http.Request, res: http.Response) bool {
    // Check for session cookie
    val cookies = req.header("Cookie");
    if cookies != "" {
        val parts = cookies.split(";");
        for part in parts {
            val kv = part.trim().split("=");
            if kv.length() == 2 && kv[0] == "session" {
                val session = sessions.get(kv[1]);
                if session != null {
                    // Add user info to request headers for downstream handlers
                    req.setHeader("X-User-Id", session["userId"]);
                    req.setHeader("X-Username", session["username"]);
                }
            }
        }
    }
    return true;  // Continue chain
}

fn loggingMiddleware(req: http.Request, res: http.Response) bool {
    log.info(req.method() + " " + req.path());
    return true;
}

// ============================================================================
// Route Handlers
// ============================================================================

fn handleHome(req: http.Request, res: http.Response) {
    val userId = req.header("X-User-Id");
    var user: User? = null;
    if userId != "" {
        user = db.getUserById(userId.toInt());
    }

    val posts = db.getPosts(10);
    val html = renderHomePage(posts, user);

    res.header("Content-Type", "text/html");
    res.send(html);
}

fn handleLogin(req: http.Request, res: http.Response) {
    if req.method() == "GET" {
        res.header("Content-Type", "text/html");
        res.send(renderLoginPage(""));
        return;
    }

    // POST - process login
    val form = parseFormData(req.body());
    val username = form["username"] ?? "";
    val password = form["password"] ?? "";

    val user = db.getUserByUsername(username);
    if user == null {
        res.header("Content-Type", "text/html");
        res.send(renderLoginPage("Invalid username or password"));
        return;
    }

    // Verify password (simplified - in production use bcrypt)
    val passwordHash = crypto.sha256(password);
    if passwordHash != user.passwordHash {
        res.header("Content-Type", "text/html");
        res.send(renderLoginPage("Invalid username or password"));
        return;
    }

    // Create session
    val sessionId = sessions.create(user.id, user.username);

    res.header("Set-Cookie", "session=" + sessionId + "; Path=/; HttpOnly");
    res.redirect("/");
}

fn handleRegister(req: http.Request, res: http.Response) {
    if req.method() == "GET" {
        res.header("Content-Type", "text/html");
        res.send(renderRegisterPage(""));
        return;
    }

    // POST - process registration
    val form = parseFormData(req.body());
    val username = form["username"] ?? "";
    val email = form["email"] ?? "";
    val password = form["password"] ?? "";
    val confirm = form["confirm"] ?? "";

    if password != confirm {
        res.header("Content-Type", "text/html");
        res.send(renderRegisterPage("Passwords do not match"));
        return;
    }

    if username.length() < 3 {
        res.header("Content-Type", "text/html");
        res.send(renderRegisterPage("Username must be at least 3 characters"));
        return;
    }

    // Check if user exists
    val existing = db.getUserByUsername(username);
    if existing != null {
        res.header("Content-Type", "text/html");
        res.send(renderRegisterPage("Username already taken"));
        return;
    }

    // Create user
    val passwordHash = crypto.sha256(password);
    val userId = db.createUser(username, email, passwordHash);

    // Create session and login
    val sessionId = sessions.create(userId, username);

    res.header("Set-Cookie", "session=" + sessionId + "; Path=/; HttpOnly");
    res.redirect("/");
}

fn handleLogout(req: http.Request, res: http.Response) {
    // Get session from cookie and destroy it
    val cookies = req.header("Cookie");
    if cookies != "" {
        val parts = cookies.split(";");
        for part in parts {
            val kv = part.trim().split("=");
            if kv.length() == 2 && kv[0] == "session" {
                sessions.destroy(kv[1]);
            }
        }
    }

    res.header("Set-Cookie", "session=; Path=/; HttpOnly; Max-Age=0");
    res.redirect("/");
}

fn handleNewPost(req: http.Request, res: http.Response) {
    val userId = req.header("X-User-Id");
    if userId == "" {
        res.redirect("/login");
        return;
    }

    val user = db.getUserById(userId.toInt());
    if user == null {
        res.redirect("/login");
        return;
    }

    if req.method() == "GET" {
        res.header("Content-Type", "text/html");
        res.send(renderNewPostPage(user));
        return;
    }

    // POST - create post
    val form = parseFormData(req.body());
    val title = form["title"] ?? "";
    val content = form["content"] ?? "";

    if title == "" || content == "" {
        res.header("Content-Type", "text/html");
        res.send(renderNewPostPage(user));
        return;
    }

    val postId = db.createPost(user.id, title, content);
    res.redirect("/posts/" + postId.toString());
}

fn handleViewPost(req: http.Request, res: http.Response) {
    val postId = req.param("id").toInt();
    val post = db.getPostById(postId);

    if post == null {
        res.status(404);
        res.header("Content-Type", "text/html");
        res.send(renderLayout("Not Found", "<h1>Post Not Found</h1>", null));
        return;
    }

    val userId = req.header("X-User-Id");
    var user: User? = null;
    if userId != "" {
        user = db.getUserById(userId.toInt());
    }

    res.header("Content-Type", "text/html");
    res.send(renderPostPage(post, user));
}

// ============================================================================
// API Routes (JSON)
// ============================================================================

fn handleApiPosts(req: http.Request, res: http.Response) {
    val posts = db.getPosts(20);

    var postsJson: array<string> = [];
    for post in posts {
        postsJson.push("{\"id\":" + post.id.toString() +
            ",\"title\":\"" + post.title.replace("\"", "\\\"") +
            "\",\"author\":\"" + post.authorName +
            "\",\"createdAt\":\"" + post.createdAt + "\"}");
    }

    res.header("Content-Type", "application/json");
    res.send("{\"posts\":[" + postsJson.join(",") + "]}");
}

// ============================================================================
// Main
// ============================================================================

fn main() {
    log.info("Starting Full Web Application...");

    // Initialize database and session manager
    db = Database(DB_CONNECTION);
    sessions = SessionManager(REDIS_HOST, REDIS_PORT, SESSION_TTL);

    // Create router
    val router = http.newRouter();

    // Add middleware
    router.use(loggingMiddleware);
    router.use(authMiddleware);

    // Public routes
    router.get("/", handleHome);
    router.get("/login", handleLogin);
    router.post("/login", handleLogin);
    router.get("/register", handleRegister);
    router.post("/register", handleRegister);
    router.get("/logout", handleLogout);

    // Protected routes
    router.get("/posts/new", handleNewPost);
    router.post("/posts/new", handleNewPost);
    router.get("/posts/:id", handleViewPost);

    // API routes
    val api = router.group("/api");
    api.get("/posts", handleApiPosts);

    // Create and start server
    val server = http.newServer(router);

    log.info("Server listening on port " + SERVER_PORT.toString());
    log.info("Visit http://localhost:" + SERVER_PORT.toString());

    server.listen(SERVER_PORT);

    // Cleanup
    db.close();
    sessions.close();
}
