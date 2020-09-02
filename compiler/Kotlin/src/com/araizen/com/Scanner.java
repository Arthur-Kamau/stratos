package com.araizen.com;

import com.araizen.com.util.print.Log;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Scanner {
    String src;
    int start = 0;
    int current = 0;
    int line = 1;
    List<Token> tokens = new ArrayList<>();

    private static final Map<String, TokenType> keywords;

    static {
        keywords = new HashMap<>();
        keywords.put("and", TokenType.And);
        keywords.put("class", TokenType.Class);
        keywords.put("else", TokenType.Else);
        keywords.put("false", TokenType.False);
        keywords.put("for", TokenType.For);
        keywords.put("fn", TokenType.Function);
        keywords.put("function", TokenType.Function);
        keywords.put("is", TokenType.Is);
        keywords.put("as", TokenType.As);
        keywords.put("if", TokenType.If);
        keywords.put("nil", TokenType.Nil);
        keywords.put("none", TokenType.None);
        keywords.put("some", TokenType.Some);
        keywords.put("alias", TokenType.Alias);
        keywords.put("or", TokenType.Or);
        keywords.put("use", TokenType.Use);
        keywords.put("import", TokenType.Import);
        keywords.put("return", TokenType.Return);
        keywords.put("super", TokenType.Super);
        keywords.put("this", TokenType.This);
        keywords.put("true", TokenType.True);
        keywords.put("val", TokenType.Val);
        keywords.put("var", TokenType.Var);
        keywords.put("let", TokenType.Let);
        keywords.put("mut", TokenType.Mut);
        keywords.put("while", TokenType.While);
        keywords.put("when", TokenType.When);
    }


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
                addToken(TokenType.LeftParen, '(');
            case ')':
                addToken(TokenType.RightParen, ')');
            case '{':
                addToken(TokenType.LeftBrace, '{');
            case '}':
                addToken(TokenType.RightBrace, '}');
            case ',':
                addToken(TokenType.Comma, ',');
            case '.':
                addToken(TokenType.Dot, ".");
            case '+':
                addToken(TokenType.Plus, '+');
            case '-':
                addToken(TokenType.Minus, '-');
            case ';':
                addToken(TokenType.Semicolon, ';');
            case '*':
                addToken(TokenType.Star);
            case '!':
                addToken(match('=') ? TokenType.BangEqual : TokenType.Bang);
            case '=':
                addToken(match('=') ? TokenType.EqualEqual : TokenType.Equal);
            case '<':
                addToken(match('=') ? TokenType.LessEqual : TokenType.Less);
            case '>':
                addToken(match('=') ? TokenType.GreaterEqual : TokenType.Greater);
            case '/':
                if (match('/')) {
                    while (

                            peek() != '\n' && !isAtEnd()) advance();
                } else {
                    addToken(TokenType.Slash);
                }

            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;
            case '\n':
                line++;
                break;
            case '"':
                string();
                break;
            default:
/* Scanning char-error < Scanning digit-start
        Lox.error(line, "Unexpected character.");
*/
//> digit-start
                if (isDigit(c)) {
                    number();
//> identifier-start
                } else if (isAlpha(c)) {
                    identifier();
//< identifier-start
                } else {
                    Log.error("Unexpected character. " + "{" + c + "}", line);
                }
//< digit-start
                break;
//< char-error

        }
    }


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

    //< scan-token
//> identifier
    private void identifier() {
        while (isAlphaNumeric(peek())) advance();

/* Scanning identifier < Scanning keyword-type
    addToken(IDENTIFIER);
*/
//> keyword-type
        // See if the identifier is a reserved word.
        String text = src.substring(start, current);

        TokenType type = keywords.get(text);
        if (type == null) type = TokenType.Identifier; //IDENTIFIER;
        addToken(type);
//< keyword-type
    }


    private void number() {
        while (isDigit(peek())) advance();

        // Look for a fractional part.
        if (peek() == '.' && isDigit(peekNext())) {
            // Consume the "."
            advance();

            while (isDigit(peek())) advance();
        }

        addToken(TokenType.Number,
                Double.parseDouble(src.substring(start, current)));
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

    //< match
//> peek
    private char peek() {
        if (isAtEnd()) return '\0';
        return src.charAt(current);
    }

    //< peek
//> peek-next
    private char peekNext() {
        if (current + 1 >= src.length()) return '\0';
        return src.charAt(current + 1);
    } // [peek-next]

    //< peek-next
//> is-alpha
    private boolean isAlpha(char c) {
        return (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                c == '_';
    }

    private boolean isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    //< is-alpha
//> is-digit
    private boolean isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    private boolean isAtEnd() {
        return current >= src.length();
    }

    //< number
    //> string
    private void string() {

        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') line++;
            advance();


        }


        // Unterminated string.
        if (isAtEnd()) {
            Log.error("Unterminated string.", line);
            return;
        }

        // The closing ".
        advance();

        // Trim the surrounding quotes.
        String value = src.substring(start + 1, current - 1);
        addToken(TokenType.String, value);
    }
}
