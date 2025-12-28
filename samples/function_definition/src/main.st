package main ;


// function without parameters
fn hello(){
   print("hello world");
}

fn nn() {

}

// function with parameters
fn printName(name: String){
   print("hello");
   print(name);
}

// function with multiple  parameters
fn printNameAndIsMale(name: String, isMale: bool){
   print("Hello");
   print(name);
   print("are you male");
   print(isMale);
}

// function with return value
fn returnAge(name: String) int {
   return 26;
}

// function with parameters and return type
fn printHelloNameAndReturnHeight(name: String) int {
   print("hello");
   print(name);
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
   print("my age:");
   print(me);

   // parameter and return
   val h = printHelloNameAndReturnHeight("Arthur");
   print("my height:");
   print(h);
}