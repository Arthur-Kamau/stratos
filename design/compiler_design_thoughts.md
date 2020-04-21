## Compiler Basics
A programming languages split lines of code into statements and expression.

* An expression evaluates to a value.Think arithmetic expression etc.
* An statement produces an effect. Think a println.

With a strong time inference with likes of Ocamel , Kotlin Or Go, show that 
even a statically typed language could look dynamic by having a refined type deduction system.


Compiler front end :

Parser -> Lexer -> syntax anlysis  ->semantic anlysis


* Parser -> Takes in a file, read the file splitting the source into tokens,token is just a struct/ class containing the type of the lexem , line number , character start , character end  and the the lexem itselft.
example 
    `let name = "Arthur"`
    lexemes are `let` `name` `=` `Arthur`
Hence the first lexeme (let) could be represented as  `Token ( type=TypeDeclaration line=1 charcterStart=0 characterEnd=2 data="let" )` 


* Lexer -> creates an abstract syntax tree  from the tokens list/stream .Modern computer(haha even phones) have enough Memory to hold the entire source code in memory so the need to use stream is not urgent probably even not necessary. 
            // TODO add more description     

* syntax analysis ->  checks if the language syntax is followed in statement examples include
                        1. use of undefined synatx eg 
                            `....`
                            `GOTO 23`
                            `.....`
                            Stratos does not have GOTO statements.
                            `error GOTO is not recognized by stratos`
                        2. use of keywords in variable declaration.
                            `...`
                            `let as := 3`
                            `...`
* semantic analysis ->  looks at language statement and verify it its correct
                     Example think of wrong english. example     `let name : string =44`
                     This should throw off  an error `integer could not be assigned to string`


