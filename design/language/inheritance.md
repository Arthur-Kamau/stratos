
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

