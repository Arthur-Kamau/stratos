package main;

class Test {
    var value: double;

    constructor() {
        this.value = 42.0;
    }

    fn getValue() double {
        return this.value;
    }
}

fn main() {
    println("Before creating Test");
    val t = Test();
    println("After creating Test");
    println(t.getValue());
}
