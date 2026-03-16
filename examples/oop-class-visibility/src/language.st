package main;

class VisibilityTest {
    fn privateMethod() {
        println("Private method");
    }

    pub fn publicMethod() {
        println("Public method");
    }

    fn privateMethodInternalCall() {
        println("Private method internal call");
    }

    pub fn callPrivate() {
        this.privateMethod(); // Should work
    }
}
