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

    // Title
    terminal.moveCursor(2, 5);
    terminal.setForeground(6); // Cyan
    terminal.setStyle(1); // Bold
    print("TERMINAL INPUT DEMO");
    terminal.reset();

    // Demonstrate different input methods
    demoBasicInput();
    demoRawKeyInput();
    demoMenuSelection();

    // Goodbye message
    terminal.clear();
    terminal.moveCursor(10, 5);
    terminal.setForeground(2); // Green
    terminal.setStyle(1); // Bold
    print("Thanks for trying the Terminal Input Demo!");
    terminal.reset();
    println("");
    println("");
}

fn demoBasicInput() void {
    terminal.clear();

    terminal.moveCursor(2, 5);
    terminal.setForeground(6); // Cyan
    terminal.setStyle(1); // Bold
    print("DEMO 1: Basic Text Input");
    terminal.reset();

    terminal.moveCursor(5, 5);
    println("This demo shows how to read text input from the user.");
    println("");

    // Get user's name
    terminal.moveCursor(8, 5);
    terminal.setForeground(2); // Green
    print("Enter your name: ");
    terminal.reset();
    val name = terminal.readLine();

    // Get user's age
    terminal.moveCursor(10, 5);
    terminal.setForeground(2); // Green
    print("Enter your age: ");
    terminal.reset();
    val ageStr = terminal.readLine();

    // Display results
    terminal.moveCursor(13, 5);
    terminal.setForeground(3); // Yellow
    terminal.setStyle(1); // Bold
    print("Results:");
    terminal.reset();

    terminal.moveCursor(15, 7);
    print("Name: ");
    terminal.setForeground(2); // Green
    print(name);
    terminal.reset();

    terminal.moveCursor(16, 7);
    print("Age: ");
    terminal.setForeground(2); // Green
    print(ageStr);
    terminal.reset();

    // Wait for confirmation
    terminal.moveCursor(19, 5);
    terminal.setForeground(8); // Gray
    print("Press Enter to continue...");
    terminal.reset();
    terminal.readLine();
}

fn demoRawKeyInput() void {
    terminal.clear();

    terminal.moveCursor(2, 5);
    terminal.setForeground(6); // Cyan
    terminal.setStyle(1); // Bold
    print("DEMO 2: Raw Key Input");
    terminal.reset();

    terminal.moveCursor(5, 5);
    println("Press any key to see its code. Press ESC to exit.");
    println("");

    terminal.moveCursor(8, 5);
    terminal.setForeground(3); // Yellow
    print("Key Code:");
    terminal.reset();

    terminal.moveCursor(9, 5);
    terminal.setForeground(3); // Yellow
    print("Key Name:");
    terminal.reset();

    var running = true;
    terminal.enableRawMode();

    while running {
        // Clear previous output
        terminal.moveCursor(8, 15);
        print("                    ");
        terminal.moveCursor(9, 15);
        print("                    ");

        // Read key
        val key = terminal.readKey();

        // Display key code
        terminal.moveCursor(8, 15);
        terminal.setForeground(2); // Green
        print(convert.toString(key));
        terminal.reset();

        // Display key name
        terminal.moveCursor(9, 15);
        terminal.setForeground(2); // Green

        if key == 1001 {
            print("Up Arrow");
        } else if key == 1002 {
            print("Down Arrow");
        } else if key == 1003 {
            print("Left Arrow");
        } else if key == 1004 {
            print("Right Arrow");
        } else if key == 13 {
            print("Enter");
        } else if key == 27 {
            print("ESC (Exiting...)");
            running = false;
        } else if key == 127 {
            print("Backspace");
        } else if key == 9 {
            print("Tab");
        } else if key == 32 {
            print("Space");
        } else if key >= 65 && key <= 90 {
            print("Letter (uppercase)");
        } else if key >= 97 && key <= 122 {
            print("Letter (lowercase)");
        } else if key >= 48 && key <= 57 {
            print("Digit");
        } else {
            print("Character");
        }

        terminal.reset();
    }

    terminal.disableRawMode();
}

fn demoMenuSelection() void {
    terminal.clear();

    terminal.moveCursor(2, 5);
    terminal.setForeground(6); // Cyan
    terminal.setStyle(1); // Bold
    print("DEMO 3: Interactive Menu");
    terminal.reset();

    terminal.moveCursor(5, 5);
    println("Use arrow keys to navigate, Enter to select:");
    println("");

    val menuItems = ["Start Game", "Load Game", "Settings", "Exit"];
    var selected = 0;
    var running = true;

    terminal.enableRawMode();

    while running {
        // Display menu
        var i = 0;
        while i < 4 {
            terminal.moveCursor(8 + i, 7);

            if i == selected {
                terminal.setStyle(7); // Reverse
                terminal.setForeground(6); // Cyan
            }

            if i == 0 {
                print("  Start Game    ");
            } else if i == 1 {
                print("  Load Game     ");
            } else if i == 2 {
                print("  Settings      ");
            } else if i == 3 {
                print("  Exit          ");
            }

            terminal.reset();
            i = i + 1;
        }

        // Instructions
        terminal.moveCursor(14, 5);
        terminal.setForeground(8); // Gray
        print("↑/↓ to navigate | Enter to select | ESC to cancel");
        terminal.reset();

        // Read key
        val key = terminal.readKey();

        if key == 1001 { // UP
            if selected > 0 {
                selected = selected - 1;
            }
        } else if key == 1002 { // DOWN
            if selected < 3 {
                selected = selected + 1;
            }
        } else if key == 13 { // ENTER
            running = false;
        } else if key == 27 { // ESC
            selected = -1;
            running = false;
        }
    }

    terminal.disableRawMode();

    // Show selection
    terminal.moveCursor(17, 5);

    if selected == -1 {
        terminal.setForeground(1); // Red
        print("Selection cancelled!");
    } else {
        terminal.setForeground(2); // Green
        terminal.setStyle(1); // Bold
        print("You selected: ");
        terminal.reset();
        terminal.setForeground(2); // Green

        if selected == 0 {
            print("Start Game");
        } else if selected == 1 {
            print("Load Game");
        } else if selected == 2 {
            print("Settings");
        } else if selected == 3 {
            print("Exit");
        }
    }

    terminal.reset();

    // Wait for Enter
    println("");
    println("");
    terminal.moveCursor(20, 5);
    terminal.setForeground(8); // Gray
    print("Press Enter to finish...");
    terminal.reset();

    terminal.enableRawMode();
    var waiting = true;
    while waiting {
        val key = terminal.readKey();
        if key == 13 {
            waiting = false;
        }
    }
    terminal.disableRawMode();
}

fn getYesNo(prompt: string) bool {
    print(prompt);
    print(" (y/n): ");

    terminal.enableRawMode();
    var result = false;
    var done = false;

    while !done {
        val key = terminal.readKey();

        if key == 121 || key == 89 { // 'y' or 'Y'
            print("y");
            result = true;
            done = true;
        } else if key == 110 || key == 78 { // 'n' or 'N'
            print("n");
            result = false;
            done = true;
        }
    }

    terminal.disableRawMode();
    println("");

    return result;
}

fn validateNumber(input: string) bool {
    // Simple number validation
    if strings.length(input) == 0 {
        return false;
    }

    var i = 0;
    while i < strings.length(input) {
        // In a real implementation, we'd check if each character is a digit
        // For now, just return true
        i = i + 1;
    }

    return true;
}
