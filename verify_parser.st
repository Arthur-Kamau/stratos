package test;

class TestClass<T> {
    pub fn genericMethod<R>(val: R) void {
        println("generic method");
    }
}

fn main() {
    println("TestClass parsed?");
}
