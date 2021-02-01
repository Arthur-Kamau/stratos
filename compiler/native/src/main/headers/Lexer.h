//
// Created by kamau on 1/29/21.
//

#ifndef STRATOS_NATIVE_LEXER_H
#define STRATOS_NATIVE_LEXER_H

class Lexer {
private:
    int lineNumber = 1;
    int currentLineCharacter = 0;
    int currentCharacter;

public:

    void consume();

};

#endif //STRATOS_NATIVE_LEXER_H
