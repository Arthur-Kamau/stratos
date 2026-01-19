#ifndef STRATOS_LEXER_H
#define STRATOS_LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "stratos/Token.h"

namespace stratos {

class Lexer {
public:
    Lexer(const std::string& source, const std::string& source_file);
    std::vector<Token> scanTokens();

private:
    const std::string& source;
    const std::string& source_file; // New member to store the source file path
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
    int column = 1;

    static std::unordered_map<std::string, TokenType> keywords;

    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, std::string literal);
    void addToken(TokenType type, std::string literal, int startColumn);
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void character();
    void number();
    void identifier();
    void scanToken();
    void blockComment();
    std::string trimDocComment(const std::string& raw);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    bool isDigit(char c);
    bool isHexDigit(char c);
};

} // namespace stratos

#endif // STRATOS_LEXER_H
