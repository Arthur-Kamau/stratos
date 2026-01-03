package main;

use terminal;
use convert;
use strings;

// Function forward declarations
fn showWelcomeScreen() void;
fn showMainMenu() int;
fn drawMenuItem(row: int, item: int, selected: int) void;
fn demoColors() void;
fn demoStyles() void;
fn demoBoxDrawing() void;
fn demoProgressBar() void;
fn demoRawInput() void;
fn waitForKey() void;
fn drawProgress(current: int, width: int) void;

fn main() {
    // Check if running in a terminal
    if !terminal.isTTY() {
        println("This program requires a terminal to run!");
        return;
    }

    // Use alternate screen to preserve terminal content
    terminal.useAlternateScreen();
    terminal.clear();
    terminal.hideCursor();

    showWelcomeScreen();

    var running = true;
    while running {
        val choice = showMainMenu();

        if choice == 1 {
            demoColors();
        } else if choice == 2 {
            demoStyles();
        } else if choice == 3 {
            demoBoxDrawing();
        } else if choice == 4 {
            demoProgressBar();
        } else if choice == 5 {
            demoRawInput();
        } else if choice == 6 {
            running = false;
        }
    }

    // Cleanup
    terminal.showCursor();
    terminal.reset();
    terminal.useMainScreen();
    println("Thanks for using Stratos Terminal Demo!");
}

fn showWelcomeScreen() void {
    terminal.clear();

    // Draw title with colors
    terminal.moveCursor(3, 1);
    terminal.setForeground(6); // Cyan
    terminal.setStyle(1); // Bold

    val width = terminal.getWidth();
    val title = "STRATOS TERMINAL DEMO";
    val titleLen = strings.length(title);
    val titleCol = (width - titleLen) / 2;

    terminal.moveCursor(3, titleCol);
    print(title);
    terminal.reset();

    // Draw subtitle
    terminal.moveCursor(5, 1);
    terminal.setForeground(7); // White
    val subtitle = "Demonstrating Terminal Control Capabilities";
    val subtitleLen = strings.length(subtitle);
    val subtitleCol = (width - subtitleLen) / 2;
    terminal.moveCursor(5, subtitleCol);
    print(subtitle);
    terminal.reset();

    // Show some info
    terminal.moveCursor(8, 5);
    terminal.setForeground(2); // Green
    print("Terminal Size: ");
    terminal.reset();
    print(convert.toString(terminal.getWidth()));
    print(" x ");
    print(convert.toString(terminal.getHeight()));

    terminal.moveCursor(10, 5);
    terminal.setForeground(2); // Green
    print("Color Support: ");
    terminal.reset();
    if terminal.supportsColor() {
        terminal.setForeground(2); // Green
        print("Yes");
    } else {
        terminal.setForeground(1); // Red
        print("No");
    }
    terminal.reset();

    terminal.moveCursor(13, 1);
    terminal.setForeground(8); // Bright black (gray)
    val msg = "Press any key to continue...";
    val msgLen = strings.length(msg);
    val msgCol = (width - msgLen) / 2;
    terminal.moveCursor(13, msgCol);
    print(msg);
    terminal.reset();

    // Wait for key press
    terminal.enableRawMode();
    terminal.readKey();
    terminal.disableRawMode();
}

fn showMainMenu() int {
    var selected = 1;
    var running = true;

    terminal.enableRawMode();

    while running {
        terminal.clear();

        // Title
        terminal.moveCursor(2, 5);
        terminal.setForeground(6); // Cyan
        terminal.setStyle(1); // Bold
        print("MAIN MENU");
        terminal.reset();

        // Menu items
        drawMenuItem(4, 1, selected);
        drawMenuItem(5, 2, selected);
        drawMenuItem(6, 3, selected);
        drawMenuItem(7, 4, selected);
        drawMenuItem(8, 5, selected);
        drawMenuItem(9, 6, selected);

        // Instructions
        terminal.moveCursor(12, 5);
        terminal.setForeground(8); // Gray
        print("Use UP/DOWN arrows to navigate, ENTER to select, ESC to quit");
        terminal.reset();

        // Read key
        val key = terminal.readKey();

        if key == 1001 { // UP
            if selected > 1 {
                selected = selected - 1;
            }
        } else if key == 1002 { // DOWN
            if selected < 6 {
                selected = selected + 1;
            }
        } else if key == 13 { // ENTER
            running = false;
        } else if key == 27 { // ESC
            selected = 6;
            running = false;
        }
    }

    terminal.disableRawMode();
    return selected;
}

fn drawMenuItem(row: int, item: int, selected: int) void {
    terminal.moveCursor(row, 7);

    if item == selected {
        terminal.setStyle(7); // Reverse
        terminal.setForeground(6); // Cyan
    }

    if item == 1 {
        print("  1. Color Demonstration       ");
    } else if item == 2 {
        print("  2. Text Style Demonstration  ");
    } else if item == 3 {
        print("  3. Box Drawing               ");
    } else if item == 4 {
        print("  4. Progress Bar Demo         ");
    } else if item == 5 {
        print("  5. Raw Input Demo            ");
    } else if item == 6 {
        print("  6. Exit                      ");
    }

    terminal.reset();
}

fn demoColors() void {
    terminal.clear();

    terminal.moveCursor(2, 5);
    terminal.setForeground(6); // Cyan
    terminal.setStyle(1); // Bold
    print("COLOR DEMONSTRATION");
    terminal.reset();

    terminal.moveCursor(4, 5);
    print("Standard Colors:");

    // Show standard colors - manually unrolled
    terminal.moveCursor(5, 7);
    terminal.setForeground(0);
    print("█████ ");
    terminal.reset();
    print("Black");

    terminal.moveCursor(6, 7);
    terminal.setForeground(1);
    print("█████ ");
    terminal.reset();
    print("Red");

    terminal.moveCursor(7, 7);
    terminal.setForeground(2);
    print("█████ ");
    terminal.reset();
    print("Green");

    terminal.moveCursor(8, 7);
    terminal.setForeground(3);
    print("█████ ");
    terminal.reset();
    print("Yellow");

    terminal.moveCursor(9, 7);
    terminal.setForeground(4);
    print("█████ ");
    terminal.reset();
    print("Blue");

    terminal.moveCursor(10, 7);
    terminal.setForeground(5);
    print("█████ ");
    terminal.reset();
    print("Magenta");

    terminal.moveCursor(11, 7);
    terminal.setForeground(6);
    print("█████ ");
    terminal.reset();
    print("Cyan");

    terminal.moveCursor(12, 7);
    terminal.setForeground(7);
    print("█████ ");
    terminal.reset();
    print("White");

    terminal.moveCursor(14, 5);
    print("Bright Colors:");

    // Show bright colors - manually unrolled
    terminal.moveCursor(15, 7);
    terminal.setForeground(8);
    print("█████ ");
    terminal.reset();
    print("Bright Black");

    terminal.moveCursor(16, 7);
    terminal.setForeground(9);
    print("█████ ");
    terminal.reset();
    print("Bright Red");

    terminal.moveCursor(17, 7);
    terminal.setForeground(10);
    print("█████ ");
    terminal.reset();
    print("Bright Green");

    terminal.moveCursor(18, 7);
    terminal.setForeground(11);
    print("█████ ");
    terminal.reset();
    print("Bright Yellow");

    terminal.moveCursor(19, 7);
    terminal.setForeground(12);
    print("█████ ");
    terminal.reset();
    print("Bright Blue");

    terminal.moveCursor(20, 7);
    terminal.setForeground(13);
    print("█████ ");
    terminal.reset();
    print("Bright Magenta");

    terminal.moveCursor(21, 7);
    terminal.setForeground(14);
    print("█████ ");
    terminal.reset();
    print("Bright Cyan");

    terminal.moveCursor(22, 7);
    terminal.setForeground(15);
    print("█████ ");
    terminal.reset();
    print("Bright White");

    waitForKey();
}

fn demoStyles() void {
    terminal.clear();

    terminal.moveCursor(2, 5);
    terminal.setForeground(6); // Cyan
    terminal.setStyle(1); // Bold
    print("TEXT STYLE DEMONSTRATION");
    terminal.reset();

    terminal.moveCursor(5, 5);
    terminal.setStyle(1); // Bold
    print("Bold Text");
    terminal.reset();

    terminal.moveCursor(7, 5);
    terminal.setStyle(2); // Dim
    print("Dim Text");
    terminal.reset();

    terminal.moveCursor(9, 5);
    terminal.setStyle(3); // Italic
    print("Italic Text");
    terminal.reset();

    terminal.moveCursor(11, 5);
    terminal.setStyle(4); // Underline
    print("Underlined Text");
    terminal.reset();

    terminal.moveCursor(13, 5);
    terminal.setStyle(7); // Reverse
    print("Reversed Text");
    terminal.reset();

    terminal.moveCursor(15, 5);
    terminal.setStyle(9); // Strikethrough
    print("Strikethrough Text");
    terminal.reset();

    terminal.moveCursor(17, 5);
    terminal.setForeground(1); // Red
    terminal.setStyle(1); // Bold
    print("Bold ");
    terminal.setForeground(2); // Green
    print("Colored ");
    terminal.setForeground(4); // Blue
    print("Text");
    terminal.reset();

    waitForKey();
}

fn demoBoxDrawing() void {
    terminal.clear();

    terminal.moveCursor(2, 5);
    terminal.setForeground(6); // Cyan
    terminal.setStyle(1); // Bold
    print("BOX DRAWING DEMONSTRATION");
    terminal.reset();

    // Draw a box
    terminal.moveCursor(5, 10);
    terminal.setForeground(3); // Yellow
    print("┌────────────────────────────────┐");

    // Draw box sides manually
    terminal.moveCursor(6, 10);
    print("│                                │");
    terminal.moveCursor(7, 10);
    print("│                                │");
    terminal.moveCursor(8, 10);
    print("│                                │");
    terminal.moveCursor(9, 10);
    print("│                                │");
    terminal.moveCursor(10, 10);
    print("│                                │");
    terminal.moveCursor(11, 10);
    print("│                                │");
    terminal.moveCursor(12, 10);
    print("│                                │");
    terminal.moveCursor(13, 10);
    print("│                                │");
    terminal.moveCursor(14, 10);
    print("│                                │");

    terminal.moveCursor(15, 10);
    print("└────────────────────────────────┘");
    terminal.reset();

    // Add title to box
    terminal.moveCursor(5, 15);
    terminal.setForeground(2); // Green
    terminal.setStyle(1); // Bold
    print(" Sample Box ");
    terminal.reset();

    // Add content
    terminal.moveCursor(7, 13);
    print("This is a demonstration of");
    terminal.moveCursor(8, 13);
    print("box drawing using Unicode");
    terminal.moveCursor(9, 13);
    print("box-drawing characters.");

    terminal.moveCursor(11, 13);
    terminal.setForeground(2); // Green
    print("✓ Borders");
    terminal.reset();

    terminal.moveCursor(12, 13);
    terminal.setForeground(2); // Green
    print("✓ Colors");
    terminal.reset();

    terminal.moveCursor(13, 13);
    terminal.setForeground(2); // Green
    print("✓ Text positioning");
    terminal.reset();

    waitForKey();
}

fn demoProgressBar() void {
    terminal.clear();

    terminal.moveCursor(2, 5);
    terminal.setForeground(6); // Cyan
    terminal.setStyle(1); // Bold
    print("PROGRESS BAR DEMONSTRATION");
    terminal.reset();

    terminal.moveCursor(5, 5);
    print("Simulating a task with progress bar...");

    var progress = 0;
    while progress <= 100 {
        terminal.moveCursor(8, 10);
        drawProgress(progress, 40);

        terminal.moveCursor(10, 10);
        terminal.setForeground(2); // Green
        print("Progress: ");
        print(convert.toString(progress));
        print("%  ");
        terminal.reset();

        // Simulate work with a simple delay loop
        var delay = 0;
        while delay < 50000000 {
            delay = delay + 1;
        }

        progress = progress + 5;
    }

    terminal.moveCursor(13, 5);
    terminal.setForeground(2); // Green
    terminal.setStyle(1); // Bold
    print("Task completed successfully!");
    terminal.reset();

    waitForKey();
}

fn drawProgress(current: int, width: int) void {
    val filled = (current * width) / 100;

    terminal.setForeground(8); // Gray
    print("[");
    terminal.reset();

    var i = 0;
    while i < width {
        if i < filled {
            terminal.setForeground(2); // Green
            print("█");
        } else {
            terminal.setForeground(8); // Gray
            print("░");
        }
        i = i + 1;
    }

    terminal.setForeground(8); // Gray
    print("]");
    terminal.reset();
}

fn demoRawInput() void {
    terminal.clear();

    terminal.moveCursor(2, 5);
    terminal.setForeground(6); // Cyan
    terminal.setStyle(1); // Bold
    print("RAW INPUT DEMONSTRATION");
    terminal.reset();

    terminal.moveCursor(5, 5);
    print("Press arrow keys, special keys, or ESC to exit...");

    terminal.moveCursor(7, 5);
    terminal.setForeground(8); // Gray
    print("Key pressed: ");
    terminal.reset();

    var running = true;
    terminal.enableRawMode();

    while running {
        terminal.moveCursor(7, 18);
        terminal.clearLine();

        val key = terminal.readKey();

        terminal.moveCursor(7, 18);

        if key == 1001 {
            terminal.setForeground(2); // Green
            print("UP ARROW    ");
        } else if key == 1002 {
            terminal.setForeground(2); // Green
            print("DOWN ARROW  ");
        } else if key == 1003 {
            terminal.setForeground(2); // Green
            print("LEFT ARROW  ");
        } else if key == 1004 {
            terminal.setForeground(2); // Green
            print("RIGHT ARROW ");
        } else if key == 13 {
            terminal.setForeground(3); // Yellow
            print("ENTER       ");
        } else if key == 27 {
            terminal.setForeground(1); // Red
            print("ESC         ");
            running = false;
        } else if key == 127 {
            terminal.setForeground(3); // Yellow
            print("BACKSPACE   ");
        } else if key == 9 {
            terminal.setForeground(3); // Yellow
            print("TAB         ");
        } else {
            terminal.setForeground(7); // White
            print("Character: ");
            print(convert.toString(key));
            print("    ");
        }

        terminal.reset();
    }

    terminal.disableRawMode();
    waitForKey();
}

fn waitForKey() void {
    val height = terminal.getHeight();
    terminal.moveCursor(height - 2, 5);
    terminal.setForeground(8); // Gray
    print("Press any key to return to menu...");
    terminal.reset();

    terminal.enableRawMode();
    terminal.readKey();
    terminal.disableRawMode();
}
