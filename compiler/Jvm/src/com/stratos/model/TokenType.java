package com.stratos.model;

public enum TokenType {

    // keywords
    VAL,
    VAR,
    LET,
    LIST,
    CLASS,
    FUNCTION,
    STRUCT,
    ENUM,
    PACKAGE,
    IMPORT,
    PRIVATE,
    PUBLIC,
    RETURN,
    SOME,
    NONE,
    Not,

    ALIAS,
    THEN,
    WITH,
    WHILE,
    FOR,

    // types
    INTEGER,
    DOUBLE,
    STRING,
    StringValue,
    Character,
    IF,
    ELSE,
    WHEN,
    IMPLEMENTS,
    EXTENDS,

    // operation 
    AND,
    OR,
    ANDOR,


    //BOOLEAN
    FALSE ,
    TRUE,

    // binary 

    BINARYAND,
    BINARYOR,


    // operators
    DIVIDE,
    MULTIPLY,
    MODULAS,
    ADD,
    ASSIGN,
    SUBTRACT,


   NOT,
    NOTEQUALTO,
    EQUALTO,

    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,LEFT_SQUARE, RIGHT_SQUARE,
    NEWLINE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON,
COLON , QUESTIONMARK ,
    LESSTHAN,
    LESSTHANOREQUALTO,
    GREATERTHAN, GREATERTHANOREQUALTO,

    LINECOMMENT, MULTILINECOMMENT,

    PRINT, SUPER, THIS, EOF


}
