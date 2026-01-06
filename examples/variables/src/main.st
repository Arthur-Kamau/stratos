package main;

fn main() {
    println("Stratos Variables Example");

    // 1. Immutable variables (val)
    // Variables defined with 'val' cannot be reassigned
    val greeting = "Hello, Stratos!";
    println(greeting);

    val pi = 3.14159;
    println("Value of pi: " + pi);

    // 2. Mutable variables (var)
    // Variables defined with 'var' can be updated
    var counter = 0;
    println("Counter starts at: " + counter);

    counter = counter + 1;
    println("Counter incremented: " + counter);
    
    counter = 10;
    println("Counter set to: " + counter);

    // 3. Different types
    val number = 42;          // int
    val text = "Text string"; // string
    val flag = true;          // bool
    
    println("Number: " + number);
    println("Text: " + text);
    
    if flag {
        println("Flag is true");
    }

    // 4. Variable logic
    var total = number + 8;
    println("Total (42 + 8): " + total);
}
