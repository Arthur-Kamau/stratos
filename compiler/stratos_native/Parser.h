//
// Created by kamau on 1/29/21.
//
#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <fstream>

#include "Token.h"

#ifndef STRATOS_NATIVE_PARSER_H
#define STRATOS_NATIVE_PARSER_H
#include "Node.cpp"

class Parser {

private:
    std::vector<Token> token_list;
    std::vector<Node> nodes_list;
    int  current_character_index =0  ;
    void parse_node();

public:
    std::vector<Token> parse(std::vector<Node> nodes);

    bool is_eof() const;

    Node peek() const;

    Node advance() ;
};

#endif //STRATOS_NATIVE_PARSER_H
