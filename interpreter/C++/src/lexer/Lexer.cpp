#include "stratos/Lexer.h"
#include <iostream>

namespace stratos {

std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"and", TokenType::AND},
    {"class", TokenType::CLASS},
    {"struct", TokenType::STRUCT},
    {"interface", TokenType::INTERFACE},
    {"enum", TokenType::ENUM},
    {"package", TokenType::PACKAGE},
    {"use", TokenType::USE},
    {"else", TokenType::ELSE},
    {"false", TokenType::FALSE},
    {"for", TokenType::FOR},
    {"fn", TokenType::FN},
    {"if", TokenType::IF},
    {"None", TokenType::NONE},
    {"or", TokenType::OR},
    {"return", TokenType::RETURN},
    {"super", TokenType::SUPER},
    {"this", TokenType::THIS},
    {"true", TokenType::TRUE},
    {"var", TokenType::VAR},
    {"val", TokenType::VAL},
    {"while", TokenType::WHILE},
    {"when", TokenType::WHEN},
    {"int", TokenType::INT},
    {"double", TokenType::DOUBLE},
    {"bool", TokenType::BOOL},
    {"void", TokenType::VOID},
    {"unit", TokenType::UNIT},
    {"Optional", TokenType::OPTIONAL},
    {"constructor", TokenType::CONSTRUCTOR},
    {"not", TokenType::NOT}
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    column++;
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    addToken(type, source.substr(start, current - start));
}

void Lexer::addToken(TokenType type, std::string literal) {
    tokens.push_back({type, literal, line, column});
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': 
            if (match('=')) addToken(TokenType::MINUS_EQUAL);
            else if (match('>')) addToken(TokenType::ARROW);
            else addToken(TokenType::MINUS); 
            break;
        case '+': 
            addToken(match('=') ? TokenType::PLUS_EQUAL : TokenType::PLUS); 
            break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': 
            addToken(match('=') ? TokenType::STAR_EQUAL : TokenType::STAR); 
            break;
        case ':': 
            addToken(TokenType::COLON); 
            break;
        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
        case '|':
            if (match('>')) addToken(TokenType::PIPE);
            else if (match('|')) addToken(TokenType::OR);
            else { /* Bitwise OR handling if needed */ }
            break;
        case '?':
             if (match('.')) addToken(TokenType::QUESTION_DOT);
             else if (match(':')) addToken(TokenType::ELVIS);
             break;
        case '/':
            if (match('/')) {
                // Comment
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('=')) {
                addToken(TokenType::SLASH_EQUAL);
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace
            break;
        case '\n':
            line++;
            column = 0;
            break;
        case '"': string(); break;
        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                std::cerr << "Unexpected character at line " << line << ": " << c << std::endl;
            }
            break;
    }
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 0;
        }
        advance();
    }

    if (isAtEnd()) {
        std::cerr << "Unterminated string at line " << line << std::endl;
        return;
    }

    advance(); // The closing "
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::number() {
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance(); // Consume .
        while (isDigit(peek())) advance();
    }

    addToken(TokenType::NUMBER);
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.find(text) != keywords.end()) {
        type = keywords[text];
    }
    addToken(type);
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

} // namespace stratos
