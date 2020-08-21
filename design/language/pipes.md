## Pipes
Get results from one operation and pass them into the next function.<br>
The result of the previous function is passed as the first argument of the next function call.<br>
The result of the last call is return automatically 

example: <br>
`var toatl =shape.area() |> plusMe(&1) ;`

Details <br>
`class shape{`<br/>
 &emsp;`fn area() int{  `<br>
  &emsp; &emsp; `return 50*50` <br>
 &emsp; `}`<br>
`}`

 &emsp;`fn plusMe( area : int ) int{  `<br>
  &emsp; &emsp; `return  area+22` <br>
 &emsp; `}`<br>



