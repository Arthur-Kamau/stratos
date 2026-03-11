#ifndef STRATOS_STUI_TOKEN_H
#define STRATOS_STUI_TOKEN_H

#include <string>

namespace stratos {
namespace stui {

enum class STUITokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, SEMICOLON, COLON, PLUS, MINUS, STAR, SLASH, PERCENT,
    EQUAL, BANG,

    // Multi-character tokens
    BANG_EQUAL, EQUAL_EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL,
    ARROW, // ->
    FAT_ARROW, // =>
    DOT_DOT, // ..
    AMPERSAND, // &&
    PIPE_PIPE, // ||

    // Literals
    IDENTIFIER, STRING, NUMBER, INTERPOLATED_STRING,

    // STUI-specific keywords
    COMPONENT, STATE, COMPUTED, VIEW, PROP, STYLE,
    ON, EMIT,

    // Shared keywords (subset of Stratos)
    IMPORT, AS, PACKAGE,
    IF, ELSE, FOR, WHILE, WHEN,
    IN, IS,
    VAL, VAR,
    FN, RETURN,
    TRUE, FALSE, NONE,
    THIS,
    INT, FLOAT, DOUBLE, STRING_TYPE, BOOL, VOID,
    ARRAY, MAP,

    // Documentation
    DOC_COMMENT,

    // EOF
    END_OF_FILE,
    ERROR
};

struct STUIToken {
    STUITokenType type;
    std::string lexeme;
    int line;
    int column;
    std::string file;

    std::string toString() const {
        return "STUIToken(" + std::to_string(static_cast<int>(type)) +
               ", '" + lexeme + "', " + file + ":" +
               std::to_string(line) + ":" + std::to_string(column) + ")";
    }
};

} // namespace stui
} // namespace stratos

#endif // STRATOS_STUI_TOKEN_H
