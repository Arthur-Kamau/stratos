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

    // Accept one client
    val client: int = websocket.acceptClient(server, 10000);

    if (client == -1) {
        println("No client connected within 10 seconds");
        websocket.closeServer(server);
        return;
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
