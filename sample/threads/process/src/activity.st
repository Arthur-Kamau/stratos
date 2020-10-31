package main;

// for you to set up a process
// you have to extends a process class
class MyActivity  extends Process {

  /** 
  * set up you variables or db connection etc
  */
  @overide
  init(variables: <T>){
      
  } 

  /** once the run is done */
  @overide
  function  result() : Stream<int>{

  } 

   @overide
  function destroy(){
      
  } 

  @overide
  run(variables: <T>){

  } 

}