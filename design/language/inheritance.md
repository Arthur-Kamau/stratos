
### Inheritance 

Stratos uses single inheritance.

&emsp; `class shape {`<br>

&emsp; `fn hasAngles() bool {`&emsp;`}`<br>
&emsp; `fn noOfLines() int {`&emsp;`}`<br>
&emsp; `}`<br>

&emsp; `class humans : shape{`<br>
 &emsp;  &emsp; `overide fn hasAngles() bool {`&emsp;`}`<br>
&emsp; `}`<br>


To use abstract class or interface use struct instead.

`struct animal {`<br>
  &emsp;  `age: int,`   
  &emsp;  `isMale: bool`<br>
`}`


&emsp; `class humans : animal{`<br>
&emsp; `}`<br>
&emsp; `class lions : animal{`<br>
&emsp; `}`<br>


