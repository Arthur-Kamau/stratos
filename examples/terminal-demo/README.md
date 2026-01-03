# Terminal Demo

This example demonstrates the capabilities of Stratos's terminal module for building interactive terminal applications.

## Features Demonstrated

- **Screen Control**: Clearing screen, alternate screen buffer
- **Cursor Control**: Moving cursor, hiding/showing cursor
- **Colors**: 16-color palette (8 standard + 8 bright colors)
- **RGB Colors**: True color support using RGB values
- **Text Styles**: Bold, italic, underline, reverse, and more
- **Raw Input**: Reading keypresses without Enter
- **Special Keys**: Arrow keys, function keys, Enter, ESC, etc.
- **Terminal Properties**: Getting terminal size, checking color support

## Files

- `src/simple_test.st` - Simple test demonstrating basic terminal functions
- `src/minimal_test.st` - Minimal test for quick verification
- `src/simple_input.st` - Demonstrates reading user input with `terminal.readLine()`
- `src/number_guesser.st` - Interactive number guessing game
- `src/main.st` - Full interactive demo (work in progress)

## Running the Examples

### Simple Test

```bash
../../src/interpreter/cpp/build/stratos run src/simple_test.st
```

This demonstrates:
- Terminal detection (isTTY)
- Terminal size (width/height)
- Color support detection
- Basic colors (red, green, yellow, blue, magenta, cyan)
- Text styles (bold, italic, underline)
- Cursor positioning

### Minimal Test

```bash
../../src/interpreter/cpp/build/stratos run src/minimal_test.st
```

A quick test that clears the screen and prints a message.

### Simple Input Test

```bash
../../src/interpreter/cpp/build/stratos run src/simple_input.st
```

Demonstrates reading user input with `terminal.readLine()`:

```stratos
print("Enter your name: ");
val name = terminal.readLine();
print("Hello, ");
print(name);
println("!");
```

### Number Guessing Game

```bash
../../src/interpreter/cpp/build/stratos run src/number_guesser.st
```

An interactive number guessing game demonstrating:
- User input with `terminal.readLine()`
- Colors for feedback (green for correct, yellow for hints)
- Cursor positioning for a clean UI
- Input conversion with `convert.toInt()`

## Terminal Module API

The terminal module provides the following capabilities:

### Screen Control
- `clear()` - Clear entire screen
- `clearLine()` - Clear current line
- `clearToBottom()` - Clear to bottom of screen
- `clearToTop()` - Clear to top of screen

### Cursor Control
- `moveCursor(row, col)` - Move cursor to position
- `cursorUp(n)` - Move cursor up
- `cursorDown(n)` - Move cursor down
- `cursorLeft(n)` - Move cursor left
- `cursorRight(n)` - Move cursor right
- `hideCursor()` - Hide cursor
- `showCursor()` - Show cursor
- `saveCursor()` - Save cursor position
- `restoreCursor()` - Restore cursor position

### Colors and Styles
- `setForeground(color)` - Set text color
- `setBackground(color)` - Set background color
- `setForegroundRGB(r, g, b)` - Set text color with RGB
- `setBackgroundRGB(r, g, b)` - Set background color with RGB
- `setStyle(style)` - Set text style
- `reset()` - Reset all colors and styles

### Terminal Properties
- `getWidth()` - Get terminal width
- `getHeight()` - Get terminal height
- `supportsColor()` - Check if terminal supports colors
- `isTTY()` - Check if running in a terminal

### Input Control
- `enableRawMode()` - Enable raw input mode
- `disableRawMode()` - Disable raw input mode
- `readChar()` - Read a single character
- `readKey()` - Read a key (handles special keys)
- `readLine()` - Read a line of input
- `input(prompt)` - Prompt and read input
- `hasInput()` - Check if input is available

### Alternate Screen
- `useAlternateScreen()` - Switch to alternate screen
- `useMainScreen()` - Switch back to main screen

## Color Codes

Standard colors (0-7):
- 0: Black
- 1: Red
- 2: Green
- 3: Yellow
- 4: Blue
- 5: Magenta
- 6: Cyan
- 7: White

Bright colors (8-15):
- 8: Bright Black (Gray)
- 9: Bright Red
- 10: Bright Green
- 11: Bright Yellow
- 12: Bright Blue
- 13: Bright Magenta
- 14: Bright Cyan
- 15: Bright White

## Style Codes

- 0: Reset
- 1: Bold
- 2: Dim
- 3: Italic
- 4: Underline
- 5: Blink
- 7: Reverse (swap foreground/background)
- 8: Hidden
- 9: Strikethrough

## Key Codes

Special keys return codes > 1000:
- 1001: Up Arrow
- 1002: Down Arrow
- 1003: Left Arrow
- 1004: Right Arrow
- 1005: Delete
- 1006: Home
- 1007: End
- 1008: Page Up
- 1009: Page Down
- 1010-1021: F1-F12

Regular keys return their ASCII values:
- 13: Enter
- 27: ESC
- 9: Tab
- 127: Backspace

## Example: Simple Color Demo

```stratos
use terminal;

fn main() {
    terminal.setForeground(1);  // Red
    print("Red text ");

    terminal.setForeground(2);  // Green
    print("Green text ");

    terminal.setForeground(4);  // Blue
    print("Blue text");

    terminal.reset();
    println("");  // Back to normal
}
```

## Example: Interactive Menu

```stratos
use terminal;

fn main() {
    terminal.clear();
    terminal.moveCursor(2, 5);
    terminal.setForeground(6);  // Cyan
    terminal.setStyle(1);        // Bold
    print("MY MENU");
    terminal.reset();

    terminal.moveCursor(4, 5);
    print("1. Option One");
    terminal.moveCursor(5, 5);
    print("2. Option Two");
    terminal.moveCursor(6, 5);
    print("3. Exit");

    terminal.enableRawMode();
    val key = terminal.readKey();
    terminal.disableRawMode();
}
```

## Requirements

- Terminal with ANSI escape sequence support
- TTY (not redirected output)
- For full color support: Terminal with 256-color or truecolor support

## Platform Support

- **Linux/Unix**: Full support
- **macOS**: Full support
- **Windows 10+**: Support with Windows Terminal or modern console

## See Also

- [Terminal Module Documentation](../../docs/stdlib/terminal.md)
- [Standard Library Documentation](../../std/README.md)
