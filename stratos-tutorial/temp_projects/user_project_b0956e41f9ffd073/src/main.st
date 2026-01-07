package main;

fn main() {
    for (i in 1..=20) {
        val output = when {
            i % 15 == 0 -> "FizzBuzz"
            i % 3 == 0 -> "Fizz"
            i % 5 == 0 -> "Buzz"
            else -> i.toString() // Convert int to string
        };
        
        println(output);
    }
}