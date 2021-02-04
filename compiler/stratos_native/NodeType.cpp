//
// Created by kamau on 1/29/21.
//

#include <iostream>
#define stringify( name ) # name
enum class NodeType {

    LineComment,
    CommentBlock,

    Function,
    Return,
    Await,
    Async,

    Class,
    Package,
    Import,
    Variable ,
    Value,
    Let,
    Int,
    Double,
    String,
    Char,


    UserDefinedName,
    UserDefinedInt,
    UserDefinedLong,
    UserDefinedString,
    UserDefinedChar,

    Private,
    Public,

    ExclamationMarkEquals,
    AmpersandEquals,

    While,
    When,
    For,

    False,
    True,

    Continue,
    Break,

    GreaterThanEquals,
    LessThanEquals,
    EqualsEquals,

    MinusMinus,
    PlusPlus,
    Ampersand,
    Asterisk,

    BracketOpen,
    BracketClose,

    Caret,
    Colon,
    Comma,

    CurlyOpen,
    CurlyClose,

    Equals,
    NotEquals,

    ExclamationMark,
    Minus,
    ParenOpen,
    ParenClose,
    Percent,
    Period,
    Pipe,
    Plus,
    Multiply,
    QuestionMark,
    Semicolon,
    Slash,
    Tilde,

    LessThan,
    GreaterThan,

    TripleDot,

    Or,
    And,
    XOr,
    XAnd
};

static std::ostream& operator << (std::ostream& os, const NodeType& obj)
{
    os << static_cast<std::underlying_type<NodeType>::type>(obj);
    return os ;
}
