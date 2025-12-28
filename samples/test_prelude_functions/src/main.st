// Test new prelude functions: panic(), assert(), dbg()

fn main() {
    println("=== Testing New Prelude Functions ===\n");

    // Test 1: dbg() - Debug print with value inspection
    println("Test 1: dbg() function");
    val x = 42;
    dbg(x);  // Should print: [DEBUG] int = 42

    val name = "Stratos";
    dbg(name);  // Should print: [DEBUG] string = "Stratos"

    val pi = 3.14159;
    dbg(pi);  // Should print: [DEBUG] double = 3.14159

    val isTrue = true;
    dbg(isTrue);  // Should print: [DEBUG] bool = true

    println("");

    // Test 2: assert() - Runtime assertions
    println("Test 2: assert() function");

    assert(true, "This should pass");
    println("✓ assert(true) passed");

    assert(1, "Non-zero should pass");
    println("✓ assert(1) passed");

    assert(42, "Positive number should pass");
    println("✓ assert(42) passed");

    val result = 10 + 5;
    assert(result > 10, "Result should be greater than 10");
    println("✓ assert(result > 10) passed");

    println("");

    // Test 3: Demonstrate dbg() with expressions
    println("Test 3: dbg() with expressions");
    val sum = dbg(10 + 20);  // Should print debug info and return value
    println("Sum is: " + sum);

    println("\n=== All tests passed! ===");
    println("\nNote: To test panic() and failing assertions,");
    println("uncomment the code below and run separately.");

    // Uncomment to test panic:
    // panic("This is a panic test");

    // Uncomment to test failing assertion:
    // assert(false, "This assertion should fail");

    // Uncomment to test assert with 0:
    // assert(0, "Zero should fail");
}
