#include "stratos/Lexer.h"
#include <iostream>
#include <sstream>

namespace stratos {

std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"and", TokenType::AND},
    {"class", TokenType::CLASS},
    {"struct", TokenType::STRUCT},
    {"interface", TokenType::INTERFACE},
    {"enum", TokenType::ENUM},
    {"package", TokenType::PACKAGE},
    {"use", TokenType::USE},
    {"as", TokenType::AS}, // Add AS keyword
    {"async", TokenType::ASYNC},
    {"await", TokenType::AWAIT},
    {"break", TokenType::BREAK},
    {"case", TokenType::CASE},
    {"class", TokenType::CLASS},
    {"continue", TokenType::CONTINUE},
    {"defer", TokenType::DEFER},
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
    {"select", TokenType::SELECT},
    {"int", TokenType::INT},
    {"double", TokenType::DOUBLE},
    {"bool", TokenType::BOOL},
    {"void", TokenType::VOID},
    {"unit", TokenType::UNIT},
    {"Optional", TokenType::OPTIONAL},
    // Low-level numeric types (like Rust/Zig)
    {"i8", TokenType::I8},
    {"i16", TokenType::I16},
    {"i32", TokenType::I32},
    {"i64", TokenType::I64},
    {"u8", TokenType::U8},
    {"u16", TokenType::U16},
    {"u32", TokenType::U32},
    {"u64", TokenType::U64},
    {"f32", TokenType::F32},
    {"f64", TokenType::F64},
    {"usize", TokenType::USIZE},
    {"isize", TokenType::ISIZE},
    {"constructor", TokenType::CONSTRUCTOR},
    {"pub", TokenType::PUB},
    {"not", TokenType::NOT},
    {"type", TokenType::TYPE}
};

Lexer::Lexer(const std::string& source, const std::string& source_file) : source(source), source_file(source_file) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line, column, source_file});
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
    tokens.push_back({type, literal, line, column, source_file});
}

void Lexer::addToken(TokenType type, std::string literal, int startColumn) {
    tokens.push_back({type, literal, line, startColumn, source_file});
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
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': 
            if (match('.')) {
                if (match('.')) addToken(TokenType::DOT_DOT_DOT);
                else addToken(TokenType::DOT_DOT);
            }
            else addToken(TokenType::DOT); 
            break;
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
        case '%': addToken(TokenType::PERCENT); break;
        case ':': 
            addToken(TokenType::COLON); 
            break;
        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            if (match('=')) addToken(TokenType::EQUAL_EQUAL);
            else if (match('>')) addToken(TokenType::ARROW);  // Support => for lambdas
            else addToken(TokenType::EQUAL);
            break;
        case '<':
            if (match('=')) addToken(TokenType::LESS_EQUAL);
            else if (match('<')) addToken(TokenType::LEFT_SHIFT);
            else if (match('-')) addToken(TokenType::LEFT_ARROW);
            else addToken(TokenType::LESS);
            break;
        case '>':
            if (match('=')) addToken(TokenType::GREATER_EQUAL);
            else if (match('>')) addToken(TokenType::RIGHT_SHIFT);
            else addToken(TokenType::GREATER);
            break;
        case '|':
            if (match('>')) addToken(TokenType::PIPE);
            else if (match('|')) addToken(TokenType::OR);
            else addToken(TokenType::BITWISE_OR);
            break;
        case '&':
            if (match('&')) addToken(TokenType::AND);
            else addToken(TokenType::BITWISE_AND);
            break;
        case '^': addToken(TokenType::BITWISE_XOR); break;
        case '~': addToken(TokenType::BITWISE_NOT); break;
        case '?':
             if (match('.')) addToken(TokenType::QUESTION_DOT);
             else if (match(':')) addToken(TokenType::ELVIS);
             else addToken(TokenType::QUESTION);
             break;
        case '/':
            if (match('/')) {
                // Single-line comment
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                // Block comment or doc comment
                blockComment();
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
        case '\'': character(); break;
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
    int tokenStartColumn = column;
    std::string value = "";
    bool hasInterpolation = false;

    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\\') {
            // Handle escape sequence
            advance(); // consume backslash
            if (isAtEnd()) {
                std::cerr << "Unterminated string at line " << line << std::endl;
                return;
            }
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '0': value += '\0'; break;
                case '$': value += '$'; break; // Escape $ to prevent interpolation
                default:
                    std::cerr << "Invalid escape sequence in string literal at line " << line << std::endl;
                    value += escaped; // Include the character anyway
                    break;
            }
        } else if (peek() == '$') {
            // Check for interpolation: $identifier or ${expression}
            char next = peekNext();
            if (isAlpha(next) || next == '_' || next == '{') {
                hasInterpolation = true;
                value += advance(); // Add the $
                if (peek() == '{') {
                    // ${expression} - include everything until }
                    value += advance(); // Add the {
                    int braceCount = 1;
                    while (braceCount > 0 && !isAtEnd() && peek() != '"') {
                        if (peek() == '{') braceCount++;
                        if (peek() == '}') braceCount--;
                        value += advance();
                    }
                } else {
                    // $identifier - include the identifier
                    while ((isAlphaNumeric(peek()) || peek() == '_') && !isAtEnd()) {
                        value += advance();
                    }
                }
            } else {
                value += advance(); // Just a $ with no interpolation
            }
        } else {
            if (peek() == '\n') {
                line++;
                column = 0;
            }
            value += advance();
        }
    }

    if (isAtEnd()) {
        std::cerr << "Unterminated string at line " << line << std::endl;
        return;
    }

    advance(); // The closing "

    if (hasInterpolation) {
        addToken(TokenType::INTERPOLATED_STRING, value, tokenStartColumn);
    } else {
        addToken(TokenType::STRING, value, tokenStartColumn);
    }
}

void Lexer::character() {
    int tokenStartColumn = column;
    // Character literals should contain exactly one character
    if (isAtEnd() || peek() == '\'') {
        std::cerr << "Empty character literal at line " << line << std::endl;
        return;
    }

    // Handle escape sequences
    char charValue;
    if (peek() == '\\') {
        advance(); // consume backslash
        if (isAtEnd()) {
            std::cerr << "Unterminated character literal at line " << line << std::endl;
            return;
        }
        char escaped = advance();
        switch (escaped) {
            case 'n': charValue = '\n'; break;
            case 't': charValue = '\t'; break;
            case 'r': charValue = '\r'; break;
            case '\\': charValue = '\\'; break;
            case '\'': charValue = '\''; break;
            case '0': charValue = '\0'; break;
            default:
                std::cerr << "Invalid escape sequence in character literal at line " << line << std::endl;
                return;
        }
    } else {
        charValue = advance();
    }

    // Expect closing quote
    if (peek() != '\'') {
        std::cerr << "Unterminated character literal at line " << line << std::endl;
        return;
    }

    advance(); // The closing '
    addToken(TokenType::CHAR, std::string(1, charValue), tokenStartColumn);
}

void Lexer::number() {
    int tokenStartColumn = column - 1;
    
    // Check for hex literal
    // We already consumed the first digit in scanToken
    if (source[current - 1] == '0' && (peek() == 'x' || peek() == 'X')) {
        advance(); // consume 'x'
        
        while (isHexDigit(peek())) advance();
    } else {
        while (isDigit(peek())) advance();

        if (peek() == '.' && isDigit(peekNext())) {
            advance(); // Consume .
            while (isDigit(peek())) advance();
        }
    }

    std::string numText = source.substr(start, current - start);
    addToken(TokenType::NUMBER, numText, tokenStartColumn);
}

void Lexer::identifier() {
    int tokenStartColumn = column;
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.find(text) != keywords.end()) {
        type = keywords[text];
    }
    addToken(type, text, tokenStartColumn);
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

bool Lexer::isHexDigit(char c) {
    return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

void Lexer::blockComment() {
    // Check if this is a doc comment (starts with /**)
    bool isDocComment = (peek() == '*' && peekNext() != '/');

    if (isDocComment) {
        advance(); // consume second *
    }

    std::string content = "";
    int startLine = line;
    int startColumn = column;

    // Consume comment content
    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
        if (peek() == '\n') {
            content += peek();
            line++;
            column = 0;
        } else {
            content += peek();
        }
        advance();
    }

    if (isAtEnd()) {
        std::cerr << "Unterminated block comment at line " << startLine << std::endl;
        return;
    }

    advance(); // *
    advance(); // /

    // If doc comment, create token with content
    if (isDocComment) {
        Token docToken;
        docToken.type = TokenType::DOC_COMMENT;
        docToken.lexeme = "/**" + content + "*/";
        docToken.docText = trimDocComment(content);
            docToken.line = startLine;
            docToken.column = startColumn;
            docToken.file = source_file;
            tokens.push_back(docToken);    }
    // Otherwise, regular comment is discarded (current behavior)
}

std::string Lexer::trimDocComment(const std::string& raw) {
    // Remove leading asterisks and whitespace from each line
    std::string result;
    std::istringstream stream(raw);
    std::string line;

    while (std::getline(stream, line)) {
        // Trim leading whitespace and asterisk
        size_t start = line.find_first_not_of(" \t*");
        if (start != std::string::npos) {
            result += line.substr(start) + "\n";
        } else {
            result += "\n";
        }
    }

    // Remove trailing newlines
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
        result.pop_back();
    }

    return result;
}

} // namespace stratos
