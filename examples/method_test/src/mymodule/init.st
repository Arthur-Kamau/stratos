// Simple module to test method resolution

package mymodule;

// A simple class with methods
class Counter {
    pub var count: int = 0;

    pub fn increment() {
        this.count = this.count + 1;
    }

    pub fn decrement() {
        this.count = this.count - 1;
    }

    pub fn getValue() int {
        return this.count;
    }

    pub fn reset() {
        this.count = 0;
    }
}

// Factory function to create a Counter
pub fn newCounter() Counter {
    return Counter { count: 0 };
}

// Another class to test method chaining
class Builder {
    pub var value: string = "";

    pub fn append(s: string) Builder {
        this.value = this.value + s;
        return this;
    }

    pub fn build() string {
        return this.value;
    }
}

pub fn newBuilder() Builder {
    return Builder { value: "" };
}
