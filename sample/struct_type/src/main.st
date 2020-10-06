package main;



struct User {
   var age : int;
   val name : string ;
   // declare default value 
   let isAudible = true; 
   // function return a string 
    getNationality() :string ;
    // function return a nothing 
    isAdult() : unit ;
}
// a class can impliment a struct
class student implements User {
   
   
   val age = 22;
   val name = "User";

   override getNationality():  string{
      return "kenya"
   }

    override isAdult(){
       // evluates to true
      val isAdult  age > 18;
      // print is adult
      print(" is the user adult $isAdult ")

   }
}

function main(){ 
 
   let me = User {
      age = 20,
      name = "Alex"
   }
   // should print name and defailt value
   print("User name is ${me.name}  is audible ${me.isAudible}") 



}