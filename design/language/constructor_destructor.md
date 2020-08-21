## constructor
The first  function to be called when a called is initialized.
It can be either called `create` or `constructor` but not both
does not have a name
example : <br>

`class School{`<br>
 &emsp; `create(){`<br>
  &emsp; &emsp; `print("default constructor);`<br>
  &emsp; `}`<br/>
`}`<br>

### static constructors
 a static constor can be declared like this

`class School{`<br><br>
 &emsp; `fun School.name(name: string){`<br>
  &emsp; &emsp; `print("custom constructor);`<br>
  &emsp; `}`<br/><br/>
   &emsp; `constructor(){`<br>
  &emsp; &emsp; `print("default constructor);`<br>
  &emsp; `}`<br/><br/>
`}`<br>
