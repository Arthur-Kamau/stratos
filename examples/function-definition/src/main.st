package main;

// function without parameters
fn hello() {
   println("hello world");
}

fn nn() {
    // empty function
}

// function with string parameter
fn printName(name: String) {
   println("hello");
   println(name);
}

// function with int parameter
fn printNumber(num: int) {
    println("Number is:");
    println(num);
}

// function with multiple parameters
fn printNameAndIsMale(name: String, isMale: bool) {
   println("Hello");
   println(name);
   println("are you male");
   println(isMale);
}

// function with return value
fn returnAge(name: String) int {
   return 26;
}

// function with parameters and return type
fn printHelloNameAndReturnHeight(name: String) int {
   println("hello");
   println(name);
   return 170;
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
