package main;

/**
*  types =>  int, number, double , string , bool, char
*  
*/

function main(){

   // mutable  and nullable 
   var name_first  = "kamau" ;
   //  mutable  and nullable  with explicit type
   var name_second : String  = "kamau" ;
   // mutable  and nullable  with explicit type string literal 
   var name_second : String  = """ 
       This
       is 
       a 
       literal 
   
   """ ;
   
   // imutable  
    val letter : Char = 'v'  // ensure single quote

   // imutable and nullable
   val userAge  = SomeFunction(); 
   //  imutable  and nullable  with explicit type
   val userHeight : int  = 145 ;

   // non nullable  
   let userLat  = 12.333 ; 
   // non nullable    with explicit type
   let userLong : double = 12.333 ; 
   //  non nullable 
   let  mutable userAngle : double = 12.333 ; 
}