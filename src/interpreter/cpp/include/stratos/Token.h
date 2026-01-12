#ifndef STRATOS_TOKEN_H
#define STRATOS_TOKEN_H

#include <string>
#include <iostream>

namespace stratos {

#ifdef _WIN32
// Undefine conflicting macros from windows.h
#undef FALSE
#undef TRUE
#undef ERROR
#undef VOID
#undef INT
#undef DOUBLE
#undef BOOL
#undef THIS
#undef INTERFACE
#undef IGNORE
#undef CONST
#undef STRICT
#undef OPTIONAL
// NONE is usually not defined, but good to be safe if it is.
#undef NONE
#endif

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, PERCENT,
    COLON,

    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    PIPE, // |>
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL,
    ARROW, // ->
    QUESTION, // ?
    QUESTION_DOT, // ?.
    ELVIS, // ?:
    DOT_DOT, // ..
    DOT_DOT_DOT, // ...

    // Literals
    IDENTIFIER, STRING, NUMBER, CHAR,

    // Keywords
    AND, OR, NOT,
    CLASS, STRUCT, INTERFACE, ENUM, PACKAGE, USE, AS,
    BREAK, CONTINUE,
    ELSE, FALSE, FN, FOR, IF, NONE,
    RETURN, SUPER, THIS, TRUE, VAR, VAL,
    WHILE, WHEN,
    INT, DOUBLE, BOOL, VOID, UNIT, OPTIONAL,
    CONSTRUCTOR, PUB,

    // Documentation
    DOC_COMMENT,

    // EOF
    END_OF_FILE,
    ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    std::string file;       // Path to the source file
    std::string docText;  // For DOC_COMMENT tokens - stores the raw comment text

    // Helper to print token for debugging
    std::string toString() const {
        return "Token(" + std::to_string(static_cast<int>(type)) + ", '" + lexeme + "', " + file + ":" + std::to_string(line) + ":" + std::to_string(column) + ")";
    }
};

} // namespace stratos

#endif // STRATOS_TOKEN_H
