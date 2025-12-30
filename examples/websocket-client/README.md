# WebSocket Client Example

This example demonstrates how to use the WebSocket module in Stratos to connect to a WebSocket server, send messages, and receive responses.

## Features Demonstrated

- Connecting to a WebSocket server
- Checking connection status with `isConnected()`
- Sending text messages with `send()`
- Receiving messages with `receive()` (with timeout)
- Closing WebSocket connections with `close()`

## Prerequisites

1. **OpenSSL** - Required for WebSocket handshake
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libssl-dev

   # macOS
   brew install openssl
   ```

2. **Python 3** - For the test server
   ```bash
   pip install websockets
   ```

## Running the Example

### Step 1: Start the WebSocket Server

In one terminal:
```bash
cd examples/websocket-client
python3 server.py
```

You should see:
```
==================================================
WebSocket Echo Server
==================================================

Server started on ws://localhost:8080
Waiting for connections...

Press Ctrl+C to stop
```

### Step 2: Run the Stratos Client

In another terminal:
```bash
cd examples/websocket-client
../../src/interpreter/cpp/build/stratos run src/main.st
```

## Expected Output

**Client output:**
```
=== Stratos WebSocket Client Example ===

Connecting to ws://localhost:8080...
✓ Connected successfully!

Connection status: CONNECTED

Sending message: 'Hello from Stratos!'

Waiting for response...
Received: Echo: Hello from Stratos!

Sending message: 'What is your name?'

Waiting for response...
Received: I am a Python WebSocket Server!

Closing WebSocket connection...
✓ Connection closed

=== Example Complete ===
```

**Server output:**
```
✓ New client connected from ('127.0.0.1', 54321)
Received: Hello from Stratos!
Sending: Echo: Hello from Stratos!
Received: What is your name?
Sending: I am a Python WebSocket Server!
✗ Client disconnected
```

## Code Walkthrough

### 1. Import the WebSocket Module
```stratos
use websocket;
```

### 2. Connect to a Server
```stratos
val ws: int = websocket.connect("ws://localhost:8080");
if (ws == -1) {
    println("Failed to connect");
    return;
}
```

The `connect()` function returns a connection ID (integer) on success, or `-1` on error.

### 3. Send Messages
```stratos
websocket.send(ws, "Hello from Stratos!");
```

### 4. Receive Messages
```stratos
val response: string = websocket.receive(ws);
```

By default, `receive()` has a 5-second timeout. You can optionally specify a custom timeout:
```stratos
val response: string = websocket.receive(ws, 10000);  // 10 second timeout
```

If no message is received within the timeout, an empty string is returned.

### 5. Check Connection Status
```stratos
val connected: int = websocket.isConnected(ws);
if (connected == 1) {
    println("Still connected!");
}
```

### 6. Close the Connection
```stratos
websocket.close(ws);
```

## WebSocket API Reference

### `connect(url: string) -> int`
Connect to a WebSocket server.
- **Parameters:** WebSocket URL (e.g., "ws://localhost:8080" or "ws://example.com:9000/chat")
- **Returns:** Connection ID (integer) on success, -1 on error

### `send(wsId: int, message: string) -> void`
Send a text message to the WebSocket server.
- **Parameters:**
  - `wsId`: Connection ID from `connect()`
  - `message`: Text message to send

### `receive(wsId: int) -> string`
Receive a message from the WebSocket server (blocking with 5s timeout).
- **Parameters:** Connection ID from `connect()`
- **Returns:** Received message, or empty string on timeout/error

### `close(wsId: int) -> void`
Close a WebSocket connection.
- **Parameters:** Connection ID from `connect()`

### `isConnected(wsId: int) -> bool`
Check if a WebSocket connection is still active.
- **Parameters:** Connection ID from `connect()`
- **Returns:** `true` if connected, `false` otherwise

## Testing with Other Servers

You can test with any WebSocket server. Here are some public echo servers:

```stratos
// Echo server
val ws = websocket.connect("ws://echo.websocket.org");

// Different port
val ws = websocket.connect("ws://localhost:3000/api/chat");
```

## Troubleshooting

### "Failed to connect to WebSocket server"
- Make sure the server is running
- Check the URL and port number
- Verify there's no firewall blocking the connection

### "No response received (timeout or error)"
- Server might not be sending responses
- Check server logs for errors
- Try increasing the timeout: `websocket.receive(ws, 10000)`

### Build errors about missing OpenSSL
- Install OpenSSL development libraries:
  ```bash
  # Ubuntu/Debian
  sudo apt-get install libssl-dev

  # macOS
  brew install openssl
  ```

## Next Steps

- Try connecting to a real WebSocket service (chat server, real-time data feed, etc.)
- Build a chat client or real-time monitoring tool
- Implement ping/pong keep-alive messages
- Handle reconnection logic for dropped connections
