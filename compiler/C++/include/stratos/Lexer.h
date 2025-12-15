#ifndef STRATOS_LEXER_H
#define STRATOS_LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "stratos/Token.h"

namespace stratos {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
    int column = 0;

    static std::unordered_map<std::string, TokenType> keywords;

    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, std::string literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();
    void scanToken();
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    bool isDigit(char c);
};

} // namespace stratos

#endif // STRATOS_LEXER_H
