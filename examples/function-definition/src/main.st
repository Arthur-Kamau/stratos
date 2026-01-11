package main;

use collections;

// function without parameters
fn hello() {
   println("hello world");
}

fn nn() {
    // empty function
}

// function with string parameter
fn printName(name: string) {
   println("hello");
   println(name);
}

// function with int parameter
fn printNumber(num: int) {
    println("Number is:");
    println(num);
}

// function with multiple parameters
fn printNameAndIsMale(name: string, isMale: bool) {
   println("Hello");
   println(name);
   println("are you male");
   println(isMale);
}

// function with return value
fn returnAge(name: string) int {
   return 26;
}

// function with parameters and return type
fn printHelloNameAndReturnHeight(name: string) int {
   println("hello");
   println(name);
   return 170;
}


fn jumbo(people: Map<string, int> = newMap<string, int>()) {
   println("People:");
   people.forEach((name, age) => {
      println(name);
      println(age);
   });
}
fn main() {
   println("--- Function Examples ---");

   // call function
   hello();

   // parameter (string)
   printName("Arthur");
   
   // parameter (int)
   printNumber(42);

   // multiple parameters
   printNameAndIsMale("Arthur", true);

   // return
   val me = returnAge("Arthur");
   println("my age:");
   println(me);

   // parameter and return
   val h = printHelloNameAndReturnHeight("Arthur");
   println("my height:");
   println(h);
}
