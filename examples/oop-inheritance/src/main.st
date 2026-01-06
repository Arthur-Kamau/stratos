package main;

class Animal {
    var name: string;

    constructor(name: string) {
        this.name = name;
    }

    fn makeSound() void {
        println(this.name + " makes a generic sound.");
    }
    
    fn sleep() void {
        println(this.name + " is sleeping.");
    }
}

class Dog : Animal {
    var breed: string;

    constructor(name: string, breed: string) {
        this.name = name; 
        this.breed = breed;
    }

    fn makeSound() void {
        println(this.name + " (" + this.breed + ") says: Woof!");
    }
    
    fn fetch() void {
        println(this.name + " runs to fetch the stick.");
    }

    // Manual override/implementation required if inheritance of methods isn't fully supported
    fn sleep() void {
        println(this.name + " (Dog) is sleeping.");
    }
}

class Cat : Animal {
    constructor(name: string) {
        this.name = name;
    }

    fn makeSound() void {
        println(this.name + " says: Meow!");
    }

    fn sleep() void {
        println(this.name + " (Cat) is sleeping.");
    }
}

fn main() {
    println("--- OOP Inheritance Demo ---");

    val myDog = Dog("Buddy", "Golden Retriever");
    val myCat = Cat("Whiskers");

    myDog.makeSound();
    myDog.sleep();
    myDog.fetch();

    println("");

    myCat.makeSound();
    myCat.sleep();
}