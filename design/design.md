## Language Design

### Key word

* `String` 
* `string` 
* `Integer` 
* `int` 
* `Float` 
* `float` 
* `bit` 
* `byte` 
* `Character` 
* `char` 
* `unit` 
* `mut` 
* `nill` 
* `list` 
* `set` 
* `array` 
* `tuple` 
* `triple` 
* `map` 
* `true` 
* `false` 
* `not` 
* `in` 
* `function` 
* `fn` 
* `let` 
* `if` 
* `else` 
* `when` 
* `while` 
* `namespace` 
* `class` 
* `enum` 
* `struct` 
* `is` 
* `as` 
* `do` 
* `with` 
* `stream` 
* `null` 
* `try` 
* `type` 
* `catch` 
* `onfail` 
* `finaly` 
* `private` 
* `use` 
* `import` 
* `not` 
* `thread` 
* `process` 
* `task` 
* `?:`
* `:=`

## Use of null.
Since stratos is written in another language it uses `nill` instead of `null`.To prevent the compiler from getting confused.


## Comparison operators
`==`, `!=` , `>=` , `=<` , `<>`,  `<=>` ,`<!>`, `<!=>` <br>
* `==` equality operator
* `!=` not equal operator
* `>=` greater than or equal operator
* `=<` less than or equal operator
* `<>` between the left and and right hand
* `<=>` between the left and and right hand inclusive range variables
* `<!=>`  not between the left and and right hand inclusive of the range variables

### Logical operators
`&&` , `||`,  `not` ,`~`, `|` , `&` ,`!`
* `&&`  logical and
* `||`  logical or
* `not`  and `!` logical not 
* `~`  bit wise not
* `|`  bit wise or
* `&`  bit wise and

### Other oprators
`+` , `-`, `/` , `*`, `mod` or `%`

* `+` - addition.
* `-` - subtraction.
* `/` - division.
* `*` - mutliply (compound).
* `mod` or `%` - remainder of (modulus).

`+=`, `++`, `--` , `*=` `/=` 
* `+=` -  incrementing the variable on the left by an arbitrary value on the right
* `++` - plus one
* `--` - minus one
* `*=`compund the variable on the left by an arbitrary value on the right
* `/=`divide the variable on the left by an arbitrary value on the right

### Data types

The basic data types ie `int` , `string` , `float` , `char`  
the corresponding data type has name starting with an uppercase letter with complete name ie `int` -> `Integer` the second is used for type conversion and hard cast 

More complex data types like `map`, `tuple`, `triple`, `array`,`list` and `set` do not have primary and complex types an all   complex data types except `tuple`, `triple` are iterable(ie you can iterate over  `map`,`array`,`list` and `set`).

tuple store only two values .<br>
`let mut ageAndName : Tuple<int, string> = Tuple(22,"victoria");`<br>
`.... //code `<br>
`print("the user is ${ageAndName.first}  and the user age is ${ageAndName.second}")`<br>
`... // more code`<br><br>
triple store only three values .The access methods are the same as tuple but with a third type accessor.<br>
`let mut ageAndNameAndHeightInCm : Triple<int, string,int> = Triple(22,"david", 152);`


### The use of ; 

the plan in the first version the  parser will need the `;` during tokenization.
Ps. Stratos does not have new.

### Basic language syntax

*   declare a variable , variables are constants by default and non null by default and private by default.
An example of the various ways you could dclare an integer variable of name myInt.
    1. declare a variable of type  int ,
    `let  myInt : int ` or 
    2. declare  a mutable variable of type  int ,
    `let mut  myInt : int ` 
    3. declare nullable  mutable variable of type  int 
    `let mut  myInt : int? `
    4. declare a public nullable  mutable variable of type  int  `public let mut  myInt : int? `
    5. For quick typing variables that are declared and initialized could be simlified as
      `let myInt := 33` .
      Also under consideration is the following synatax.Remeber variables are immutable and non nullable by default.
      `let myInt ? := 33`.-> declaring a nullable int. 
      `let mut myInt ? := 33`.-> declaring a mutable  nullable int. 


*  function declarations 
    1.  declare a function (functions with no return type have a return type of type void) or you could set it explicitly remember `fn == function`, example : <br>
`fn function_name(arg: string) : unit{ }`  <br><br>

    2.  declare a  function with named paratmeters  and return type of string `fn function_name({arg: string}) : string{ }` <br><br>

    3.  declare a function with optional parameters and return type of int `fn function_name([arg: string]) : int{ }` <br><br>
    
    4.  at a mature level you could write this as valid syntax ie a function with 2 named parameters args1 and args2, args1 is mandatory and args2 is optional
    `fn function_name(  { args1 : string,[arg2: string]}) : Pair<int, string>{ }` 

      finally you could call the function  using number 4 above with named args1 as manadatory and args 2 as optional
`function_name(args1="name1",args2="name1")` 

* ternary oprator. 
   since `?` is used for null safty ie a variable with `?` can be null, we could have an automatic return statement in an if like kotlin, which is inpired by functional programming,however i consider this esotoric,as Object oriented dominats software development its wise to have a smooth transition for user from other languages.
   The use of `?:` looks more appealing plus less typing.
   `Avoid  EXAMPLE`
   `....`
   `let age = 21 `
   `let isAdult = if( age>18 ) { "you are an adult " }else{ "You ar underaged}` 
   `print(isAdult);`
   `.....`

   `Prefer `
 `....`
   `let age = 21 `
   `let isAdult =  age>18 ?: "you are an adult " : "You ar underaged ;` 
   `print(isAdult);`
   `.....`

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

### Error syntax

Errors derive from the exception class the error syntax should resemble these :

`try {` <br> 
 &emsp; `let path =  Path("~/me.txt");` <br>
 &emsp; `let file = File(path.absolute);` <br>
  &emsp; `let fileData = file.readAll(); ` <br>
`} onfail e of type FileNotFoundException{` <br>
 &emsp;  `e.printStackTrace();` <br>
`} onfail e of type FilePermissionException{` <br>
 &emsp;  `e.printStackTrace();` <br>
`} catch e of type Exception {`  <br>
&emsp;  `e.printStackTrace();` <br>
`} finally {` <br>
&emsp; `print("done")` <br>
`}`

descrption the  try ctatch logic gets the path as a string and resolve to the os dependent file path for example in linux its `/home/user/me.txt`  then the absolute path is passed into `File` and finally  `readAll` reads the whole file as String the   `readAll` can throw `FileNotFoundException` if the file is not found or `FilePermissionException` if the file does not have access permission.This two expetions are handled uniquly finally using the `catch` key word we capt
ure all other errors.


### loops
* using for in loops

  `for (let name in ["allan","mathew","rachael"]){`<br>
  &emsp; `println("name $name");`<br>
  `}`<br>

  or for loop with index

  `for (let (name, index) in ["allan","mathew","rachael"].withIndex){`<br>
  &emsp; `println("name $name at index $index");`<br>
  `}`<br>


* using for with incrimental
  `for (let size=0;size== ["allan","mathew","rachael"].size,size++){`<br>
  &emsp; `println("name $name");`<br>
  `}`<br>


* using do while

  `do{`<br>
  &emsp; `println("name $name");`<br>
  `}while(let name in ["allan","mathew","rachael"] )`


### conditional

`let name="allan";`<br>
 &emsp;  `when(name){`<br>
  &emsp; &emsp;`"mary" -> {`<br>
  &emsp; &emsp;&emsp;&emsp;`print("name is mary");`<br>
  &emsp; &emsp;`}`<br>
  &emsp; &emsp;`!"allan" -> {`<br>
    &emsp; &emsp;&emsp;&emsp;`print("name is not allan");`<br>
  &emsp; &emsp;`}`<br>
    &emsp; &emsp;`name == "hussein" || name == "rashid" -> {`<br>
    &emsp; &emsp;&emsp;&emsp;`print("name is husrashidsein or ");`<br>
  &emsp; &emsp;`}`<br>
    &emsp; &emsp;`name is int -> {`<br>
    &emsp; &emsp;&emsp;&emsp;`print("name is integer");`<br>
  &emsp; &emsp;`}`<br>
    &emsp; &emsp;` else -> {`<br>
    &emsp; &emsp;&emsp;&emsp;`print("name is john");`<br>
  &emsp; &emsp;`}`<br>
  &emsp;`}`<br>

or  onother example is the use of with, with is used on the succes of  an expression or do this if that fails excute the else statement 

`let name="allan";`<br>
`with name as  int do {`<br>
 &emsp;`print("name can be an intger");`<br>
`} else { `<br>
&emsp;`print(" name cannot be cast to an intgeer");`<br>
`   }`


### Inheritance.

Stratos uses single inheritance.

To use abstract class or interface use struct instead.

`enum speciesTypes { m,4,3 }`

`struct animal {`<br>
  &emsp;  `age: int,`   
  &emsp;  `species: speciesTypes,`<br>
  &emsp;  `isMale: bool`<br>
`}`

`namespace mamals{`<br>
&emsp; `class humans : animal{`<br>
&emsp; `}`<br>
&emsp; `class lions : animal{`<br>
&emsp; `}`<br>
`}`


---
### Pointers

pointers are a bone of contetion and are still under evaluation.


---
The language syntax borrows from  these languages `java` , `kotlin`, `dart`, `c` , `c#` , `rust` and `elixir`.