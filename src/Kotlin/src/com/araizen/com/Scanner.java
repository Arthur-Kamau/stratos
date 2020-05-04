package com.araizen.com;

import java.util.ArrayList;
import java.util.List;

public class Scanner {
    String src;
    int start = 0;
    int current = 0;
    int line = 1;
    List<Token> tokens = new ArrayList<>();

    Scanner(String src) {
        this.src = src;
    }

    List<Token> scanTokens() {
        while (!isAtEnd()) {
            start = current;
            scanToken();
        }
        tokens.add(new Token(TokenType.Eof, "", null, line));
        return tokens;
    }

    void scanToken() {
        char c = advance();
        switch (c) {
            case '(':
                addToken(TokenType.LeftParen);
            case ')':
                addToken(TokenType.RightParen);
            case '{':
                addToken(TokenType.LeftBrace);
            case '}':
                addToken(TokenType.RightBrace);
            case ',':
                addToken(TokenType.Comma);
            case '.':
                addToken(TokenType.Dot);
            case '+':
                addToken(TokenType.Plus);
            case '-':
                addToken(TokenType.Minus);
            case ';':
                addToken(TokenType.Semicolon);
            case '*':
                addToken(TokenType.Star);
            case '!' :  addToken(match('=') ?  TokenType.BangEqual : TokenType.Bang );
            case '=' :  addToken(match('=') ?  TokenType.EqualEqual : TokenType.Equal );
            case '<' :  addToken(match('=') ?  TokenType.LessEqual : TokenType.Less );
            case '>' :  addToken(match('=') ?  TokenType.GreaterEqual : TokenType.Greater );
            case '/':  if(match('/')){ while (peek() != '\n' && !isAtEnd())  advance(); }else{ addToken(TokenType.Slash);}

            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;
            case '\n':
                line++; break;
            case '"': string() ; break ;
        }
    }
void 
    boolean match(char ex) {
        if (isAtEnd()) {
            return false;
        }
        if (src.charAt(current) != ex) {
            return false;
        }
        current++;
        return true;
    }

    void addToken(TokenType type) {
        addToken(type, null);
    }

    void addToken(TokenType type, Object literal) {
        String txt = src.substring(start, current);
        tokens.add(new Token(type, txt, literal, line));
    }

    char advance() {
        current++;
        return src.charAt(current - 1);
    }

    private boolean isAtEnd() {
        return current >= src.length();
    }
}
