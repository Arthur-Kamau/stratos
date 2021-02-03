//
// Created by kamau on 1/29/21.
//

#ifndef STRATOS_NATIVE_LEXER_H
#define STRATOS_NATIVE_LEXER_H
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <bits/stdc++.h>
#include "NodeType.h"
#include "Node.h"

class Lexer {
private:

    std::vector<Node> nodes;
    std::vector<char> char_array;

    int line_number ;
    int source_length ;
    int current_character_index =0;


    char peek();
    bool is_eof() const;
    char consume();
    void lex();

public:

     std::vector<Node> lex_text(std::string source);

};

#endif //STRATOS_NATIVE_LEXER_H
