package main;

use util;
use name;

// Classes defined in the main package
class Base{
     fn hello(){
         println("base hello");
     }
}

class Core{
     fn hello(){
         println("core hello");
     }
}

// files in src belong to main package
fn main(){

    // use classes defined above in the same file
    val b = Base();
    b.hello();

    // use class Util from util package
    val u = Util();
    u.hello();

    // use class Name from name package
    val n = Name();
    n.helloName();

}