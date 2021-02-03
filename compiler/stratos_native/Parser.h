//
// Created by kamau on 1/29/21.
//

#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include "TokenType.h"
#include "Token.h"

#ifndef STRATOS_NATIVE_PARSER_H
#define STRATOS_NATIVE_PARSER_H

class parser {

private:
    parser();

     std::map<std::string, TokenType> s_keywords;

     std::map<std::string, TokenType> s_three_char_tokens;
     std::map<std::string, TokenType> s_two_char_tokens;
     std::map<char, TokenType> s_single_char_tokens;
public:
    std::vector<Token> parse();
};

#endif //STRATOS_NATIVE_PARSER_H
