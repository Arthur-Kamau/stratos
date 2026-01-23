// Simple module to test method resolution

package mymodule;

// A simple class with methods
class Counter {
    pub var count: int;

    pub fn increment() void;
    pub fn decrement() void;
    pub fn getValue() int;
    pub fn reset() void;
}

// Factory function to create a Counter
fn newCounter() Counter;

// Another class to test method chaining
class Builder {
    pub var value: string;

    pub fn append(s: string) Builder;
    pub fn build() string;
}

fn newBuilder() Builder;
