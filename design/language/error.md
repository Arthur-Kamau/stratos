### Error syntax
* To promote  type safety  functions/ classes can result in non  nullable and nullable types.

* Nullable types have `let` as the base declaration  while non nullables have `var` and `val` as the base declaration (`variables.md`)

* 


Errors derive from the exception class the error syntax should resemble these :

`try {` <br> 
 &emsp; `let path =  Path("~/me.txt");` <br>
 &emsp; `let file = File(path.absolute);` <br>
  &emsp; `let fileData = file.readAll(); ` <br>
`} onfail e of type FileNotFoundException{` <br>
 &emsp;  `e.printStackTrace();` <br>
`} onfail e of type FilePermissionException{` <br>
 &emsp;  `e.printStackTrace();` <br>
`} catch e of type Exception {`  <br>
&emsp;  `e.printStackTrace();` <br>
`} finally {` <br>
&emsp; `print("done")` <br>
`}`

descrption the  try ctatch logic gets the path as a string and resolve to the os dependent file path for example in linux its `/home/user/me.txt`  then the absolute path is passed into `File` and finally  `readAll` reads the whole file as String the   `readAll` can throw `FileNotFoundException` if the file is not found or `FilePermissionException` if the file does not have access permission.This two expetions are handled uniquly finally using the `catch` key word we capt
ure all other errors.

