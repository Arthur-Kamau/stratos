//
// Created by kamau on 1/29/21.
//

#ifndef STRATOS_NATIVE_NODETYPE_H
#define STRATOS_NATIVE_NODETYPE_H


enum class NodeType {
    Comma,
    SemiColon,
    LParen,
    RParen,
    LCurlyBrace,
    RCurlyBrace,
    LSquareBrace,
    RSquareBrace,
    Function,
    Class,
    Package,
    If,
    Else,
    True,
    False,
    Return,

    Assign,
    Equal,
    GreaterThan,
    LessThan,

    Int,
    Float,
    Character,
    String,

    NewLine,

    AlphaNumeric




};


#endif //STRATOS_NATIVE_NODETYPE_H
