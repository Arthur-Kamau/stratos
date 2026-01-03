package main;

use terminal;

fn main() {
    println("Simple Input Demo");
    println("=================");
    println("");

    print("Enter your name: ");
    val name = terminal.readLine();

    print("Hello, ");
    print(name);
    println("!");
}
