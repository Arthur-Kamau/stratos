// Test panic() function - should terminate program

fn main() {
    println("About to panic...");
    panic("This is a test panic!");
    println("This should never print");
}
