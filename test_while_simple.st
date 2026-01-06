package main;

fn main() void {
    println("Test 1: Simple while");
    var i = 1;
    while i <= 3 {
        println("  i = " + i);
        i = i + 1;
    }
    println("Done with test 1");

    println("\nTest 2: Multiplication");
    var value = 2;
    println("  Initial value: " + value);
    value = value * 2;
    println("  After * 2: " + value);
    value = value * 2;
    println("  After * 2 again: " + value);

    println("\nTest 3: While with multiplication");
    var num = 2;
    var count = 0;
    while num < 20 {
        num = num * 2;
        count = count + 1;
        println("  Iteration " + count + ": " + num);
    }
    println("Final: " + num);
}
