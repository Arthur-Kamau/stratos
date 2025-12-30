# WebSocket Examples

Demonstrates WebSocket client-server communication in Stratos, including both single-shot and daemon modes.

## Structure

```
websocket/
├── websocket-server/
│   ├── src/main.st          # Single-client server
│   ├── src/server-daemon.st # Multi-client daemon server
│   └── stratos.conf
├── websocket-client/
│   ├── src/main.st            # Single-run client
│   ├── src/client-interactive.st  # Interactive client for daemon
│   └── stratos.conf
├── test-websocket.sh        # Test single-client mode
└── test-daemon.sh           # Test daemon mode
```

## Quick Start

### Test Single-Client Mode

Runs server and client once:

```bash
cd examples/websocket
./test-websocket.sh
```

### Test Daemon Mode

Runs server continuously with multiple sequential clients:

```bash
cd examples/websocket
./test-daemon.sh
```

## Manual Usage

### Single-Client Mode

**Terminal 1 - Server:**
```bash
cd websocket-server
../../../src/interpreter/cpp/build/stratos run
```

**Terminal 2 - Client:**
```bash
cd websocket-client
../../../src/interpreter/cpp/build/stratos run
```

### Daemon Mode (Continuous Server)

**Terminal 1 - Start Daemon:**
```bash
cd websocket-server
../../../src/interpreter/cpp/build/stratos run src/server-daemon.st
```

The daemon will run indefinitely, handling multiple clients sequentially.

**Terminal 2 - Connect Clients:**
```bash
cd websocket-client
../../../src/interpreter/cpp/build/stratos run src/client-interactive.st
```

Run this multiple times to connect different clients. The server will handle each one and remain running.

## Features Demonstrated

### Server Capabilities
- ✅ Creating WebSocket server on port 8080
- ✅ Accepting client connections
- ✅ Sending messages to clients
- ✅ Receiving messages from clients
- ✅ Command handling (ping, bye, etc.)
- ✅ Echo functionality
- ✅ Connection management
- ✅ **Daemon mode** - continuous operation with multiple clients

### Client Capabilities
- ✅ Connecting to WebSocket server
- ✅ Sending messages
- ✅ Receiving server responses
- ✅ Graceful disconnection

## Message Protocol

### Server Commands

The server recognizes these special messages:
- `ping` → Server responds with `pong`
- `What is your name?` → Server responds with identity
- `bye` → Server sends goodbye and closes connection
- Any other message → Server echoes back with "Echo: " prefix

### Communication Flow

1. Client connects to `ws://localhost:8080`
2. Server accepts connection
3. Server sends welcome message (timing-dependent in single mode)
4. Client sends test messages
5. Server processes and responds to each message
6. Client sends `bye` to disconnect
7. **Daemon mode**: Server continues waiting for next client

## Test Scripts

### test-websocket.sh

Tests single-client mode:
- Starts server in background
- Runs client to completion
- Displays both server and client output
- Verifies all communication steps
- 5/6 checks pass (welcome message has timing issue)

### test-daemon.sh

Tests daemon server mode:
- Starts daemon server
- Connects 3 sequential clients
- Verifies all clients were handled
- Confirms ping-pong and echo work
- All checks pass ✓

## Implementation Details

### TCP_NODELAY Optimization

The WebSocket implementation uses `TCP_NODELAY` to disable Nagle's algorithm, ensuring:
- Low-latency message delivery
- Immediate transmission without buffering delays
- Optimal performance for interactive communication

### Polling for Client Connections

The daemon server uses polling to accept clients:
```stratos
val client: int = -1;
while (client == -1) {
    client = websocket.acceptClient(server);
    // Small delay between polls
}
```

This allows continuous operation without blocking indefinitely.

## Known Issues & Solutions

### ~~Welcome Message Timing~~ (Partially Resolved)

**Issue**: In single-client mode, the initial welcome message may occasionally not be received due to race conditions between server send and client receive.

**Impact**: Non-critical - all subsequent messages work correctly.

**Solution**: Use daemon mode for production scenarios, or the welcome message can be sent as part of application-level handshake.

### Logical Operators

Stratos supports logical operators using both keyword and symbolic syntax:

```stratos
// ✓ Keyword syntax (recommended)
if (message == "quit" or message == "bye") { }

// ✓ Symbolic syntax (also supported)
if (message == "quit" || message == "bye") { }

// AND operator
if (age >= 18 and hasLicense) { }
if (age >= 18 && hasLicense) { }  // Also works
```

The keyword syntax (`and`, `or`, `not`) is recommended for better readability, following Kotlin's style.

## Requirements

- Stratos interpreter: `../../src/interpreter/cpp/build/stratos`
- WebSocket standard library: `std/websocket`
- Port 8080 must be available
- Linux/Unix environment (uses POSIX sockets)

## Building

If the Stratos interpreter isn't built yet:

```bash
cd ../../src/interpreter/cpp
./build.sh
```

## Version

- Stratos Interpreter v0.2.0-dev
- WebSocket Protocol: RFC 6455
- Platform: Linux (POSIX sockets)

## Use Cases

### Single-Client Mode
- Quick testing and demos
- Simple request-response patterns
- One-time data exchanges

### Daemon Mode
- Production servers
- Multiple sequential clients
- Long-running services
- Testing with multiple connections
- Development and debugging

## Example Output

### Daemon Server Handling Multiple Clients

```
=== Stratos WebSocket Server (Daemon Mode) ===

Starting WebSocket server on port 8080...
✓ Server started!
Listening on ws://localhost:8080
Press Ctrl+C to stop

[0] Waiting for client...

=== Client #1 ===
New client connected!
← Hello from Stratos!
→ Echo: Hello from Stratos!
← ping
→ pong
← bye
→ Goodbye!
Client session ended

[1] Waiting for client...

=== Client #2 ===
New client connected!
...
```

## Future Enhancements

Potential improvements:
- Concurrent client handling (requires threading)
- Broadcast messages to all connected clients
- Client authentication
- Binary message support
- Custom timeout configuration
- Ping/pong heartbeat mechanism
