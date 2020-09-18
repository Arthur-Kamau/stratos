package main;



// class dclaration
class User{
    // class initialization before  
    // constructor is called
    init {
        print("Class initialization")
    }
    // class destructor  before
    // class is destroyed
    destroy{
        print("Class destruction")
    }

    // constructor 
    function User(){
       print("Class constructo")
    }
}





enum HAND { RIGHT, LEFT  }

struct Person { 
    val hand  :  HAND;
    val  name : string ;
    getName():  string ;
}

// A class can implement a struct
class Student implements Person{
  override getName():  string{
      return "Student"
   }
   function isExpelled():  bool{
      return  true
   }
}
// a class can also extend another class
class Arthur extends Student{
 
  //overide getName method in Student class that is implemented from Person
  override getName():  string{
      return "Arthur"
   }
   //overide isExpelled method in Student class
   override isExpelled():  bool{
      return false
   }
}



// class with constructor 
class Country (val name :String , val population : int){
 override function toString(){
    print("Country name $name with population of ");
  }
}

// a class with getter and setters
data class City (val latitude :double , val height : int, val name : string){
 override function  toString(){
    print("City $name height above sea level $height latitude  ${latitude} ")
  }
}








function man(){

}
