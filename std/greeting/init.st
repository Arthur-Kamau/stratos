// Greeting module - Simple test module for pure Stratos std library
package greeting;

fn hello(name: string) string {
    return "Hello, " + name + "!";
}

fn goodbye(name: string) string {
    return "Goodbye, " + name + "!";
}

fn greet() string {
    return "Hello from pure Stratos standard library!";
}
