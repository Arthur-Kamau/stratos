// Simple number converter demonstrating the convert module

use convert;

fn main() {
    println("=== Number Converter ===");
    println("");

    // Get user input (simulated)
    val userInput: string = "3.14159";
    
    println("Input: " + userInput);
    println("");

    // Convert to different types
    val asInt: int = convert.toInt(userInput);
    val asDouble: double = convert.toDouble(userInput);
    
    println("As integer: " + asInt);
    println("As double: " + asDouble);
    println("");

    // Format the number
    println("Formatted outputs:");
    println("  2 decimals: " + convert.toFixed(asDouble, 2));
    println("  4 decimals: " + convert.toFixed(asDouble, 4));
    println("  Scientific: " + convert.toExponential(asDouble, 3));
    println("");

    // Base conversions
    val num: int = 42;
    println("The number " + num + " in different bases:");
    println("  Binary: " + convert.toBinary(num));
    println("  Octal: " + convert.toOct(num));
    println("  Hex: " + convert.toHex(num));
}
