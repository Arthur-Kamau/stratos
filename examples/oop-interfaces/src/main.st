package main;

interface Printable {
    fn print(content: string) void;
    fn getStatus() string;
}

class ConsolePrinter : Printable {
    fn print(content: string) void {
        println("[Console] " + content);
    }
    
    fn getStatus() string {
        return "Ready (Console)";
    }
}

class FancyPrinter : Printable {
    var prefix: string;
    
    constructor(prefix: string) {
        this.prefix = prefix;
    }

    fn print(content: string) void {
        println("*** " + this.prefix + " " + content + " ***");
    }
    
    fn getStatus() string {
        return "Fancy Mode";
    }
}

fn printDocument(p: Printable, doc: string) {
    println("Printer Status: " + p.getStatus());
    p.print(doc);
}

fn main() {
    println("--- OOP Interfaces Demo ---");

    val simple = ConsolePrinter();
    val fancy = FancyPrinter(">>>");

    println("Using ConsolePrinter:");
    printDocument(simple, "Hello World");

    println("\nUsing FancyPrinter:");
    printDocument(fancy, "Hello World");
}

