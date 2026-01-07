package main;

fn main() {
    val integer: int = 42;
    val pi: double = 3.14159;
    val text: string = "Stratos";
    val isAwesome: bool = true;

    // Type casting
    val rounded = pi as int;

    println(text + " is awesome: " + isAwesome);
    println("Rounded PI: " + rounded);
}