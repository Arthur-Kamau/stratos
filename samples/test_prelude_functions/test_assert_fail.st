// Test assert() failure - should terminate program

fn main() {
    println("Testing assertion failure...");
    assert(false, "Expected assertion failure");
    println("This should never print");
}
