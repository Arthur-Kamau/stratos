package main;

fn main() {
    println("--- Stratos Casting Examples ---");

    // --- Double to Int (Truncation) ---
    val d1 = 3.14 as int;
    println("3.14 as int: " + d1); // Expected: 3
    val d2 = 3.99 as int;
    println("3.99 as int: " + d2); // Expected: 3
    val d3 = -2.5 as int;
    println("-2.5 as int: " + d3); // Expected: -2

    // --- Int to Double (Promotion) ---
    val i1 = 10 as double;
    println("10 as double: " + i1); // Expected: 10.0
    val i2 = -5 as double;
    println("-5 as double: " + i2); // Expected: -5.0

    // --- Bool to Int ---
    val b1 = true as int;
    println("true as int: " + b1); // Expected: 1
    val b2 = false as int;
    println("false as int: " + b2); // Expected: 0

    // --- Int to Bool ---
    val intTrue = 1 as bool;
    println("1 as bool: " + intTrue); // Expected: true
    val intFalse = 0 as bool;
    println("0 as bool: " + intFalse); // Expected: false
    val intAny = 123 as bool;
    println("123 as bool: " + intAny); // Expected: true

    // --- Double to Bool ---
    val doubleTrue = 0.5 as bool;
    println("0.5 as bool: " + doubleTrue); // Expected: true
    val doubleFalse = 0.0 as bool;
    println("0.0 as bool: " + doubleFalse); // Expected: false
    val doubleAny = -10.0 as bool;
    println("-10.0 as bool: " + doubleAny); // Expected: true

    // --- String to Int/Double ---
    val s1 = "123" as int;
    println("\"123\" as int: " + s1); // Expected: 123
    val s2 = "3.14" as double;
    println("\"3.14\" as double: " + s2); // Expected: 3.14

    // --- Int/Double/Bool to String ---
    val iToStr = 456 as string;
    println("456 as string: \"" + iToStr + "\""); // Expected: "456"
    val dToStr = 7.89 as string;
    println("7.89 as string: \"" + dToStr + "\""); // Expected: "7.89"
    val bToStr = true as string;
    println("true as string: \"" + bToStr + "\""); // Expected: "true"

    // --- Char conversions ---
    val cToInt = 'A' as int;
    println("'A' as int: " + cToInt); // Expected: 65 (ASCII)
    val intToC = 97 as char;
    println("97 as char: '" + intToC + "'" ); // Expected: 'a'
    val strToC = "Z" as char;
    println("\"Z\" as char: '" + strToC + "'" ); // Expected: 'Z'
    
    println("\n--- Casting Tests Complete ---");
}
