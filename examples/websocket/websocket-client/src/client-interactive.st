// WebSocket Client - Interactive Mode
// Can connect to a running server and send custom messages

use websocket;

fn main() {
    println("=== Stratos WebSocket Client (Interactive) ===");
    println("");

    // Connect
    println("Connecting to ws://localhost:8080...");
    val ws: int = websocket.connect("ws://localhost:8080");

    if ws == -1 {
        println("Failed to connect");
        println("Make sure the server is running:");
        println("  cd ../websocket-server");
        println("  stratos run src/server-daemon.st");
        return;
    }

    println("✓ Connected!");
    println("");

    // Receive welcome message
    val welcome: string = websocket.receive(ws);
    if welcome != "" {
        println("Server: " + welcome);
        println("");
    }

    // Send test messages
    println("Sending test messages...");
    println("");

    // Test 1: Hello
    println("[1/4] → Hello from Stratos!");
    websocket.send(ws, "Hello from Stratos!");
    val r1: string = websocket.receive(ws);
    println("      ← " + r1);
    println("");

    // Test 2: Ping
    println("[2/4] → ping");
    websocket.send(ws, "ping");
    val r2: string = websocket.receive(ws);
    println("      ← " + r2);
    println("");

    // Test 3: Question
    println("[3/4] → What is your name?");
    websocket.send(ws, "What is your name?");
    val r3: string = websocket.receive(ws);
    println("      ← " + r3);
    println("");

    // Test 4: Goodbye
    println("[4/4] → bye");
    websocket.send(ws, "bye");
    val r4: string = websocket.receive(ws);
    println("      ← " + r4);
    println("");

    // Close
    websocket.close(ws);
    println("✓ Disconnected");
    println("");
    println("=== Session Complete ===");
}
