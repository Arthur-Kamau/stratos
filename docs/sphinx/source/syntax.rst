Syntax
===========
The syntax is closely based to kotlin and c#.But with an approach to code layout  similar to golang.
If you are familiar with above languages it easy to pick up Stratos syntax.

+++++++++
Comments
+++++++++

    Comments could be:

    1. Multi line comment.

    This encapsulate everthing between the start of the comment and thge end of the comment ie ``/*`` start of the comment and ``*/`` end of the comment.

    2. Single line comments.
    
    Single line comment start with ``//`` upto the line

    Example : 

        .. code-block:: java
    
            /**
            * multi line 
            * comment 
            */

            // single line comment

    Find an example project under ``sample/comments_and_documentation``        



+++++++++++++++
Package 
+++++++++++++++
    A package is a collection of source files in the same directory that are compiled together. Functions,Classes and Variables defined in one source file are visible to all other source files within the same package.

    A package should be the first declaration in  a file, based on the folder name but if in src the package name is src.


+++++++++++++
Import
+++++++++++++
The import keyword is used to import a package, class or struct.that exists in a different package.
for imported package the import declaration should be the name of the dependency, the name is part of the object specied in the app.config file



++++++++++++++++++++++++
Variable declaration
++++++++++++++++++++++++

    There are three ways to declare variables there is ``var``, ``val``, ``let``.This is followed by a variable name a colon and the variable type wich is optional.


    for more information view ``sample/variables``


++++++++++++++++
Data Types
++++++++++++++++

    The  language has 8 basic data types are;

        1.  Byte
                The byte data type is an 8-bit signed two's complement integer. It has a minimum value of -128 and a maximum value of 127 (inclusive)

        2.  Strings.
                A sequence of characters.

        3.  Character
                A  UTF-16 code units.

        4.  Double
                A double-precision floating point number.Stratos doubles are 64-bit floating-point numbers as specified in the IEEE 754 standard.

        5.  Integer
                is 64-bit  

        6. Boolean.
                The boolean data type has only two possible values: true and false. Use this data type for simple flags that track true/false conditions

        7.  Option 
                a. Some
                    Used to indicate that data is contained in a variable
                b. None
                    Used to represent empty, nothing."null"

        8.  Any
                This is a container for an unknown data type.

    There are secondary data types based on the primary types
        1. Map.
                A collection of key value objects.

        2. List.
                A collection of objects .

        3. Set
            A collection of objects in which each object can occur only once.

    **Note**  Stratos has a strong type inference as such specifying the data type is mostly optional.
    The follwing example are used to illustrate data type.


    Example : 

        .. code-block:: java
            
            // variable of type string
            let  name : String = "Arthur"
            // or 
            let name ="Arthur"
            



    for more information view ``sample/variables``    

++++++++++++++++++++++++++++++++
Type decorators 
++++++++++++++++++++++++++++++++

    1. `static` 
        Variables or functions to be refenced do not require an instance

    2. `private`  
        Variables , functions or classes denoted with private keyword cannot be used outside the scope they are declared 
        example:

            * a private class cannot be used outside the package scope if declared in main packages only classes in the main package can use the class
            * if a function is declared private only function


++++++++++++++++++++++++++++++++
loops
++++++++++++++++++++++++++++++++


    loops include 

    1. ``For`` loop
        For loop is used to iterate an enumarable
        example
    
    Example : 
        .. code-block:: java

            var countries = ["Kenya", "Uganda" , "Rwanda", "Tanzania", "Burundi" ]

            for(var country in countries){
                // it is a keyword 
                print($"{it.index} country {country}")
            }

    2.  ``loop`` 
        similar to while loop
    
     Example : 
        .. code-block:: java

            // to create an infinite list
            loop {
                print( $"{it.index} will print forever")
            }

            // loop over a list
            var countries = ["Kenya", "Uganda" , "Rwanda", "Tanzania", "Burundi" ]

            loop countries {
                 print($"{it.index} country {it.item}")
            }





++++++++++++++++++++++++++++++++
Conditions
++++++++++++++++++++++++++++++++
