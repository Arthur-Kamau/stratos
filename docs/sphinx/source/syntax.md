
## Syntax 

Stratos Syntax is closely related to Kotlin/C# and Java.


### A. comments
comments could be:
1. Multi line comment.<br/>
   This encapsulate everthing between the start of the comment and thge end of the comment ie `/*` start of the comment and `*/` end of the comment.
2. Single line comments.<br/>
   Single line comment start with `//` upto the line

### B. Types
the language basic data types are;
1.  strings.
2.  characters
3.  double
4.  integer/ number 

### C. Functions
      

### D. classes
       

### E. package 
 

### F. structs
  

### G. Enums
       

### H. Variables
      

for an example check `/sample/variables` project

### I. Conditions
      
1. `If`, `Else` Conditions

2. `When` Condition

### J loops
       
There are two main types of loops

 1. `for` loop(commonly called foreach loop)
 2. `loop` 

a for loop is used to loop within ranges example<br>
`for(x in a..z){`<br>`
`&emsp;&emsp;`print(x);`<br>
&emsp;&emsp;`// prints a , b ,c to z `<br>
`}`
<br> or <br>
`for(x in 1..100){`<br>`
`&emsp;&emsp;`print(x);`<br>`
`&emsp;&emsp;`// prints 1 to 100 `<br>`
}`

a loop without paramters equates to a while true loop ie `loop{ print("hello")}`  will print hello forever.

looping through a list is eassy using loop.Using the keyword `it` to get the current index and the current value 
example
`
var mList = [1,2,3,4]`<br>`
loop mList{`<br>`
   `&emsp;&emsp;`val  value =it.value        `<br>`
   `&emsp;&emsp;`val  index =it.index        `<br>`
   `&emsp;&emsp;`print (" at index $index the value is $value ")     ;  `<br>`
}`

for an example check `/sample/loops` project


    

 