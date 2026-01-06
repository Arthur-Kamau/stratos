package main;

fn main() {
    println("Stratos Booleans Example");

    val isAlive = true;
    val isDead = false;

    if isAlive {
        println("It's alive!");
    }

    if !isDead {
        println("It's not dead!");
    }

    val comparison = 5 > 3;
    if comparison {
        println("5 is greater than 3");
    }
    
    println("Boolean value: " + isAlive);
}
