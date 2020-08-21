

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


use of if, just like any c based language.<br>
`if(20>21)`<br>
&emsp; `print("greater")`
`} else {`<br>
&emsp; `print("greater")`
`}`