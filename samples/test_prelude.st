package main;

fn main() {
    // Test println - should print with newline
    println("Hello from println!");
    println(42);
    println(3.14);
    println(true);

    // Test print - should print without newline
    print("This is ");
    print("on the ");
    print("same line");
    println("");  // Empty println for newline

    // Test printf - formatted printing
    printf("Name: {}, Age: {}, Score: {}\n", "Alice", 25, 95.5);
    printf("Boolean: {}, Char: {}\n", false, 'X');

    // Test with variables
    val name = "Bob";
    val age = 30;
    printf("Hello {}, you are {} years old.\n", name, age);
}
