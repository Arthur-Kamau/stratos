// 08_object_alloc.st
// Test object allocation and method calls

class Counter {
    var count: int;

    constructor(start: int) {
        this.count = start;
    }

    fn increment() {
        this.count = this.count + 1;
    }

    fn get() int {
        return this.count;
    }
}

fn main() {
    val c = Counter(10);
    c.increment();
    print(c.get()); // Expected: 11
}
