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
    Stream,
    Yield,
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

    With,
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


    As,
    Is,
    Range,

    List,
    Set,
    Map,

    Caret,
    Colon,
    Comma,


    CurlyBracketsClose,
    CurlyBracketsOpen,

    RoundBracketsOpen,
    RoundBracketsClose,

    SquareBracketsOpen,
    SquareBracketsClose,

    AngleBracketsOpen,
    AngleBracketsClose,

    Equals,
    NotEquals,

    ExclamationMark,
    Minus,

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
    XAnd, UnderScore, Else
};

static std::ostream& operator << (std::ostream& os, const NodeType& obj)
{
    os << static_cast<std::underlying_type<NodeType>::type>(obj);
    return os ;
}
