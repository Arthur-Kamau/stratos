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
      // mutable  and nullable
    var letter : Char = 'v'  // ensure single quote

   // imutable and nullable
   var userAge  = SomeFunction(); 
   //  imutable  and nullable  with explicit type
   var userHeight : int  = 145 ;

   // non nullable and imutable 
   var userLat  = 12.333 ; 
   // non nullable and imutable   with explicit type
   var userLong : double = 12.333 ; 
   //  non nullable and mutable
    var mutable userAngle : double = 12.333 ; 
}