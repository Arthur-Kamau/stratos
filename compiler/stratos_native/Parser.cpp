//
// Created by kamau on 1/29/21.
//
#include "Parser.h"

std::vector<Token> Parser::parse(std::vector<Node> nodes) {

    while (!is_eof()){
       parse_node();
    }

    return std::vector<Token>();
}

void Parser::parse_node()  {
    switch (peek().type) {
//        case NodeType::Variable :
//
//            break;
        default:
            std::cerr << "Unable to process" << peek() << std::endl;
    }
    advance();
}

Node Parser::advance()  {
    if(!is_eof()){
        current_character_index++;
    }
    return nodes_list[current_character_index];
}
Node Parser::peek() const {
    return nodes_list[current_character_index];
}
bool Parser::is_eof() const {
    return peek().type == NodeType::Eof;

}


