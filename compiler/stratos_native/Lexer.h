//
// Created by kamau on 1/29/21.
//

#ifndef STRATOS_NATIVE_LEXER_H
#define STRATOS_NATIVE_LEXER_H
#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <fstream>
#include "TokenType.h"
#include "Token.h"

class Lexer {
private:
    int lineNumber = 1;
    int currentLineCharacter = 0;
    int currentCharacter;
    static std::map<std::string, TokenType> s_keywords;

    static std::map<std::string, TokenType> s_three_char_tokens;
    static std::map<std::string, TokenType> s_two_char_tokens;
    static std::map<char, TokenType> s_single_char_tokens;

public:

    void consume();
    Token next();
    explicit Lexer(std::string source);

};

#endif //STRATOS_NATIVE_LEXER_H
