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

class Parser {

private:


public:
    std::vector<Token> parse();
};

#endif //STRATOS_NATIVE_PARSER_H
