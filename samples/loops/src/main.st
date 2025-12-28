package main;

// Simple while loop counting from 1 to 5
fn simpleWhileLoop() {
    var i = 1;
    while (i <= 5) {
        print(i);
        i = i + 1;
    }
}

// Count down from 10 to 1
fn countDown() {
    var count = 10;
    while (count > 0) {
        print(count);
        count = count - 1;
    }
    print("Liftoff!");
}

// Sum numbers from 1 to 10
fn sumNumbers() {
    var sum = 0;
    var i = 1;
    while (i <= 10) {
        sum = sum + i;
        i = i + 1;
    }
    print("Sum of 1 to 10:");
    print(sum);
}

// Find maximum in a series of numbers
fn findMax() {
    var max = 0;
    var i = 1;

    while (i <= 5) {
        var value = i * i;  // Square of i
        print(value);

        if (value > max) {
            max = value;
        }
        i = i + 1;
    }

    print("Maximum value:");
    print(max);
}

// Nested loops - multiplication table (3x3)
fn nestedLoops() {
    var i = 1;
    while (i <= 3) {
        var j = 1;
        while (j <= 3) {
            var product = i * j;
            print(product);
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
        print(value);
    }

    print("Reached 100 in iterations:");
    print(iterations);
}

// Main function - runs all loop examples
fn main() {
    print("=== Simple While Loop ===");
    simpleWhileLoop();

    print("");
    print("=== Count Down ===");
    countDown();

    print("");
    print("=== Sum Numbers ===");
    sumNumbers();

    print("");
    print("=== Find Maximum ===");
    findMax();

    print("");
    print("=== Nested Loops ===");
    nestedLoops();

    print("");
    print("=== Loop with Condition ===");
    loopWithCondition();
}
