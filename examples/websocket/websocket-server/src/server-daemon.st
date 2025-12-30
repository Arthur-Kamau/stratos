// WebSocket Server Daemon
// This server runs continuously, accepting multiple clients

use websocket;

fn handleClient(client: int) {
    println("New client connected!");

    // Send welcome message
    websocket.sendToClient(client, "Welcome to Stratos WebSocket Server!");

    // Handle messages
    val active: int = 1;
    while (active == 1) {
        val message: string = websocket.receiveFromClient(client);

        if (message == "") {
            // Timeout or disconnection
            val connected: int = websocket.isClientConnected(client);
            if (connected == 0) {
                println("Client disconnected");
                active = 0;
            }
        } else {
            println("← " + message);

            // Handle commands
            if (message == "ping") {
                websocket.sendToClient(client, "pong");
                println("→ pong");
            } else if (message == "bye" or message == "quit") {
                websocket.sendToClient(client, "Goodbye!");
                println("→ Goodbye!");
                active = 0;
            } else if (message == "What is your name?") {
                websocket.sendToClient(client, "I am a Stratos WebSocket Server!");
                println("→ I am a Stratos WebSocket Server!");
            } else {
                val response: string = "Echo: " + message;
                websocket.sendToClient(client, response);
                println("→ " + response);
            }
        }
    }

    // Cleanup
    websocket.closeClient(client);
    println("Client session ended");
    println("");
}

fn main() {
    println("=== Stratos WebSocket Server (Daemon Mode) ===");
    println("");

    // Create server
    println("Starting WebSocket server on port 8080...");
    val server: int = websocket.createServer(8080);

    if (server == -1) {
        println("Failed to create WebSocket server");
        return;
    }

    println("✓ Server started!");
    println("Listening on ws://localhost:8080");
    println("Press Ctrl+C to stop");
    println("");

    // Accept clients in a loop
    val running: int = 1;
    val clientCount: int = 0;

    while (running == 1) {
        println("[" + clientCount + "] Waiting for client...");

        val client: int = -1;
        val retries: int = 0;

        // Poll for client connection
        while (client == -1) {
            client = websocket.acceptClient(server);
            if (client == -1) {
                retries = retries + 1;
                // Small delay
                val i: int = 0;
                while (i < 100000) {
                    i = i + 1;
                }
            }
        }

        clientCount = clientCount + 1;
        println("");
        println("=== Client #" + clientCount + " ===");

        // Handle the client
        handleClient(client);
    }

    // Cleanup (never reached in daemon mode, but good practice)
    websocket.closeServer(server);
    println("Server stopped");
}
