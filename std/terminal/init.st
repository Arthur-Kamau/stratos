// Terminal module - ANSI terminal control and manipulation
// Provides functions for colors, cursor control, input, and screen management

package terminal;

// ============================================================================
// Types
// ============================================================================

enum Color {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BRIGHT_BLACK,
    BRIGHT_RED,
    BRIGHT_GREEN,
    BRIGHT_YELLOW,
    BRIGHT_BLUE,
    BRIGHT_MAGENTA,
    BRIGHT_CYAN,
    BRIGHT_WHITE,
    DEFAULT
}

enum Style {
    RESET,
    BOLD,
    DIM,
    ITALIC,
    UNDERLINE,
    BLINK,
    REVERSE,
    HIDDEN,
    STRIKETHROUGH
}

class TerminalSize {
    var width: int;
    var height: int;
}

class Key {
    var char: string;
    var code: int;
    var ctrl: bool;
    var alt: bool;
    var shift: bool;
}

// Key codes for special keys
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
val KEY_F3 = 1012;
val KEY_F4 = 1013;
val KEY_F5 = 1014;
val KEY_F6 = 1015;
val KEY_F7 = 1016;
val KEY_F8 = 1017;
val KEY_F9 = 1018;
val KEY_F10 = 1019;
val KEY_F11 = 1020;
val KEY_F12 = 1021;

// ============================================================================
// Native function declarations - Screen Control
// ============================================================================

// Clear the entire screen
fn clear() void;

// Clear from cursor to end of line
fn clearLine() void;

// Clear from cursor to end of screen
fn clearToBottom() void;

// Clear from cursor to start of screen
fn clearToTop() void;

// ============================================================================
// Native function declarations - Cursor Control
// ============================================================================

// Move cursor to specific position (1-based)
fn moveCursor(row: int, col: int) void;

// Move cursor up by n lines
fn cursorUp(n: int) void;

// Move cursor down by n lines
fn cursorDown(n: int) void;

// Move cursor left by n columns
fn cursorLeft(n: int) void;

// Move cursor right by n columns
fn cursorRight(n: int) void;

// Save cursor position
fn saveCursor() void;

// Restore cursor position
fn restoreCursor() void;

// Hide cursor
fn hideCursor() void;

// Show cursor
fn showCursor() void;

// Get cursor position
fn getCursorPosition() Array<int>;

// ============================================================================
// Native function declarations - Colors and Styles
// ============================================================================

// Set foreground color
fn setForeground(color: Color) void;

// Set background color
fn setBackground(color: Color) void;

// Set foreground color using RGB (0-255)
fn setForegroundRGB(r: int, g: int, b: int) void;

// Set background color using RGB (0-255)
fn setBackgroundRGB(r: int, g: int, b: int) void;

// Set text style
fn setStyle(style: Style) void;

// Reset all styles and colors
fn reset() void;

// ============================================================================
// Native function declarations - Terminal Properties
// ============================================================================

// Get terminal size
fn getSize() TerminalSize;

// Get terminal width
fn getWidth() int;

// Get terminal height
fn getHeight() int;

// Check if terminal supports colors
fn supportsColor() bool;

// Check if running in a TTY
fn isTTY() bool;

// ============================================================================
// Native function declarations - Input Control
// ============================================================================

// Enable raw mode (no buffering, no echo)
fn enableRawMode() void;

// Disable raw mode (restore normal mode)
fn disableRawMode() void;

// Read a single character (non-blocking if in raw mode)
fn readChar() string;

// Read a single key press (handles escape sequences)
fn readKey() Key;

// Read a line of input
fn readLine() string;

// Read input with a prompt
fn input(prompt: string) string;

// Check if input is available (non-blocking)
fn hasInput() bool;

// ============================================================================
// Native function declarations - Alternate Screen
// ============================================================================

// Switch to alternate screen buffer
fn useAlternateScreen() void;

// Switch back to main screen buffer
fn useMainScreen() void;

// ============================================================================
// Pure Stratos implementations - Helper Functions
// ============================================================================

// Print with color
fn printColor(text: string, fg: Color) void {
    setForeground(fg);
    print(text);
    reset();
}

// Print with color and background
fn printColorBg(text: string, fg: Color, bg: Color) void {
    setForeground(fg);
    setBackground(bg);
    print(text);
    reset();
}

// Print styled text
fn printStyled(text: string, style: Style) void {
    setStyle(style);
    print(text);
    reset();
}

// Print bold text
fn printBold(text: string) void {
    setStyle(Style.BOLD);
    print(text);
    reset();
}

// Print italic text
fn printItalic(text: string) void {
    setStyle(Style.ITALIC);
    print(text);
    reset();
}

// Print underlined text
fn printUnderline(text: string) void {
    setStyle(Style.UNDERLINE);
    print(text);
    reset();
}

// Print error message in red
fn printError(text: string) void {
    setForeground(Color.RED);
    setStyle(Style.BOLD);
    print("ERROR: ");
    reset();
    setForeground(Color.RED);
    println(text);
    reset();
}

// Print success message in green
fn printSuccess(text: string) void {
    setForeground(Color.GREEN);
    setStyle(Style.BOLD);
    print("✓ ");
    reset();
    setForeground(Color.GREEN);
    println(text);
    reset();
}

// Print warning message in yellow
fn printWarning(text: string) void {
    setForeground(Color.YELLOW);
    setStyle(Style.BOLD);
    print("⚠ ");
    reset();
    setForeground(Color.YELLOW);
    println(text);
    reset();
}

// Print info message in blue
fn printInfo(text: string) void {
    setForeground(Color.BLUE);
    setStyle(Style.BOLD);
    print("ℹ ");
    reset();
    setForeground(Color.BLUE);
    println(text);
    reset();
}

// Create a colored string (returns ANSI escape codes)
fn colored(text: string, fg: Color) string {
    val code = getColorCode(fg, false);
    return code + text + "\x1b[0m";
}

// Create a colored string with background
fn coloredBg(text: string, fg: Color, bg: Color) string {
    val fgCode = getColorCode(fg, false);
    val bgCode = getColorCode(bg, true);
    return fgCode + bgCode + text + "\x1b[0m";
}

// Helper to get ANSI color code
fn getColorCode(color: Color, background: bool) string {
    val base = background ? 40 : 30;
    val brightBase = background ? 100 : 90;

    // Match color enum to ANSI code
    if color == Color.BLACK {
        return "\x1b[" + convert.toString(base + 0) + "m";
    } else if color == Color.RED {
        return "\x1b[" + convert.toString(base + 1) + "m";
    } else if color == Color.GREEN {
        return "\x1b[" + convert.toString(base + 2) + "m";
    } else if color == Color.YELLOW {
        return "\x1b[" + convert.toString(base + 3) + "m";
    } else if color == Color.BLUE {
        return "\x1b[" + convert.toString(base + 4) + "m";
    } else if color == Color.MAGENTA {
        return "\x1b[" + convert.toString(base + 5) + "m";
    } else if color == Color.CYAN {
        return "\x1b[" + convert.toString(base + 6) + "m";
    } else if color == Color.WHITE {
        return "\x1b[" + convert.toString(base + 7) + "m";
    } else if color == Color.BRIGHT_BLACK {
        return "\x1b[" + convert.toString(brightBase + 0) + "m";
    } else if color == Color.BRIGHT_RED {
        return "\x1b[" + convert.toString(brightBase + 1) + "m";
    } else if color == Color.BRIGHT_GREEN {
        return "\x1b[" + convert.toString(brightBase + 2) + "m";
    } else if color == Color.BRIGHT_YELLOW {
        return "\x1b[" + convert.toString(brightBase + 3) + "m";
    } else if color == Color.BRIGHT_BLUE {
        return "\x1b[" + convert.toString(brightBase + 4) + "m";
    } else if color == Color.BRIGHT_MAGENTA {
        return "\x1b[" + convert.toString(brightBase + 5) + "m";
    } else if color == Color.BRIGHT_CYAN {
        return "\x1b[" + convert.toString(brightBase + 6) + "m";
    } else if color == Color.BRIGHT_WHITE {
        return "\x1b[" + convert.toString(brightBase + 7) + "m";
    }

    return background ? "\x1b[49m" : "\x1b[39m"; // Default color
}

// Draw a horizontal line
fn drawLine(width: int, char: string) void {
    var i = 0;
    while i < width {
        print(char);
        val i = i + 1;
    }
    println("");
}

// Draw a box
fn drawBox(x: int, y: int, width: int, height: int, title: string) void {
    // Top border
    moveCursor(y, x);
    print("┌");
    drawLine(width - 2, "─");
    moveCursor(y, x + width - 1);
    print("┐");

    // Title if provided
    if title != "" {
        moveCursor(y, x + 2);
        print(" " + title + " ");
    }

    // Sides
    var i = 1;
    while i < height - 1 {
        moveCursor(y + i, x);
        print("│");
        moveCursor(y + i, x + width - 1);
        print("│");
        val i = i + 1;
    }

    // Bottom border
    moveCursor(y + height - 1, x);
    print("└");
    drawLine(width - 2, "─");
    moveCursor(y + height - 1, x + width - 1);
    print("┘");
}

// Center text on screen
fn printCentered(text: string, row: int) void {
    val width = getWidth();
    val col = (width - strings.length(text)) / 2;
    moveCursor(row, col);
    print(text);
}

// Print at specific position
fn printAt(row: int, col: int, text: string) void {
    moveCursor(row, col);
    print(text);
}

// Progress bar
fn drawProgressBar(current: int, total: int, width: int) void {
    val percentage = (current * 100) / total;
    val filled = (current * width) / total;

    print("[");

    var i = 0;
    while i < width {
        if i < filled {
            print("█");
        } else {
            print("░");
        }
        val i = i + 1;
    }

    print("] " + convert.toString(percentage) + "%");
}

// Menu system
class MenuItem {
    var label: string;
    var action: Function<void>;
}

fn showMenu(title: string, items: Array<MenuItem>) int {
    val selected = 0;

    while true {
        clear();
        printCentered(title, 2);
        println("");
        println("");

        var i = 0;
        while i < items.length() {
            val item = items[i];

            if i == selected {
                setStyle(Style.REVERSE);
            }

            print("  ");
            println(item.label);

            if i == selected {
                reset();
            }

            val i = i + 1;
        }

        println("");
        println("Use ↑↓ arrows to navigate, Enter to select, Esc to cancel");

        val key = readKey();

        if key.code == KEY_UP {
            if selected > 0 {
                val selected = selected - 1;
            }
        } else if key.code == KEY_DOWN {
            if selected < items.length() - 1 {
                val selected = selected + 1;
            }
        } else if key.code == KEY_ENTER {
            return selected;
        } else if key.code == KEY_ESC {
            return -1;
        }
    }

    return -1;
}

// Confirm dialog
fn confirm(message: string) bool {
    print(message);
    print(" (y/n): ");

    val response = readLine();
    return response == "y" || response == "Y" || response == "yes" || response == "Yes";
}

// Text input with validation
fn inputWithValidation(prompt: string, validator: Function<string, bool>) string {
    while true {
        val value = input(prompt);
        if validator(value) {
            return value;
        }
        printError("Invalid input. Please try again.");
    }
    return "";
}

// Spinner animation
class Spinner {
    var frames: Array<string>;
    var currentFrame: int;
    var running: bool;

    constructor() {
        this.frames = ["⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"];
        this.currentFrame = 0;
        this.running = false;
    }

    pub fn next() string {
        val frame = this.frames[this.currentFrame];
        val this.currentFrame = (this.currentFrame + 1) % this.frames.length();
        return frame;
    }
}

fn newSpinner() Spinner {
    return Spinner();
}

// Table printing
fn printTable(headers: Array<string>, rows: Array<Array<string>>) void {
    // Calculate column widths
    val widths = Array<int>();

    var i = 0;
    while i < headers.length() {
        widths.add(strings.length(headers[i]));
        val i = i + 1;
    }

    // Update widths based on row data
    for val row in rows {
        var j = 0;
        while j < row.length() {
            val len = strings.length(row[j]);
            if len > widths[j] {
                widths[j] = len;
            }
            val j = j + 1;
        }
    }

    // Print header
    setStyle(Style.BOLD);
    var k = 0;
    while k < headers.length() {
        print(strings.padRight(headers[k], widths[k] + 2));
        val k = k + 1;
    }
    reset();
    println("");

    // Print separator
    var l = 0;
    while l < headers.length() {
        drawLine(widths[l] + 2, "─");
        val l = l + 1;
    }
    println("");

    // Print rows
    for val row in rows {
        var m = 0;
        while m < row.length() {
            print(strings.padRight(row[m], widths[m] + 2));
            val m = m + 1;
        }
        println("");
    }
}
