# Terminal Module

The `terminal` module provides ANSI terminal control capabilities for building interactive terminal applications. It supports colors, cursor control, text styling, and raw input handling.

## Table of Contents

- [Screen Control](#screen-control)
- [Cursor Control](#cursor-control)
- [Colors and Styles](#colors-and-styles)
- [Terminal Properties](#terminal-properties)
- [Input Control](#input-control)
- [Alternate Screen](#alternate-screen)
- [Helper Functions](#helper-functions)

## Screen Control

### `clear() void`

Clears the entire screen and moves cursor to home position.

```stratos
use terminal;

fn main() {
    terminal.clear();
    println("Screen cleared!");
}
```

### `clearLine() void`

Clears from cursor position to end of line.

### `clearToBottom() void`

Clears from cursor position to end of screen.

### `clearToTop() void`

Clears from cursor position to start of screen.

## Cursor Control

### `moveCursor(row: int, col: int) void`

Moves cursor to specific position (1-based indexing).

```stratos
terminal.moveCursor(10, 20);  // Move to row 10, column 20
print("Text at specific position");
```

### `cursorUp(n: int) void`

Moves cursor up by n lines.

### `cursorDown(n: int) void`

Moves cursor down by n lines.

### `cursorLeft(n: int) void`

Moves cursor left by n columns.

### `cursorRight(n: int) void`

Moves cursor right by n columns.

### `saveCursor() void`

Saves the current cursor position.

### `restoreCursor() void`

Restores the previously saved cursor position.

### `hideCursor() void`

Hides the cursor.

### `showCursor() void`

Shows the cursor.

## Colors and Styles

### Color Enum

```stratos
enum Color {
    BLACK,          // 0
    RED,            // 1
    GREEN,          // 2
    YELLOW,         // 3
    BLUE,           // 4
    MAGENTA,        // 5
    CYAN,           // 6
    WHITE,          // 7
    BRIGHT_BLACK,   // 8 (Gray)
    BRIGHT_RED,     // 9
    BRIGHT_GREEN,   // 10
    BRIGHT_YELLOW,  // 11
    BRIGHT_BLUE,    // 12
    BRIGHT_MAGENTA, // 13
    BRIGHT_CYAN,    // 14
    BRIGHT_WHITE,   // 15
    DEFAULT         // 16
}
```

### `setForeground(color: int) void`

Sets the foreground (text) color.

```stratos
terminal.setForeground(1);  // Red
print("Red text");
terminal.reset();
```

### `setBackground(color: int) void`

Sets the background color.

```stratos
terminal.setBackground(4);  // Blue background
print("Text with blue background");
terminal.reset();
```

### `setForegroundRGB(r: int, g: int, b: int) void`

Sets foreground color using RGB values (0-255).

```stratos
terminal.setForegroundRGB(255, 128, 0);  // Orange
print("Orange text");
terminal.reset();
```

### `setBackgroundRGB(r: int, g: int, b: int) void`

Sets background color using RGB values (0-255).

### Style Enum

```stratos
enum Style {
    RESET,           // 0
    BOLD,            // 1
    DIM,             // 2
    ITALIC,          // 3
    UNDERLINE,       // 4
    BLINK,           // 5
    REVERSE,         // 7 (Invert fg/bg)
    HIDDEN,          // 8
    STRIKETHROUGH    // 9
}
```

### `setStyle(style: int) void`

Sets the text style.

```stratos
terminal.setStyle(1);  // Bold
print("Bold text");
terminal.reset();
```

### `reset() void`

Resets all colors and styles to default.

```stratos
terminal.setForeground(1);
terminal.setStyle(1);
print("Red and bold");
terminal.reset();
print("Back to normal");
```

## Terminal Properties

### `getSize() int`

Returns the terminal width (height implementation pending).

### `getWidth() int`

Returns the terminal width in columns.

```stratos
val width = terminal.getWidth();
println(convert.toString(width));
```

### `getHeight() int`

Returns the terminal height in rows.

```stratos
val height = terminal.getHeight();
println(convert.toString(height));
```

### `supportsColor() bool`

Checks if the terminal supports color output.

```stratos
if terminal.supportsColor() {
    terminal.setForeground(2);
    println("Colorful!");
} else {
    println("No color support");
}
```

### `isTTY() bool`

Checks if stdout is connected to a terminal.

```stratos
if !terminal.isTTY() {
    println("Not running in a terminal!");
    return;
}
```

## Input Control

### `enableRawMode() void`

Enables raw mode (no buffering, no echo).

```stratos
terminal.enableRawMode();
// Read keys without Enter
terminal.disableRawMode();
```

### `disableRawMode() void`

Disables raw mode and restores normal terminal settings.

### `readChar() string`

Reads a single character (requires raw mode for immediate response).

```stratos
terminal.enableRawMode();
val c = terminal.readChar();
terminal.disableRawMode();
```

### `readKey() int`

Reads a key press and returns a key code. Handles special keys like arrows.

```stratos
terminal.enableRawMode();
val key = terminal.readKey();

if key == 1001 {  // UP arrow
    println("Up arrow pressed");
} else if key == 13 {  // ENTER
    println("Enter pressed");
}

terminal.disableRawMode();
```

#### Key Codes

```stratos
val KEY_UP = 1001;
val KEY_DOWN = 1002;
val KEY_LEFT = 1003;
val KEY_RIGHT = 1004;
val KEY_ENTER = 13;
val KEY_ESC = 27;
val KEY_BACKSPACE = 127;
val KEY_TAB = 9;
val KEY_DELETE = 1005;
val KEY_HOME = 1006;
val KEY_END = 1007;
val KEY_PAGE_UP = 1008;
val KEY_PAGE_DOWN = 1009;
val KEY_F1 = 1010;
val KEY_F2 = 1011;
// ... through KEY_F12 = 1021
```

### `readLine() string`

Reads a line of input (waits for Enter).

```stratos
val line = terminal.readLine();
```

### `input(prompt: string) string`

Displays a prompt and reads a line of input.

```stratos
val name = terminal.input("Enter your name: ");
```

### `hasInput() bool`

Non-blocking check if input is available.

## Alternate Screen

### `useAlternateScreen() void`

Switches to alternate screen buffer (preserves current screen).

```stratos
terminal.useAlternateScreen();
terminal.clear();
// Do your work here
terminal.useMainScreen();  // Restore original screen
```

### `useMainScreen() void`

Switches back to main screen buffer.

## Helper Functions

These are pure Stratos implementations in the terminal module:

### `printColor(text: string, fg: Color) void`

Prints text with foreground color, then resets.

### `printBold(text: string) void`

Prints bold text.

### `printError(text: string) void`

Prints error message in red with "ERROR:" prefix.

### `printSuccess(text: string) void`

Prints success message in green with "✓" prefix.

### `printWarning(text: string) void`

Prints warning message in yellow with "⚠" prefix.

### `printInfo(text: string) void`

Prints info message in blue with "ℹ" prefix.

### `drawProgress(current: int, total: int, width: int) void`

Draws a progress bar.

### `confirm(message: string) bool`

Shows a confirmation prompt (y/n).

## Complete Example

```stratos
package main;

use terminal;
use convert;

fn main() {
    if !terminal.isTTY() {
        println("This program requires a terminal!");
        return;
    }

    // Use alternate screen
    terminal.useAlternateScreen();
    terminal.clear();
    terminal.hideCursor();

    // Show title
    terminal.moveCursor(2, 10);
    terminal.setForeground(6);  // Cyan
    terminal.setStyle(1);        // Bold
    print("WELCOME TO STRATOS");
    terminal.reset();

    // Show menu
    terminal.moveCursor(5, 5);
    print("1. Option One");
    terminal.moveCursor(6, 5);
    print("2. Option Two");
    terminal.moveCursor(7, 5);
    print("3. Exit");

    // Read input
    terminal.moveCursor(10, 5);
    terminal.setForeground(2);  // Green
    print("Select an option: ");
    terminal.reset();

    terminal.enableRawMode();
    val key = terminal.readKey();
    terminal.disableRawMode();

    // Cleanup
    terminal.showCursor();
    terminal.useMainScreen();

    println("You selected: " + convert.toString(key));
}
```

## Platform Support

The terminal module works on:
- **Linux/Unix**: Full support using ANSI escape codes and termios
- **macOS**: Full support
- **Windows**: Partial support (Windows 10+ with ANSI support enabled)

## Best Practices

1. **Always clean up**: Use `terminal.reset()`, `terminal.showCursor()`, and `terminal.useMainScreen()` before exiting
2. **Check TTY**: Use `terminal.isTTY()` to verify you're in a terminal
3. **Raw mode**: Always pair `enableRawMode()` with `disableRawMode()`
4. **Alternate screen**: Use alternate screen for full-screen apps to preserve user's terminal
5. **Error handling**: Gracefully handle non-TTY environments

## See Also

- [IO Module](io.md) - File and stream I/O
- [OS Module](os.md) - Operating system interface
- [Strings Module](strings.md) - String manipulation
