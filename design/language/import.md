
### Difference between import and use

Import is for a class while use is for a namespace.
The difference is that the compiler at compile time should resolve all the classes in the namespace and discard all those that are not in use.
This is target to avoid uneccessary imports example

`use System;` <br> 
`fn main (){` <br> 
  &emsp; `print("hello");` <br>
  &emsp; `println("hello");` <br>
`}` 

could compile to an intermediate form of

`import System.Print;` <br> 
`import System.Println;` <br> 
`fn main (){` <br> 
  &emsp; `print("hello");` <br>
  &emsp; `println("hello");` <br>
`}` 

the aim is to reduce the imports and verbosity.By importing a namespace you import all the classes however at run time only the needed import are used if you write the second syntax by hand all is well.

