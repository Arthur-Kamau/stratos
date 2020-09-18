package main;

// function without parameters
function hello(){
   print ("hello world ")
}
function nn() {
  
}

// function with parameters
function printName(val name : String){
   print("hello $name  ")
}

// function with multiple  parameters
function printNameAndIsMale(val name : String, val isMale : bool ){
   print ("Hello $name are you male $isMale  ")
}

// function with parameters
function returnAge(val name : String){
      return 26
}

// function with parameters and return type
function printHelloNameAndReturnHight(val name : String ) int {
   print("hello $name  ")
   return 170
}

function m(){
   // call function
   hello();
   // parameter 
   printName("Arthur");
   // multiple parameters
   printNameAndIsMale(name="Arthur", isMale=true);
   // return 
   val me = returnAge();
   print("my age $age");
   // parameter and return 
   val h : int = returnAge();
   print("my height $h");
}