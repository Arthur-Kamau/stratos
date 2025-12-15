#ifndef STRATOS_TOKEN_H
#define STRATOS_TOKEN_H

#include <string>
#include <iostream>

namespace stratos {

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
    COLON,

    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    PIPE, // |>
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL,
    ARROW, // ->
    QUESTION_DOT, // ?.
    ELVIS, // ?:

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    AND, OR, NOT,
    CLASS, STRUCT, INTERFACE, ENUM, NAMESPACE,
    ELSE, FALSE, FN, FOR, IF, NONE,
    RETURN, SUPER, THIS, TRUE, VAR, VAL,
    WHILE, WHEN,
    INT, DOUBLE, BOOL, VOID, UNIT, OPTIONAL,
    CONSTRUCTOR,

    // EOF
    END_OF_FILE,
    ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    // Helper to print token for debugging
    std::string toString() const {
        return "Token(" + std::to_string(static_cast<int>(type)) + ", '" + lexeme + "')";
    }
};

} // namespace stratos

#endif // STRATOS_TOKEN_H
