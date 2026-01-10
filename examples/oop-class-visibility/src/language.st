package main;

class VisibiltyTest {
    fn privateMethod() {
        println("Private method");
    }

    pub fn publicMethod() {
        println("Public method");
    }

        fn privateMethodInternalCall() {
        println("Private method internal call");
    }

    pub fn publicMethod() {
        println("Public method call");
    }

    pub fn callPrivate() {
        this.privateMethod(); // Should work
    }
}
