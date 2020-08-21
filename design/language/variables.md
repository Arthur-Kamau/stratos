  declare a variable , variables are constants by default and non null by default and private by default.
An example of the various ways you could dclare an integer variable of name myInt.

1. declare a variable of type  int that is non nullable  and non mutable
    `val  myInt : int  = 33` or 
2. declare  a mutable variable of type  int  that is non nullable
    `var  myInt : int ` 
3. declare  nullable and  mutable variable of type  int 
    `let mut  myInt : int`
4.  For quick typing variables that are declared and initialized could be simlified as
      * `let myInt = 33` . The  example is non nullable and immutable interger
    
    
### Take away
1. Let =>  Nullable  types
           To access them neeeed to check if none(`errors.md`).Non mutable by default but can use `mut` keyword to make it mutable
2. Var =>  Non nullbale and mutable.
3. Val => Non nullable and non mutable
4. mut  => used to  make `let` mutable.

## Safety
While accessing/operating/checking nullable you could try any of the follwing.

* Accessing object property use `?.`, `?`
  `myUserObject?.name?`, ie myUserObject could be none if not none get the value of name, name could be also be none if not none get the value.


* checking if null <br/>
`when(my_value){ `<br/>
 &emsp; `none -> { report_err() } `<br/>
 &emsp; `Some -> {some_operation()} `<br/>
`}`
