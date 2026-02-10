package main;

use log;

// Performance profiling demo: record a profile to see the flame graph

fn bubbleSort(size: int) {
    // Simulate sorting with nested loops
    var i = 0;
    while i < size {
        var j = 0;
        while j < size - i - 1 {
            j = j + 1;
        }
        i = i + 1;
    }
}

fn computeFibonacci(n: int) int {
    if n <= 1 {
        return n;
    }
    return computeFibonacci(n - 1) + computeFibonacci(n - 2);
}

fn heavyComputation() {
    log.debug("Starting heavy computation");

    // Sorting work
    val sortSize = 100;
    bubbleSort(sortSize);
    log.debug("Sorting complete");

    // Fibonacci work
    val fib = computeFibonacci(20);
    println("fibonacci(20) = " + fib);
    log.debug("Fibonacci complete");
}

fn lightWork() {
    var i = 0;
    while i < 50 {
        i = i + 1;
    }
}

fn main() {
    log.info("Profiler demo starting");
    log.info("Open DevTools Performance tab");
    log.info("Click Record, wait, then Stop to capture a profile");

    // Run CPU-intensive functions
    var round = 1;
    while round <= 3 {
        println("--- Round " + round + " ---");
        heavyComputation();
        lightWork();
        round = round + 1;
    }

    log.info("Profiler demo complete");
    log.info("Check the flame graph and function table in DevTools");
}
