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



