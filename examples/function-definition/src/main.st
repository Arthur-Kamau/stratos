package main ;


// function without parameters
fn hello(){
   println("hello world");
}

fn nn() {

}

// function with parameters
fn printName(name: String){
   println("hello");
   println(name);
}

// function with multiple  parameters
fn printNameAndIsMale(name: String, isMale: bool){
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

fn main(){
   // call function
   hello();

   // parameter
   printName("Arthur");

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