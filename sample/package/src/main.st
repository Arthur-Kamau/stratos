package main;

// import util package 
// and string package
import "./util";



// files in src belong to main package
function main(){ 
    
    // user classes in base.st file wthout import as they belong to the same file
    val u = Base();
    u.hello(); 

    // use class util from util package
    val u = Util();
    u.hello(); 

    // use class Name from util/name package
    val r = Name();
    r.helloName(); 

}