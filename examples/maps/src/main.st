package main;

fn printHeader(title: string) void {
    println("\n" + "=".repeat(50));
    println(title);
    println("=".repeat(50));
}

fn main() {
    printHeader("Stratos Map Demo");

    // Creating maps with string values
    println("\n1. Creating Maps");
    println("-".repeat(40));

    val config: map<string, int> = {"port": 8080, "timeout": 30, "retries": 3};
    println("Created config map");

    val scores: map<string, int> = {"alice": 95, "bob": 87, "charlie": 92};
    println("Created scores map");

    // String-keyed maps
    println("\n2. String-keyed map");
    println("-".repeat(40));
    val headers = {content_type: "application/json", accept: "text/html"};
    println("Created headers map");

    // Numeric maps
    println("\n3. Numeric values");
    println("-".repeat(40));
    val sizes = {small: 10, medium: 20, large: 30};
    println("Created sizes map");

    printHeader("Map Demo Complete");
    println("\nMaps created successfully!");
    println("Features: literal syntax { key: value }");
    println("=".repeat(50));
}
