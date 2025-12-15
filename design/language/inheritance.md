### Inheritance 

Stratos uses single inheritance for classes.

&emsp; `class shape {`<br>

&emsp; `fn hasAngles() bool {`&emsp;`}`<br>
&emsp; `fn noOfLines() int {`&emsp;`}`<br>
&emsp; `}`<br>

&emsp; `class humans : shape{`<br>
 &emsp;  &emsp; `overide fn hasAngles() bool {`&emsp;`}`<br>
&emsp; `}`<br>


To define a contract of behavior (abstract class or interface), use `interface`.

`interface Animal {`<br>
  &emsp;  `fn makeSound() unit`<br>
`}`


&emsp; `class Lion : Animal {`<br>
&emsp; `    fn makeSound() { print("Roar") }`<br>
&emsp; `}`<br>

To define data structures, use `struct` or `class`.

`struct Point {`<br>
  &emsp;  `x: int,`   
  &emsp;  `y: int`<br>
`}`