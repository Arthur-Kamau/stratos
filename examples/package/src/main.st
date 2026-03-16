package main;

use util;
use animal;

class Base {
    pub fn hello() {
        println("base hello");
    }
}

fn main(){
    // use classes defined in same file
    val b = Base();
    b.hello();

    // use class Util from util package
    val u = Util();
    u.hello();

    // use class Animolia from animal package
    val n = Animolia();
    n.helloAnimolia();

    println("Package imports working!");
}
