## Stratos Native Compiler.

###Design
```
    |-----------------------------|
    |                             |
    |   Text (File/console        |
    |                             |
    |-----------------------------|
                    |   
                    |       
                    |
                    &#8595;
    |--------------------------------------------|
    |                                            |
    |   Lexer                                    |
    |   Generates -> {Nodes (index,type)}        |
    |                                            |
    |--------------------------------------------|
                    |   
                    |       
                    |
                    &#8595;
    |-----------------------------------------------------------|
    |                                                           |
    |   Parser                                                  |
    |   Generates  -> Tokens (expression/statement/functioncal) |       |
    |                                                           |
    |-----------------------------------------------------------|
                    |   
                    |       
                    |
                    &#8595;
    |--------------------------------------------|
    |                                            |
    |   Analysis                                 |
    |   validates tokens                         |
    |                                            |
    |--------------------------------------------|

```




###Code patterns.
1. Classes, Structs & enums start with uppercase 
2. variables start with lower letter, private variables start with an underscore)
3. classes should have a cpp & header files(wherever possible)