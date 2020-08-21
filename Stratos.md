


### Language basic
compile to binary or genrate bytecode to run in an interpreter
File extensions are `.st` or `.sts`<br>
* `.st` files are for compilation (aot)
* `.srt` files  that require a runtime (jit)

however passed in compiler arguments overide the file extension type inference. 

### Design & docs
Thoughts and desions made. 

<br>
<br>

### Tooling
tooling there will be a cli tool called `toolman` 
(inspired by go in golang and cargo in rust). To get packages , test , build and run the language
for more info check `/stratos/tooling/toolman/README.md`

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

3. a `bin` for the debug executables.
4. `dist` distributable binaries. 
