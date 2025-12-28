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
    print("Before creating Test");
    val t = Test();
    print("After creating Test");
    print(t.getValue());
}
