package main;

use greeting;

fn main() {
    println("Testing pure Stratos standard library implementation...");

    val message1 = greeting.greet();
    println(message1);

    val message2 = greeting.hello("Alice");
    println(message2);

    val message3 = greeting.goodbye("Bob");
    println(message3);

    println("Success! Pure Stratos std library modules work!");
}
