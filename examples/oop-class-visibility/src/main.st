package main;

use log;


fn main() {
    log.info("Hello from oop-class-visibility!");

     val t = VisibiltyTest();
    t.publicMethod(); 
     println("=== The following should fail ===");
    t.callPrivate(); 
    println("Success!");
   
}
