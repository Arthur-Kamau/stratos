// Test escape sequences in strings

fn main() {
    println("=== Testing Escape Sequences ===");

    // Test newline
    println("Line 1\nLine 2\nLine 3");

    // Test tab
    println("Column1\tColumn2\tColumn3");

    // Test carriage return
    println("Before\rAfter");

    // Test backslash
    println("Path: C:\\Users\\test");

    // Test quote
    println("She said \"Hello!\"");

    // Test multiple escapes
    println("Tab:\tNewline:\nBackslash:\\Quote:\"");

    // Test null character (might not display)
    println("Null:\0End");

    println("=== All escape sequences tested ===");
}
