package main;

fn main() {
    println("Stratos Conditionals Example");
    
    val x = 5;
    val y = 10;
    
    println("x = " + x);
    println("y = " + y);

    if x < y {
        println("x is less than y");
    } else {
        println("x is not less than y");
    }
    
    if x == 5 {
        println("x is equal to 5");
    }
    
    val age = 18;
    if age >= 18 {
        println("Adult");
    }
}
