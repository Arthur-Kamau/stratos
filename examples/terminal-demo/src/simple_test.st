package main;

use terminal;
use convert;

fn main() {
    // Test basic terminal functions
    println("Testing Stratos Terminal Module");
    println("================================");
    println("");

    // Test terminal detection
    print("Is TTY: ");
    if terminal.isTTY() {
        println("Yes");
    } else {
        println("No");
    }

    // Test terminal size
    print("Terminal Width: ");
    println(convert.toString(terminal.getWidth()));

    print("Terminal Height: ");
    println(convert.toString(terminal.getHeight()));

    // Test color support
    print("Supports Color: ");
    if terminal.supportsColor() {
        println("Yes");
    } else {
        println("No");
    }

    println("");
    println("Testing Colors:");
    println("---------------");

    // Test basic colors
    terminal.setForeground(1); // Red
    print("Red ");
    terminal.setForeground(2); // Green
    print("Green ");
    terminal.setForeground(3); // Yellow
    print("Yellow ");
    terminal.setForeground(4); // Blue
    print("Blue ");
    terminal.setForeground(5); // Magenta
    print("Magenta ");
    terminal.setForeground(6); // Cyan
    print("Cyan ");
    terminal.reset();
    println("");

    println("");
    println("Testing Styles:");
    println("---------------");

    terminal.setStyle(1); // Bold
    print("Bold ");
    terminal.reset();

    terminal.setStyle(3); // Italic
    print("Italic ");
    terminal.reset();

    terminal.setStyle(4); // Underline
    print("Underline");
    terminal.reset();
    println("");

    println("");
    println("Testing Cursor Movement:");
    println("------------------------");

    terminal.moveCursor(20, 10);
    print("Text at row 20, col 10");

    terminal.moveCursor(21, 1);
    println("");
    println("All tests completed!");
}
