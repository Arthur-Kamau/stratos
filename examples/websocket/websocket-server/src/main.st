// WebSocket Server Example
// This example demonstrates how to create a WebSocket server in Stratos,
// accept client connections, and handle bidirectional communication.

use websocket;

fn main() {
    println("=== Stratos WebSocket Server ===");
    println("");

    // Create server on port 8080
    println("Starting WebSocket server on port 8080...");
    val server: int = websocket.createServer(8080);

    if (server == -1) {
        println("Failed to create WebSocket server");
        println("Make sure port 8080 is available");
        return;
    }

    println("✓ Server started successfully!");
    println("Listening on ws://localhost:8080");
    println("");
    println("Waiting for first client...");
    println("");

    // Accept one client (poll with retry)
    val client: int = -1;
    val retries: int = 0;
    val maxRetries: int = 20;  // Wait up to ~10 seconds (20 * 0.5s)

    while (client == -1) {
        client = websocket.acceptClient(server);
        if (client == -1) {
            retries = retries + 1;
            if (retries >= maxRetries) {
                println("No client connected after " + retries + " attempts");
                websocket.closeServer(server);
                return;
            }
            // Small delay before retry (simulated with loop - not ideal but works)
            val i: int = 0;
            while (i < 100000) {
                i = i + 1;
            }
        }
    }

    println("✓ Client connected!");
    println("");

    // Send welcome message
    println("Sending welcome message...");
    websocket.sendToClient(client, "Welcome to Stratos WebSocket Server!");
    println("✓ Sent welcome message");
    println("");

    // Receive and echo messages
    val messageCount: int = 0;
    val maxMessages: int = 5;

    while (messageCount < maxMessages) {
        println("Waiting for message " + "(timeout: 5s)...");
        val message: string = websocket.receiveFromClient(client);

        if (message == "") {
            println("No message received (timeout)");
            val connected: int = websocket.isClientConnected(client);
            if (connected == 0) {
                println("Client disconnected");
                messageCount = maxMessages;
            }
        } else {
            println("← Received: " + message);

            // Handle special messages
            if (message == "ping") {
                websocket.sendToClient(client, "pong");
                println("→ Sent: pong");
            } else if (message == "What is your name?") {
                websocket.sendToClient(client, "I am a Stratos WebSocket Server!");
                println("→ Sent: I am a Stratos WebSocket Server!");
            } else if (message == "bye") {
                websocket.sendToClient(client, "Goodbye!");
                println("→ Sent: Goodbye!");
                messageCount = maxMessages;
            } else {
                val response: string = "Echo: " + message;
                websocket.sendToClient(client, response);
                println("→ Sent: " + response);
            }

            messageCount = messageCount + 1;
        }
        println("");
    }

    // Cleanup
    println("Closing client connection...");
    websocket.closeClient(client);
    println("✓ Client closed");
    println("");

    println("Shutting down server...");
    websocket.closeServer(server);
    println("✓ Server closed");
    println("");
    println("=== Server Example Complete ===");
}
