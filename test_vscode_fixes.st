package main;

// NOTE: Intentionally missing "use io;" and "use maps;" to test import suggestions

fn main() {
    // Test Issue #1: Should suggest adding "use io;"
    io.writeFile("test.txt", "content");

    // Test Issue #2: Error range should underline entire identifier
    val x = undefined_identifier_test;

    // Test Issue #3: maps now requires import like all other modules
    val userMap = maps.create();
}
