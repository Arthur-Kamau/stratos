package main;

use util;
use animal;
use animal.cat;

// files in src belong to main package
fn main(){

    // use classes defined above in the same file
    val b = Base();
    b.hello();

    // use class Util from util package
    val u = Util();
    u.hello();

    // use class Animolia from animal package
    val n = Animolia();
    n.helloAnimolia();

    // use struct BirmanCat from cat package
    val k = BirmanCat{name: "Lola", age: 10};
    println("The Birman cat name: {} age: {}", k.name, k.age);


}