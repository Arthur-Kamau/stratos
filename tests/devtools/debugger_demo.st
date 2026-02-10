package main;

use log;

// Debugger demo: set breakpoints and step through this code

fn fibonacci(n: int) int {
    if n <= 1 {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

fn factorial(n: int) int {
    var result = 1;
    var i = 2;
    while i <= n {
        result = result * i;
        i = i + 1;
    }
    return result;
}

fn greet(name: string) string {
    val message = "Hello, " + name + "!";
    log.debug("Generated greeting");
    return message;
}

fn main() {
    log.info("Debugger demo starting");

    // Try setting a breakpoint on these lines
    val name = "Stratos";
    val greeting = greet(name);
    println(greeting);

    // Step through the loop
    var i = 1;
    while i <= 5 {
        val fib = fibonacci(i);
        val fact = factorial(i);
        println("fib(" + i + ") = " + fib + ", " + i + "! = " + fact);
        i = i + 1;
    }

    log.info("Debugger demo complete");
}
