//
// Created by kamau on 1/29/21.
//

#include <iostream>

enum class NodeType {
    Function,
    Class,
    Package,
    Variable ,
    Value,
    Constant,
    UserDefinedName,
    ExclamationMarkEquals,
    AmpersandEquals,
    Await,
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
    ExclamationMark,
    Minus,
    ParenOpen,
    ParenClose,
    Percent,
    Period,
    Pipe,
    Plus,
    QuestionMark,
    Semicolon,
    Slash,
    Tilde,
    LessThan,
    GreaterThan,
    TripleDot,
};

static std::ostream& operator << (std::ostream& os, const NodeType& obj)
{
    os << static_cast<std::underlying_type<NodeType>::type>(obj);
    return os;
}
