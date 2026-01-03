package main;

use terminal;
use convert;
use strings;

fn main() {
    if !terminal.isTTY() {
        println("This program requires a terminal to run!");
        return;
    }

    terminal.clear();

    // Draw title
    terminal.moveCursor(2, 10);
    terminal.setForeground(6); // Cyan
    terminal.setStyle(1); // Bold
    print("NUMBER GUESSING GAME");
    terminal.reset();

    terminal.moveCursor(4, 10);
    terminal.setForeground(7); // White
    print("I'm thinking of a number between 1 and 100");
    terminal.reset();

    // Simple number generation (not truly random, but for demo)
    val secretNumber = 42;  // In a real game, this would be random

    var attempts = 0;
    var won = false;

    while !won && attempts < 10 {
        terminal.moveCursor(7 + attempts, 5);

        // Clear previous message if any
        if attempts > 0 {
            terminal.moveCursor(6, 5);
            print("                                                    ");
        }

        // Get user input
        terminal.moveCursor(7 + attempts, 5);
        terminal.setForeground(2); // Green
        print("Guess #");
        print(convert.toString(attempts + 1));
        print(": ");
        terminal.reset();

        val guessStr = terminal.readLine();
        val guess = convert.toInt(guessStr);

        attempts = attempts + 1;

        // Check guess
        terminal.moveCursor(6, 5);

        if guess == secretNumber {
            won = true;
            terminal.setForeground(2); // Green
            terminal.setStyle(1); // Bold
            print("🎉 Correct! You won in ");
            print(convert.toString(attempts));
            print(" attempts!");
            terminal.reset();
        } else if guess < secretNumber {
            terminal.setForeground(3); // Yellow
            print("Too low! Try again...");
            terminal.reset();
        } else {
            terminal.setForeground(3); // Yellow
            print("Too high! Try again...");
            terminal.reset();
        }
    }

    if !won {
        terminal.moveCursor(18, 5);
        terminal.setForeground(1); // Red
        terminal.setStyle(1); // Bold
        print("Game Over! The number was ");
        print(convert.toString(secretNumber));
        terminal.reset();
    }

    // Ask to play again
    terminal.moveCursor(20, 5);
    terminal.setForeground(8); // Gray
    print("Press Enter to exit...");
    terminal.reset();

    terminal.readLine();

    terminal.clear();
}
