package main;

fn main() {
    // Demonstrate println - prints with newline
    println("=== Stratos Prelude Functions Demo ===");
    println("");

    // Demonstrate print - prints without newline
    print("Building a sentence: ");
    print("word1 ");
    print("word2 ");
    println("word3");
    println("");

    // Demonstrate printf - formatted printing
    println("=== printf Examples ===");

    val name = "Alice";
    val age = 25;
    val score = 95.5;

    printf("Student: {}\n", name);
    printf("Age: {}\n", age);
    printf("Score: {}\n", score);
    printf("Summary: {} is {} years old with a score of {}\n", name, age, score);
    println("");

    // Math example with printf
    println("=== Math with printf ===");
    val a = 10;
    val b = 5;

    printf("{} + {} = {}\n", a, b, a + b);
    printf("{} - {} = {}\n", a, b, a - b);
    printf("{} * {} = {}\n", a, b, a * b);
    println("");

    // Boolean and special types
    println("=== Different Types ===");
    printf("Boolean: {}\n", true);
    printf("Boolean: {}\n", false);
    printf("Character: {}\n", 'X');
    printf("String: {}\n", "Hello, Stratos!");
    println("");

    // Progress bar example using print
    println("=== Progress Bar Demo ===");
    print("Loading: [");

    var i = 0;
    while (i < 10) {
        print("#");
        i = i + 1;
    }

    println("] Done!");
}
