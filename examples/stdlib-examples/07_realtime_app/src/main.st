// Real-time Application Example
// Demonstrates: Server-Sent Events (SSE), Redis pub/sub, live dashboard
package main;

use net.http;
use db.redis;
use template;
use encoding.json;
use log;
use time;
use concurrent;

// ============================================================================
// Configuration
// ============================================================================

val REDIS_HOST = "localhost";
val REDIS_PORT = 6379;
val SERVER_PORT = 8080;

// ============================================================================
// Metrics Tracking
// ============================================================================

class Metrics {
    var client: redis.Client;

    constructor(host: string, port: int) {
        this.client = redis.connect(host, port);
    }

    pub fn incrementPageViews() int {
        return this.client.incr("metrics:pageviews");
    }

    pub fn incrementApiCalls() int {
        return this.client.incr("metrics:apicalls");
    }

    pub fn incrementErrors() int {
        return this.client.incr("metrics:errors");
    }

    pub fn getPageViews() int {
        val value = this.client.get("metrics:pageviews");
        return value != "" ? value.toInt() : 0;
    }

    pub fn getApiCalls() int {
        val value = this.client.get("metrics:apicalls");
        return value != "" ? value.toInt() : 0;
    }

    pub fn getErrors() int {
        val value = this.client.get("metrics:errors");
        return value != "" ? value.toInt() : 0;
    }

    pub fn recordResponseTime(ms: int) {
        // Store in a list for averaging
        this.client.lpush("metrics:response_times", ms.toString());
        // Keep only last 100 entries
        this.client.lrange("metrics:response_times", 0, 99);
    }

    pub fn getAverageResponseTime() double {
        val times = this.client.lrange("metrics:response_times", 0, 99);
        if times.length() == 0 {
            return 0.0;
        }

        var sum = 0;
        for t in times {
            sum = sum + t.toInt();
        }
        return sum.toDouble() / times.length().toDouble();
    }

    pub fn addActiveUser(userId: string) {
        this.client.sadd("metrics:active_users", userId);
        this.client.expire("metrics:active_users", 300);  // 5 min TTL
    }

    pub fn getActiveUserCount() int {
        return this.client.smembers("metrics:active_users").length();
    }

    pub fn publishEvent(eventType: string, data: string) {
        val event = "{\"type\":\"" + eventType + "\",\"data\":" + data + ",\"timestamp\":" + time.now().toString() + "}";
        this.client.publish("events", event);
    }

    pub fn reset() {
        this.client.del("metrics:pageviews");
        this.client.del("metrics:apicalls");
        this.client.del("metrics:errors");
        this.client.del("metrics:response_times");
        this.client.del("metrics:active_users");
    }

    pub fn close() {
        this.client.close();
    }
}

// ============================================================================
// Chat Room
// ============================================================================

class ChatRoom {
    var client: redis.Client;
    var roomId: string;

    constructor(host: string, port: int, room: string) {
        this.client = redis.connect(host, port);
        this.roomId = room;
    }

    pub fn sendMessage(username: string, message: string) {
        val msg = "{\"username\":\"" + escapeJson(username) +
                  "\",\"message\":\"" + escapeJson(message) +
                  "\",\"timestamp\":" + time.now().toString() + "}";

        // Store in list for history
        this.client.lpush("chat:" + this.roomId + ":messages", msg);
        // Keep only last 100 messages
        this.client.lrange("chat:" + this.roomId + ":messages", 0, 99);

        // Publish for real-time delivery
        this.client.publish("chat:" + this.roomId, msg);
    }

    pub fn getHistory(count: int) array<string> {
        return this.client.lrange("chat:" + this.roomId + ":messages", 0, count - 1);
    }

    pub fn close() {
        this.client.close();
    }
}

// ============================================================================
// JSON Helper
// ============================================================================

fn escapeJson(str: string) string {
    return str.replace("\\", "\\\\")
              .replace("\"", "\\\"")
              .replace("\n", "\\n")
              .replace("\r", "\\r")
              .replace("\t", "\\t");
}

// ============================================================================
// Global State
// ============================================================================

var metrics: Metrics;
var chatRoom: ChatRoom;

// ============================================================================
// Dashboard Templates
// ============================================================================

fn renderDashboard() string {
    val pageViews = metrics.getPageViews();
    val apiCalls = metrics.getApiCalls();
    val errors = metrics.getErrors();
    val avgResponse = metrics.getAverageResponseTime();
    val activeUsers = metrics.getActiveUserCount();

    val ctx = template.context()
        .setInt("pageViews", pageViews)
        .setInt("apiCalls", apiCalls)
        .setInt("errors", errors)
        .setDouble("avgResponse", avgResponse)
        .setInt("activeUsers", activeUsers);

    val html = template.render("
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset=\"UTF-8\">
            <title>Real-time Dashboard</title>
            <style>
                body { font-family: system-ui, sans-serif; margin: 0; padding: 20px; background: #1a1a2e; color: #eee; }
                h1 { color: #00d4ff; }
                .dashboard { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; margin-bottom: 30px; }
                .card { background: #16213e; padding: 20px; border-radius: 10px; box-shadow: 0 4px 6px rgba(0,0,0,0.3); }
                .card h3 { margin: 0 0 10px; color: #888; font-size: 0.9em; text-transform: uppercase; }
                .card .value { font-size: 2.5em; font-weight: bold; color: #00d4ff; }
                .card.error .value { color: #ff6b6b; }
                .events { background: #16213e; padding: 20px; border-radius: 10px; margin-bottom: 20px; }
                .events h2 { margin-top: 0; color: #00d4ff; }
                .event-list { max-height: 300px; overflow-y: auto; font-family: monospace; font-size: 0.9em; }
                .event { padding: 5px 0; border-bottom: 1px solid #333; }
                .event.pageview { border-left: 3px solid #4caf50; padding-left: 10px; }
                .event.api { border-left: 3px solid #2196f3; padding-left: 10px; }
                .event.error { border-left: 3px solid #ff6b6b; padding-left: 10px; }
                .status { display: inline-block; width: 10px; height: 10px; border-radius: 50%; margin-right: 5px; }
                .status.connected { background: #4caf50; }
                .status.disconnected { background: #ff6b6b; }
                nav { margin-bottom: 20px; }
                nav a { color: #00d4ff; margin-right: 15px; }
            </style>
        </head>
        <body>
            <nav>
                <a href=\"/\">Dashboard</a>
                <a href=\"/chat\">Chat Room</a>
                <a href=\"/simulate\">Simulate Traffic</a>
            </nav>

            <h1><span class=\"status\" id=\"status\"></span> Real-time Dashboard</h1>

            <div class=\"dashboard\">
                <div class=\"card\">
                    <h3>Page Views</h3>
                    <div class=\"value\" id=\"pageViews\">{{ pageViews }}</div>
                </div>
                <div class=\"card\">
                    <h3>API Calls</h3>
                    <div class=\"value\" id=\"apiCalls\">{{ apiCalls }}</div>
                </div>
                <div class=\"card error\">
                    <h3>Errors</h3>
                    <div class=\"value\" id=\"errors\">{{ errors }}</div>
                </div>
                <div class=\"card\">
                    <h3>Avg Response (ms)</h3>
                    <div class=\"value\" id=\"avgResponse\">{{ avgResponse }}</div>
                </div>
                <div class=\"card\">
                    <h3>Active Users</h3>
                    <div class=\"value\" id=\"activeUsers\">{{ activeUsers }}</div>
                </div>
            </div>

            <div class=\"events\">
                <h2>Live Events</h2>
                <div class=\"event-list\" id=\"eventList\">
                    <div class=\"event\">Waiting for events...</div>
                </div>
            </div>

            <script>
                // Server-Sent Events connection
                const evtSource = new EventSource('/events');
                const statusEl = document.getElementById('status');
                const eventList = document.getElementById('eventList');

                evtSource.onopen = () => {
                    statusEl.className = 'status connected';
                };

                evtSource.onerror = () => {
                    statusEl.className = 'status disconnected';
                };

                evtSource.onmessage = (e) => {
                    const data = JSON.parse(e.data);

                    // Update metrics
                    if (data.pageViews !== undefined) {
                        document.getElementById('pageViews').textContent = data.pageViews;
                    }
                    if (data.apiCalls !== undefined) {
                        document.getElementById('apiCalls').textContent = data.apiCalls;
                    }
                    if (data.errors !== undefined) {
                        document.getElementById('errors').textContent = data.errors;
                    }
                    if (data.avgResponse !== undefined) {
                        document.getElementById('avgResponse').textContent = data.avgResponse.toFixed(2);
                    }
                    if (data.activeUsers !== undefined) {
                        document.getElementById('activeUsers').textContent = data.activeUsers;
                    }

                    // Add event to list
                    if (data.event) {
                        const eventDiv = document.createElement('div');
                        eventDiv.className = 'event ' + data.event.type;
                        eventDiv.textContent = new Date().toLocaleTimeString() + ' - ' + data.event.type + ': ' + JSON.stringify(data.event.data);
                        eventList.insertBefore(eventDiv, eventList.firstChild);

                        // Keep only 50 events
                        while (eventList.children.length > 50) {
                            eventList.removeChild(eventList.lastChild);
                        }
                    }
                };

                statusEl.className = 'status connected';
            </script>
        </body>
        </html>
    ", ctx);

    ctx.free();
    return html;
}

fn renderChatPage() string {
    val history = chatRoom.getHistory(50);

    var messagesHtml = "";
    for msg in history {
        messagesHtml = messagesHtml + "<div class=\"message\">" + template.escapeHtml(msg) + "</div>";
    }

    val ctx = template.context()
        .set("messages", messagesHtml);

    val html = template.render("
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset=\"UTF-8\">
            <title>Chat Room</title>
            <style>
                body { font-family: system-ui, sans-serif; margin: 0; padding: 20px; background: #1a1a2e; color: #eee; }
                h1 { color: #00d4ff; }
                nav { margin-bottom: 20px; }
                nav a { color: #00d4ff; margin-right: 15px; }
                .chat-container { max-width: 600px; margin: 0 auto; }
                .messages { background: #16213e; border-radius: 10px; padding: 20px; height: 400px; overflow-y: auto; margin-bottom: 20px; }
                .message { padding: 10px; margin-bottom: 10px; background: #0f3460; border-radius: 5px; }
                .message .username { font-weight: bold; color: #00d4ff; }
                .message .time { color: #666; font-size: 0.8em; }
                .input-area { display: flex; gap: 10px; }
                input { flex: 1; padding: 10px; border: none; border-radius: 5px; background: #16213e; color: #eee; }
                button { padding: 10px 20px; background: #00d4ff; border: none; border-radius: 5px; cursor: pointer; }
                button:hover { background: #00b8d4; }
            </style>
        </head>
        <body>
            <nav>
                <a href=\"/\">Dashboard</a>
                <a href=\"/chat\">Chat Room</a>
                <a href=\"/simulate\">Simulate Traffic</a>
            </nav>

            <div class=\"chat-container\">
                <h1>Chat Room</h1>

                <div class=\"messages\" id=\"messages\">
                    {{{ messages }}}
                </div>

                <div class=\"input-area\">
                    <input type=\"text\" id=\"username\" placeholder=\"Username\" value=\"User\" style=\"max-width: 150px;\">
                    <input type=\"text\" id=\"message\" placeholder=\"Type a message...\">
                    <button onclick=\"sendMessage()\">Send</button>
                </div>
            </div>

            <script>
                const messagesEl = document.getElementById('messages');
                const usernameEl = document.getElementById('username');
                const messageEl = document.getElementById('message');

                // SSE for real-time messages
                const evtSource = new EventSource('/chat/events');

                evtSource.onmessage = (e) => {
                    const msg = JSON.parse(e.data);
                    if (msg.message) {
                        const div = document.createElement('div');
                        div.className = 'message';
                        div.innerHTML = '<span class=\"username\">' + msg.username + '</span> ' +
                                       '<span class=\"time\">(' + new Date(msg.timestamp).toLocaleTimeString() + ')</span><br>' +
                                       msg.message;
                        messagesEl.appendChild(div);
                        messagesEl.scrollTop = messagesEl.scrollHeight;
                    }
                };

                function sendMessage() {
                    const username = usernameEl.value || 'Anonymous';
                    const message = messageEl.value;
                    if (!message) return;

                    fetch('/chat/send', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ username, message })
                    });

                    messageEl.value = '';
                }

                messageEl.addEventListener('keypress', (e) => {
                    if (e.key === 'Enter') sendMessage();
                });
            </script>
        </body>
        </html>
    ", ctx);

    ctx.free();
    return html;
}

fn renderSimulatePage() string {
    return "
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset=\"UTF-8\">
            <title>Traffic Simulator</title>
            <style>
                body { font-family: system-ui, sans-serif; margin: 0; padding: 20px; background: #1a1a2e; color: #eee; }
                h1 { color: #00d4ff; }
                nav { margin-bottom: 20px; }
                nav a { color: #00d4ff; margin-right: 15px; }
                .controls { max-width: 400px; }
                button { padding: 10px 20px; margin: 5px; background: #00d4ff; border: none; border-radius: 5px; cursor: pointer; }
                button:hover { background: #00b8d4; }
                button.error { background: #ff6b6b; }
                button.error:hover { background: #ff5252; }
                .log { background: #16213e; padding: 20px; border-radius: 10px; margin-top: 20px; font-family: monospace; max-height: 300px; overflow-y: auto; }
            </style>
        </head>
        <body>
            <nav>
                <a href=\"/\">Dashboard</a>
                <a href=\"/chat\">Chat Room</a>
                <a href=\"/simulate\">Simulate Traffic</a>
            </nav>

            <h1>Traffic Simulator</h1>

            <div class=\"controls\">
                <button onclick=\"simulate('pageview')\">Simulate Page View</button>
                <button onclick=\"simulate('api')\">Simulate API Call</button>
                <button onclick=\"simulate('error')\" class=\"error\">Simulate Error</button>
                <button onclick=\"simulate('burst')\">Burst (10 events)</button>
                <button onclick=\"reset()\">Reset Metrics</button>
            </div>

            <div class=\"log\" id=\"log\">
                Click buttons to simulate events...
            </div>

            <script>
                const logEl = document.getElementById('log');

                function log(msg) {
                    logEl.innerHTML = new Date().toLocaleTimeString() + ' - ' + msg + '<br>' + logEl.innerHTML;
                }

                async function simulate(type) {
                    try {
                        const res = await fetch('/api/simulate/' + type, { method: 'POST' });
                        const data = await res.json();
                        log('Simulated ' + type + ': ' + JSON.stringify(data));
                    } catch (e) {
                        log('Error: ' + e.message);
                    }
                }

                async function reset() {
                    try {
                        await fetch('/api/reset', { method: 'POST' });
                        log('Metrics reset');
                    } catch (e) {
                        log('Error: ' + e.message);
                    }
                }
            </script>
        </body>
        </html>
    ";
}

// ============================================================================
// Handlers
// ============================================================================

fn handleDashboard(req: http.Request, res: http.Response) {
    metrics.incrementPageViews();
    metrics.addActiveUser(req.header("X-Forwarded-For"));

    res.header("Content-Type", "text/html");
    res.send(renderDashboard());
}

fn handleChat(req: http.Request, res: http.Response) {
    metrics.incrementPageViews();

    res.header("Content-Type", "text/html");
    res.send(renderChatPage());
}

fn handleSimulate(req: http.Request, res: http.Response) {
    res.header("Content-Type", "text/html");
    res.send(renderSimulatePage());
}

// Server-Sent Events endpoint for dashboard
fn handleEvents(req: http.Request, res: http.Response) {
    // Set SSE headers
    res.header("Content-Type", "text/event-stream");
    res.header("Cache-Control", "no-cache");
    res.header("Connection", "keep-alive");

    // Send current metrics as initial data
    val data = "{" +
        "\"pageViews\":" + metrics.getPageViews().toString() + "," +
        "\"apiCalls\":" + metrics.getApiCalls().toString() + "," +
        "\"errors\":" + metrics.getErrors().toString() + "," +
        "\"avgResponse\":" + metrics.getAverageResponseTime().toString() + "," +
        "\"activeUsers\":" + metrics.getActiveUserCount().toString() +
    "}";

    res.send("data: " + data + "\n\n");
}

// SSE for chat messages
fn handleChatEvents(req: http.Request, res: http.Response) {
    res.header("Content-Type", "text/event-stream");
    res.header("Cache-Control", "no-cache");
    res.header("Connection", "keep-alive");

    // Send last messages as initial data
    val history = chatRoom.getHistory(10);
    for msg in history {
        res.send("data: " + msg + "\n\n");
    }
}

fn handleChatSend(req: http.Request, res: http.Response) {
    val data = json.decode(req.body());
    val username = data["username"] ?? "Anonymous";
    val message = data["message"] ?? "";

    if message != "" {
        chatRoom.sendMessage(username, message);
    }

    res.header("Content-Type", "application/json");
    res.send("{\"success\":true}");
}

// Simulation endpoints
fn handleSimulatePageview(req: http.Request, res: http.Response) {
    val count = metrics.incrementPageViews();
    metrics.recordResponseTime((50 + (count % 100)).toInt());
    metrics.publishEvent("pageview", "{\"path\":\"/simulated\"}");

    res.header("Content-Type", "application/json");
    res.send("{\"pageViews\":" + count.toString() + "}");
}

fn handleSimulateApi(req: http.Request, res: http.Response) {
    val count = metrics.incrementApiCalls();
    metrics.recordResponseTime((30 + (count % 50)).toInt());
    metrics.publishEvent("api", "{\"endpoint\":\"/api/simulated\"}");

    res.header("Content-Type", "application/json");
    res.send("{\"apiCalls\":" + count.toString() + "}");
}

fn handleSimulateError(req: http.Request, res: http.Response) {
    val count = metrics.incrementErrors();
    metrics.publishEvent("error", "{\"message\":\"Simulated error\"}");

    res.header("Content-Type", "application/json");
    res.send("{\"errors\":" + count.toString() + "}");
}

fn handleSimulateBurst(req: http.Request, res: http.Response) {
    for i in 1..10 {
        metrics.incrementPageViews();
        metrics.incrementApiCalls();
        metrics.recordResponseTime(50 + i * 5);
    }

    res.header("Content-Type", "application/json");
    res.send("{\"pageViews\":" + metrics.getPageViews().toString() +
             ",\"apiCalls\":" + metrics.getApiCalls().toString() + "}");
}

fn handleReset(req: http.Request, res: http.Response) {
    metrics.reset();

    res.header("Content-Type", "application/json");
    res.send("{\"success\":true}");
}

// ============================================================================
// Main
// ============================================================================

fn main() {
    log.info("Starting Real-time Application...");

    // Initialize services
    metrics = Metrics(REDIS_HOST, REDIS_PORT);
    chatRoom = ChatRoom(REDIS_HOST, REDIS_PORT, "general");

    // Create router
    val router = http.newRouter();

    // Dashboard routes
    router.get("/", handleDashboard);
    router.get("/events", handleEvents);

    // Chat routes
    router.get("/chat", handleChat);
    router.get("/chat/events", handleChatEvents);
    router.post("/chat/send", handleChatSend);

    // Simulation routes
    router.get("/simulate", handleSimulate);
    router.post("/api/simulate/pageview", handleSimulatePageview);
    router.post("/api/simulate/api", handleSimulateApi);
    router.post("/api/simulate/error", handleSimulateError);
    router.post("/api/simulate/burst", handleSimulateBurst);
    router.post("/api/reset", handleReset);

    // Create and start server
    val server = http.newServer(router);

    log.info("Real-time App listening on port " + SERVER_PORT.toString());
    log.info("Visit http://localhost:" + SERVER_PORT.toString());

    server.listen(SERVER_PORT);

    // Cleanup
    metrics.close();
    chatRoom.close();
}
