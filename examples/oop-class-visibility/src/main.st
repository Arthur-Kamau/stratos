package main;

use log;

class VisibilityTest {
    fn privateMethod() {
        println("Private method");
    }

    pub fn publicMethod() {
        println("Public method");
    }

    pub fn callPrivate() {
        this.privateMethod();
    }
}

fn main() {
    log.info("Hello from oop-class-visibility!");

    val t = VisibilityTest();
    t.publicMethod();
    println("=== The following should fail ===");
    t.callPrivate();
    println("Success!");
}
