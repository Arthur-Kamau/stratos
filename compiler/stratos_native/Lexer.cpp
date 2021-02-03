//
// Created by kamau on 1/29/21.
//

#include "Lexer.h"


char consume();

std::vector<Node> Lexer::lex_text(std::string source) {

    source_length = source.size();
    char_array.assign(source.begin(), source.end());

    while (!is_eof()){
       lex();

    }
    return nodes;
}

char Lexer::peek() {
    return char_array[current_character_index];
}

bool Lexer::is_eof() const {
    return current_character_index > source_length ;
}

char Lexer::consume() {
    char x = char_array[current_character_index];
    current_character_index++;
    return x;
}

void Lexer::lex(){
    std::cerr << "lex";
    char item = consume();
    switch (item) {

        default:
            std::cerr << "Unknown " << item << std::endl ;
    }
}





