package com.stratos.analysis.Parser;
import com.stratos.model.Statement;
import com.stratos.model.Token;
import com.stratos.model.TokenType;


import java.util.ArrayList;
import java.util.List;

import static  com.stratos.model.TokenType.*;

public class Parser {

    private static class ParseError extends RuntimeException {}
    private final List<Token> tokens;
    private int current = 0;

    public Parser(List<Token> tokens) {
        this.tokens = tokens;
    }
    public List<Statement> parse(){
        List<Statement> statements = new ArrayList<>();
        while (!isAtEnd()) {
/* Statements and State parse < Statements and State parse-declaration
      statements.add(statement());
*/
//> parse-declaration
            statements.add(declaration());
//< parse-declaration
        }

        return statements;
    }



    //> match
    private boolean match(TokenType... types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }

        return false;
    }
    //< match
//> consume
    private Token consume(TokenType type, String message) {
        if (check(type)) return advance();

        throw error(peek(), message);
    }
    //< consume
//> check
    private boolean check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }
    //< check
//> advance
    private Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }
    //< advance
//> utils
    private boolean isAtEnd() {
        return peek().type == EOF;
    }

    private Token peek() {
        return tokens.get(current);
    }

    private Token previous() {
        return tokens.get(current - 1);
    }
    //< utils
//> error
    private ParseError error(Token token, String message) {
        error(token, message);
        return new ParseError();
    }
    //< error
//> synchronize
    private void synchronize() {
        advance();

        while (!isAtEnd()) {
            if (previous().type == SEMICOLON) return;

            switch (peek().type) {
                case CLASS:
                case FUNCTION:
                case VAR:
                case FOR:
                case IF:
                case WHILE:
                case PRINT:
                case RETURN:
                    return;
            }

            advance();
        }
    }
//< synchronize
}
