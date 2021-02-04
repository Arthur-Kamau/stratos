//
// Created by kamau on 1/29/21.
//
#pragma once
#ifndef STRATOS_NATIVE_LEXER_H
#define STRATOS_NATIVE_LEXER_H
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <fstream>
#include <bits/stdc++.h>
#include "Node.cpp"
//#include "NodeType.cpp"

class Lexer {
private:

    std::vector<Node> nodes;
    std::vector<char> char_array;

    int line_number = 0;
    int source_length =0;
    int  current_character_index =0  ;


    std::map<std::string, NodeType> s_keywords;

    std::map<std::string, NodeType> s_three_char_tokens;
    std::map<std::string, NodeType> s_two_char_tokens;
    std::map<char, NodeType> s_single_char_tokens;

    char current();
    bool is_eof() ;
    char advance();
    char previous();
    void lex();

public:

     std::vector<Node> lex_text(std::string source);

    bool non_digit(char ch);

    bool hexadecimal_digit(char ch);

    bool octal_digit(char ch);

    bool digit(char ch);

    bool single_symbol(char ch);
    bool double_symbol(std::string ch);
    bool key_word(std::string ch);
    bool nonzero_digit(char ch);

    Lexer();
};

#endif //STRATOS_NATIVE_LEXER_H
