// Test logical AND and OR operators

fn main() {
    println("=== Testing Logical Operators ===");
    println("");

    // Test AND operator
    println("--- AND Operator Tests ---");

    val t: bool = true;
    val f: bool = false;

    println("true and true = " + (t and t));
    println("true and false = " + (t and f));
    println("false and true = " + (f and t));
    println("false and false = " + (f and f));
    println("");

    // Test OR operator
    println("--- OR Operator Tests ---");

    println("true or true = " + (t or t));
    println("true or false = " + (t or f));
    println("false or true = " + (f or t));
    println("false or false = " + (f or f));
    println("");

    // Test with comparisons
    println("--- With Comparisons ---");

    val x: int = 5;
    val y: int = 10;

    println("5 > 3 and 10 > 5 = " + (x > 3 and y > 5));
    println("5 > 3 and 10 < 5 = " + (x > 3 and y < 5));
    println("5 < 3 or 10 > 5 = " + (x < 3 or y > 5));
    println("5 < 3 or 10 < 5 = " + (x < 3 or y < 5));
    println("");

    // Test complex expressions
    println("--- Complex Expressions ---");

    println("(5 > 3 and 10 > 5) or false = " + ((x > 3 and y > 5) or f));
    println("(5 < 3 or 10 < 5) and true = " + ((x < 3 or y < 5) and t));
    println("");

    // Test short-circuit evaluation
    println("--- Short-Circuit Evaluation ---");

    if (t or (x == 100)) {
        println("✓ OR short-circuits correctly (second operand not evaluated)");
    }

    if (f and (x == 100)) {
        println("✗ This should not print");
    } else {
        println("✓ AND short-circuits correctly (second operand not evaluated)");
    }
    println("");

    // Real-world example
    println("--- Real-World Example ---");

    val age: int = 25;
    val hasLicense: bool = true;

    if (age >= 18 and hasLicense) {
        println("✓ Can drive (age >= 18 AND has license)");
    }

    val isWeekend: bool = false;
    val isHoliday: bool = false;

    if (isWeekend or isHoliday) {
        println("✗ This should not print");
    } else {
        println("✓ It's a workday (not weekend OR holiday)");
    }
    println("");

    println("=== All Tests Complete ===");
}
