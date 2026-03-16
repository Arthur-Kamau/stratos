// Test class methods and internal calls

package main;

use log;

class Greeter {
    pub var greeting: string;

    constructor(greeting: string) {
        this.greeting = greeting;
    }

    pub fn greet(name: string) string {
        return this.greeting + ", " + name + "!";
    }

    pub fn greetAll(names: array<string>) void {
        for val name in names {
            log.info(this.greet(name));
        }
    }
}

fn main() {
    log.info("=== Method Test 3 ===");

    val g = Greeter("Hello");
    log.info(g.greet("World"));
    log.info(g.greet("Stratos"));

    val names = ["Alice", "Bob", "Charlie"];
    g.greetAll(names);

    log.info("=== Test Complete ===");
}
