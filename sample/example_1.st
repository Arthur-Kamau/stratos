use "system"

/**
**
* use of namespace, Enum, Class , Constructors (create), Destructors (destroy), Contional (When)
*/
namespace Species{

    Enums Race {
       African , Asian , Europian, American
    }
    class Mammals{

          // create getter and setter for class properties
          data limbs: int;
          data hasHooves: bool;

          /**
           *default constructor
           *  you could als create is ussing
           *    ``` create mamalia(){}```
           *   or
           *    ``` create { }````
           *  args 0
           *  return none
           */
          create(){
            print("constructor")
            // asign limbs
            limbs =4
            // has hooves
            hasHooves = false
          }

          // non default constructor
          create biped(){
             println("Non default  constructot")
          }


           /**
             *  default destructor
             *
             */
          destroy {

          }

    }
}





fn main (args :vargs){
        // gettter for limbs
       val res = Species::Mammals().limbs

       // try and reassign val will throw errri
       // val res = 44  // will cause error

       let mut race  = Race.African

       when (race){
          African -> {
          }
          else ->{
          }
       }




}