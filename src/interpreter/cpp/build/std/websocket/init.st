package websocket;

// ============================================================================
// CLIENT FUNCTIONS
// ============================================================================

// Connect to a WebSocket server
// Usage: val ws = websocket.connect("ws://localhost:8080/chat")
// Returns: WebSocket connection ID (int), -1 on error
fn connect(url: string) int;

// Send a text message to the WebSocket server
// Usage: websocket.send(ws, "Hello, World!")
fn send(wsId: int, message: string) void;

// Receive a message from the WebSocket server (blocking with timeout)
// Usage: val message = websocket.receive(ws)
// Returns: Message string (empty if timeout/error)
fn receive(wsId: int) string;

// Close a WebSocket connection
// Usage: websocket.close(ws)
fn close(wsId: int) void;

// Check if a WebSocket connection is active
// Usage: val connected = websocket.isConnected(ws)
// Returns: true if connected, false otherwise
fn isConnected(wsId: int) bool;

// ============================================================================
// SERVER FUNCTIONS
// ============================================================================

// Create a WebSocket server listening on the specified port
// Usage: val server = websocket.createServer(8080)
// Returns: Server ID (int), -1 on error
fn createServer(port: int) int;

// Accept an incoming client connection (non-blocking)
// Usage: val client = websocket.acceptClient(server)
// Returns: Client ID (int), -1 if no connection or timeout
fn acceptClient(serverId: int) int;

// Send a message to a specific client
// Usage: websocket.sendToClient(client, "Hello, Client!")
fn sendToClient(clientId: int, message: string) void;

// Receive a message from a specific client (blocking with timeout)
// Usage: val message = websocket.receiveFromClient(client)
// Returns: Message string (empty if timeout/error)
fn receiveFromClient(clientId: int) string;

// Close a client connection
// Usage: websocket.closeClient(client)
fn closeClient(clientId: int) void;

// Close the WebSocket server
// Usage: websocket.closeServer(server)
fn closeServer(serverId: int) void;

// Check if a client is connected
// Usage: val connected = websocket.isClientConnected(client)
// Returns: true if connected, false otherwise
fn isClientConnected(clientId: int) bool;
