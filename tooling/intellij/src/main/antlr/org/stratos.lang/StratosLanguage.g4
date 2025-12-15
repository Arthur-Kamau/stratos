grammar StratosLanguage;

script
    : (declaration | statement)* EOF
    ;

declaration
    : varDecl
    | fnDecl
    | classDecl
    | structDecl
    | interfaceDecl
    | enumDecl
    | packageDecl
    | useDecl
    ;

useDecl
    : 'use' ID (':' ID)* ';'
    ;

packageDecl
    : 'package' ID ';'
    ;

varDecl
    : ('val' | 'var') ID (':' type)? ('=' expr)? ';'
    ;

fnDecl
    : 'fn' ID '(' paramList? ')' (type)? block
    ;

paramList
    : param (',' param)*
    ;

param
    : ID ':' type
    ;

classDecl
    : 'class' ID (':' ID)? '{' classMember* '}'
    ;

structDecl
    : 'struct' ID '{' structMember* '}'
    ;

interfaceDecl
    : 'interface' ID '{' fnSignature* '}'
    ;

enumDecl
    : 'enum' ID '{' enumList '}'
    ;

enumList
    : ID (',' ID)*
    ;

classMember
    : varDecl 
    | fnDecl 
    | constructorDecl
    ;

structMember
    : ID ':' type (',' | ';')?
    ;

fnSignature
    : 'fn' ID '(' paramList? ')' (type)? ';'
    ;

constructorDecl
    : 'constructor' '(' paramList? ')' block
    ;

type
    : 'int'
    | 'double'
    | 'string'
    | 'bool'
    | 'void'
    | 'unit'
    | 'any'
    | 'Optional' '<' type '>'
    | ID
    ;

block
    : '{' (declaration | statement)* '}'
    ;

statement
    : ifStmt
    | whileStmt
    | returnStmt
    | exprStmt
    | block
    ;

ifStmt
    : 'if' '(' expr ')' statement ('else' statement)?
    ;

whileStmt
    : 'while' '(' expr ')' statement
    ;

returnStmt
    : 'return' expr? ';'
    ;

exprStmt
    : expr ';'
    ;

expr
    : expr pipeOp expr          # PipeExpr
    | expr orOp expr            # OrExpr
    | expr andOp expr           # AndExpr
    | expr eqOp expr            # EqExpr
    | expr compOp expr          # CompExpr
    | expr addOp expr           # AddExpr
    | expr multOp expr          # MulExpr
    | unaryOp expr              # UnaryExpr
    | primary                   # AtomExpr
    ;

primary
    : INT
    | FLOAT
    | STRING
    | 'true'
    | 'false'
    | 'None'
    | ID
    | '(' expr ')'
    | primary '(' argList? ')'  # CallExpr
    | primary '.' ID            # AccessExpr
    | primary safeDot ID        # SafeAccessExpr
    | primary elvis expr        # ElvisExpr
    | ifExpr                    # IfExpr
    | whenExpr                  # WhenExpr
    ;

argList
    : expr (',' expr)*
    ;

ifExpr
    : 'if' '(' expr ')' expr 'else' expr
    ;

whenExpr
    : 'when' '(' expr ')' '{' whenEntry* '}'
    ;

whenEntry
    : (expr | 'else') '->' block
    ;

// Lexer Rules

VAL : 'val' ;
VAR : 'var' ;
FN : 'fn' ;
CLASS : 'class' ;
STRUCT : 'struct' ;
INTERFACE : 'interface' ;
ENUM : 'enum' ;
PACKAGE : 'package' ;
USE : 'use' ;
CONSTRUCTOR : 'constructor' ;
IF : 'if' ;
ELSE : 'else' ;
WHILE : 'while' ;
RETURN : 'return' ;
WHEN : 'when' ;
TRUE : 'true' ;
FALSE : 'false' ;
NONE : 'None' ;
SOME : 'Some' ;

pipeOp : '|>' ;
orOp : '||' ;
andOp : '&&' ;
eqOp : '==' | '!=' ;
compOp : '<' | '<=' | '>' | '>=' ;
addOp : '+' | '-' ;
multOp : '*' | '/' | '%' ;
unaryOp : '!' | '-' | 'not' ;
safeDot : '?.' ;
elvis : '?:' ;

ID : [a-zA-Z_] [a-zA-Z0-9_]* ;
INT : [0-9]+ ;
FLOAT : [0-9]+ '.' [0-9]+ ;
STRING : '"' .*? '"' ;

WS : [ 	
]+ -> skip ;
LINE_COMMENT : '//' ~[
]* -> skip ;
BLOCK_COMMENT : '/*' .*? '*/' -> skip ;