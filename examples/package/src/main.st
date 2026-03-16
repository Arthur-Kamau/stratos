package main;

use util;
use animal;

// files in src belong to main package
fn main(){

    // use classes defined in base.st (same package)
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
