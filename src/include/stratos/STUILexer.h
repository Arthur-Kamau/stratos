#ifndef STRATOS_STUI_LEXER_H
#define STRATOS_STUI_LEXER_H

#include "STUIToken.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace stratos {
namespace stui {

class STUILexer {
public:
    STUILexer(const std::string& source, const std::string& sourceFile);
    std::vector<STUIToken> scanTokens();

private:
    const std::string& source_;
    const std::string& sourceFile_;
    std::vector<STUIToken> tokens_;
    int start_ = 0;
    int current_ = 0;
    int line_ = 1;
    int column_ = 1;

    static std::unordered_map<std::string, STUITokenType> keywords_;

    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    void addToken(STUITokenType type);
    void addToken(STUITokenType type, const std::string& lexeme);

    void scanToken();
    void string();
    void number();
    void identifier();
    void blockComment();

    static bool isDigit(char c);
    static bool isAlpha(char c);
    static bool isAlphaNumeric(char c);
};

} // namespace stui
} // namespace stratos

#endif // STRATOS_STUI_LEXER_H
