// 01_basics.st
// Basic variable declarations and arithmetic

val pi: double = 3.14159;
var counter: int = 0;

// Type inference
val message = "Hello, Stratos";
var isRunning = true;

// Mutable assignment
counter = counter + 1;
counter += 5;

// Block scope
{
    val temp = 100;
    var result = temp * 2;
}
