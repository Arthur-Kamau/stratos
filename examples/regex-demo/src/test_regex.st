package main;

use regex;

fn main() {
    println("Testing regex module...");
    val result = regex.matches("[a-z]+", "hello");
    if result {
        println("Match found!");
    } else {
        println("No match");
    }
}
