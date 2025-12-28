package main;

// Simple while loop counting from 1 to 5
fn simpleWhileLoop() {
    var i = 1;
    while (i <= 5) {
        println(i);
        i = i + 1;
    }
}

// Count down from 10 to 1
fn countDown() {
    var count = 10;
    while (count > 0) {
        println(count);
        count = count - 1;
    }
    println("Liftoff!");
}

// Sum numbers from 1 to 10
fn sumNumbers() {
    var sum = 0;
    var i = 1;
    while (i <= 10) {
        sum = sum + i;
        i = i + 1;
    }
    println("Sum of 1 to 10:");
    println(sum);
}

// Find maximum in a series of numbers
fn findMax() {
    var max = 0;
    var i = 1;

    while (i <= 5) {
        var value = i * i;  // Square of i
        println(value);

        if (value > max) {
            max = value;
        }
        i = i + 1;
    }

    println("Maximum value:");
    println(max);
}

// Nested loops - multiplication table (3x3)
fn nestedLoops() {
    var i = 1;
    while (i <= 3) {
        var j = 1;
        while (j <= 3) {
            var product = i * j;
            println(product);
            j = j + 1;
        }
        i = i + 1;
    }
}

// Loop until condition is met
fn loopWithCondition() {
    var value = 2;
    var iterations = 0;

    while (value < 100) {
        value = value * 2;  // Double each time
        iterations = iterations + 1;
        println(value);
    }

    println("Reached 100 in iterations:");
    println(iterations);
}

// Main function - runs all loop examples
fn main() {
    println("=== Simple While Loop ===");
    simpleWhileLoop();

    println("");
    println("=== Count Down ===");
    countDown();

    println("");
    println("=== Sum Numbers ===");
    sumNumbers();

    println("");
    println("=== Find Maximum ===");
    findMax();

    println("");
    println("=== Nested Loops ===");
    nestedLoops();

    println("");
    println("=== Loop with Condition ===");
    loopWithCondition();
}
