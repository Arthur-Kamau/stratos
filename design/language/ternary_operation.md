## Ternary Logic (If Expression)
   
Stratos uses `if` expressions instead of a dedicated ternary operator like `? :`. This avoids conflict with the null-coalescing (Elvis) operator `?:` and improves readability.

`val value = if (expression) value_a else value_b;`<br>
Example <br/>

   `val isAdult =  if (age > 18) "You are an adult" else "You are underage";` <br>
   `print(isAdult);`