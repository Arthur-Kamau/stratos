.. Stratos Documentation documentation master file, created by
   sphinx-quickstart on Fri Sep 25 08:32:02 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to Stratos Documentation's documentation!
=================================================

   * Inspirations.
      A fast, fun to right, type safe , mixed paradigm programming language.
      A language that is familiar, industry proven  concepts such as `constructors`, `destructors`, `structs`, `classes`, `objects`  but also
      with also a mix of more recent patterns such as safety accessor (`some` and `none`)  rather than null.

      Hoping to provide concurrenncy language guarantees using  message passing inspired by Erlang  style mailbox.

    

   * Language basic
      compile to binary or genrate bytecode to run in an interpreter
      File extensions are `.st` or `.sts`<br>
      * `.st` files are for compilation (aot)
      * `.srt` files  that require a runtime (jit)

      however passed in compiler arguments (.conf file) overide the file extension type inference. 

   * Design.
      This contains the disgn notes when building or making decision involving the language, standard libraries or compiler.
      <br>
      
   * Tooling
      Tooling there will be a cli tool called `toolman` 
       (inspired by go in golang and cargo in rust). To get packages , test , build and run the language
       for more info check `/stratos/tooling/toolman/README.md`

   * Projects directory
      Projects can be located anywhere.
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



.. toctree::
   :maxdepth: 4
   :caption: Contents:

   folder_structure
   installing
   tooling
   syntax
   contribute
   help
   license
   disclaimer
   copyright
   extra



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
