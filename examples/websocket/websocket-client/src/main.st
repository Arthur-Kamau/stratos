// WebSocket Client Example
// This example demonstrates how to connect to a WebSocket server,
// send messages, and receive responses.

use websocket;

fn main() {
    println("=== Stratos WebSocket Client ===");
    println("");

    // Connect to WebSocket server
    println("Connecting to ws://localhost:8080...");
    val ws: int = websocket.connect("ws://localhost:8080");

    if ws == -1 {
        println("Failed to connect to WebSocket server");
        println("Make sure the server is running:");
        println("  cd ../websocket-server");
        println("  ../../src/interpreter/cpp/build/stratos run src/main.st");
        return;
    }

    println("✓ Connected successfully!");
    println("");

    // Receive welcome message
    println("Waiting for welcome message...");
    val welcome: string = websocket.receive(ws);
    if welcome != "" {
        println("Received: " + welcome);
    }
    println("");

    // Send test messages
    println("Sending test messages...");
    println("");

    // Message 1: Simple greeting
    println("[1/5] Sending: Hello from Stratos!");
    websocket.send(ws, "Hello from Stratos!");
    val response1: string = websocket.receive(ws);
    println("      Received: " + response1);
    println("");

    // Message 2: Ping command
    println("[2/5] Sending: ping");
    websocket.send(ws, "ping");
    val response2: string = websocket.receive(ws);
    println("      Received: " + response2);
    println("");

    // Message 3: Question
    println("[3/5] Sending: What is your name?");
    websocket.send(ws, "What is your name?");
    val response3: string = websocket.receive(ws);
    println("      Received: " + response3);
    println("");

    // Message 4: Another message
    println("[4/5] Sending: This is amazing!");
    websocket.send(ws, "This is amazing!");
    val response4: string = websocket.receive(ws);
    println("      Received: " + response4);
    println("");

    // Message 5: Goodbye
    println("[5/5] Sending: bye");
    websocket.send(ws, "bye");
    val response5: string = websocket.receive(ws);
    println("      Received: " + response5);
    println("");

    // Close connection
    println("Closing connection...");
    websocket.close(ws);
    println("✓ Connection closed");
    println("");

    println("=== Client Example Complete ===");
}
