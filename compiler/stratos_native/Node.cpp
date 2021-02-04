//
// Created by kamau on 1/29/21.
//
#include <iostream>
#include "NodeType.cpp"

struct Node {

    int start;
    int end;
    int line;
    std::string literal;
    NodeType type;


};

static std::ostream& operator<<(std::ostream& os, const Node& node) {
    return os << "  start: " << node.start << std::endl
              << "  end: " << node.end << std::endl
              << "  line: " << node.line << std::endl
              << "  literal: " << node.literal << std::endl
              << "  type:" << node.type << std::endl;
}