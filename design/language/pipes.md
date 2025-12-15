## Pipes
Get results from one operation and pass them into the next function.<br>
The result of the previous function is passed as the **first argument** of the next function call implicitly.<br>
The result of the last call is returned automatically.

example: <br>
`var total = shape.area() |> plusMe(22)`

This is equivalent to: `plusMe(shape.area(), 22)`

Details <br>
`class shape{`<br/>
 &emsp;`fn area() int{  `<br>
  &emsp; &emsp; `return 50*50` <br>
 &emsp; `}`<br>
`}`

 &emsp;`fn plusMe( area : int, bonus: int ) int{  `<br>
  &emsp; &emsp; `return  area + bonus` <br>
 &emsp; `}`<br>