# Stratos.

<img src="./icons/images.jpg"/>


The language priorities are fast and fun to write.

## objective

* fast
* lowlevel
* mix of functional and oop.
* flexible.
* compiled language.
* Ability to generate static binaries.



## Status.
Status In Active development
Plan 
    * Focus now is in tooling.
    * Parser.
    * Lexer.
    * Stratos IR.
    * Stratos WorkMan Tool.
    * Priting the front end to CPP.
    * Basic standard





### Workplan .
This is a wip (work in progress).

* Designing the language.(The basics are done)  check out
     [design.md ](stratos/design/design.md) {`location /stratos/design/design.md`}
     [compiler_design_thoughts.md ](stratos/design/compiler_design_thoughts.md) {`location /stratos/design/compiler_design_thoughts.md`}
* Writing a Parser and a Lexer. (starting off)
* Sematic and syntax analysis (coming soon).
* Human Intermediate Representation (HIR) (We will get there soon )


### Language basic
compile to binary or genrate bytecode to run in an intergpreter
File extensions are `.st` or `.sts`<br>
* `.st` files are for compilation
* `.sts` files are for intrepration



however passed in compiler arguments overide the file extension type inference. 

### Design
* check the  [design.md ](stratos/design/design.md) {`location /stratos/design/design.md`} file for the language design.

* [compiler_design_thoughts.md ](stratos/design/compiler_design_thoughts.md) {`location /stratos/design/compiler_design_thoughts.md`} file for compiler design thoughts.

* [standard_library_thoughts.md ](stratos/design/compiler_design_thoughts.md) {`location /stratos/design/standard_library_thoughts.md`} file for the standard library design thoughts.

* [tooling.md ](stratos/tooling/tooling.md) {`location /stratos/tooling/tooling.md`} file for the  ide & code editor (Jetbrains and Vs code),ie Syntax highlight, tooling 

<br>
<br>

### Tooling
tooling there will be a cli tool called `toolman` thats the package manager.
(inspired by go in golang and cargo in rust). To get packages build and run the language

packages can be stored in github or gitlab.

*   to get packages just run 
`tool get git@my-stratos-package`
*   to build the project run (in project directory)
`tool build`
*   to build the project run (in project directory)
`tool run`
*   to create a project run `tool create  project_name` 

### Projects directory
projects can be located anywhere.
the project has
1. `app.conf` file describing the project , project name, dependancies and verisons.
          check HOCON(Human-Optimized Config Object Notation)
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


