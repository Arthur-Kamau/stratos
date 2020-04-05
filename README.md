# Stratos.

Trying to build a compiler and an interpretter.

The language priorities are fast and fun to write.

The tutorial is in cSharp 💓. Quickly prototype ideas in Kotlin.
The language in written in C 💞.


There are few bsics which have not been concluded.
1) Will use gcc or clang(llvm yess.).
2) Whether it will be  vm(Virtual Machine) based language .

This project intends to  enable its creators make mistakes and learn from them 😘.

hopefully one day Stratos will be built in stratos.(dogfeeding)


###  The plan.

This is a pretty young project.The plan is to build a lexer and a parser.
The idea is to rely on the underlying infrastructure be it llvm ir or ocamel.



### Workplan .
This is a wip (work in progress).
currently designing the language. 

### Language basic
compile to binary or genrate bytecode to run in an intergpreter
File extensions are `.st` or `.sts`<br>
* `.st` files are for compilation
* `.sts` files are for intrepration



however passed in compiler arguments overide the file extension type inference. 

### Design
check the  [design.md ](stratos/design/design.md) {`location /stratos/design/design.md`} file for the language design.
<br>
<br>

### Tooling
tooling there will be a cli tool called `toolman` thats the package manager.
(inspired by go in golang and cargo in rust). To get packages build and run the language

packages can be stored in github or gitlab.

*   to get packages just run 
`toolman get git@my-stratos-package`
*   to build the project run (in project directory)
`toolman build`
*   to build the project run (in project directory)
`toolman run`
*   to create a project run `toolman create  project_name` 

### Projects directory
projects can be located anywhere.
the project has
1. `.ini` file describing the project , project name, dependancies and verisons.
2. a `src` folder with `main.st` with hello world boiler plate code ie :

    `use System;` <br> 
    `fn main (){` <br> 
    &emsp; `print("hello world");` <br>
    `}` 

   To know more read the `stratos/design/design.md` file for the language design.



---
### How to contribute.
You are welcome to contribute.
make sure your commits are atomic and well commented.

*   git pull `git@github.com:Arthur-Kamau/stratos.git`
*   git checkout -b `branch name with feature`
*   `your changes`
*   git add .
*   git commit -m `your changes`
*   git push


