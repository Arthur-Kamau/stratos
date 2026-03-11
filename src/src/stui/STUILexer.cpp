#include "stratos/STUILexer.h"
#include <stdexcept>

namespace stratos {
namespace stui {

std::unordered_map<std::string, STUITokenType> STUILexer::keywords_ = {
    // STUI-specific
    {"component", STUITokenType::COMPONENT},
    {"state",     STUITokenType::STATE},
    {"computed",  STUITokenType::COMPUTED},
    {"view",      STUITokenType::VIEW},
    {"prop",      STUITokenType::PROP},
    {"style",     STUITokenType::STYLE},
    {"on",        STUITokenType::ON},
    {"emit",      STUITokenType::EMIT},
    // Shared with Stratos
    {"import",    STUITokenType::IMPORT},
    {"as",        STUITokenType::AS},
    {"package",   STUITokenType::PACKAGE},
    {"if",        STUITokenType::IF},
    {"else",      STUITokenType::ELSE},
    {"for",       STUITokenType::FOR},
    {"while",     STUITokenType::WHILE},
    {"when",      STUITokenType::WHEN},
    {"in",        STUITokenType::IN},
    {"is",        STUITokenType::IS},
    {"val",       STUITokenType::VAL},
    {"var",       STUITokenType::VAR},
    {"fn",        STUITokenType::FN},
    {"return",    STUITokenType::RETURN},
    {"true",      STUITokenType::TRUE},
    {"false",     STUITokenType::FALSE},
    {"none",      STUITokenType::NONE},
    {"this",      STUITokenType::THIS},
    {"int",       STUITokenType::INT},
    {"float",     STUITokenType::FLOAT},
    {"double",    STUITokenType::DOUBLE},
    {"string",    STUITokenType::STRING_TYPE},
    {"bool",      STUITokenType::BOOL},
    {"void",      STUITokenType::VOID},
    {"array",     STUITokenType::ARRAY},
    {"map",       STUITokenType::MAP},
};

STUILexer::STUILexer(const std::string& source, const std::string& sourceFile)
    : source_(source), sourceFile_(sourceFile) {}

std::vector<STUIToken> STUILexer::scanTokens() {
    while (!isAtEnd()) {
        start_ = current_;
        scanToken();
    }
    tokens_.push_back({STUITokenType::END_OF_FILE, "", line_, column_, sourceFile_});
    return tokens_;
}

bool STUILexer::isAtEnd() const {
    return current_ >= static_cast<int>(source_.length());
}

char STUILexer::advance() {
    char c = source_[current_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

char STUILexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char STUILexer::peekNext() const {
    if (current_ + 1 >= static_cast<int>(source_.length())) return '\0';
    return source_[current_ + 1];
}

bool STUILexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    advance();
    return true;
}

void STUILexer::addToken(STUITokenType type) {
    std::string lexeme = source_.substr(start_, current_ - start_);
    tokens_.push_back({type, lexeme, line_, column_ - static_cast<int>(lexeme.length()), sourceFile_});
}

void STUILexer::addToken(STUITokenType type, const std::string& lexeme) {
    tokens_.push_back({type, lexeme, line_, column_ - static_cast<int>(lexeme.length()), sourceFile_});
}

void STUILexer::scanToken() {
    char c = advance();

    switch (c) {
        case '(': addToken(STUITokenType::LEFT_PAREN); break;
        case ')': addToken(STUITokenType::RIGHT_PAREN); break;
        case '{': addToken(STUITokenType::LEFT_BRACE); break;
        case '}': addToken(STUITokenType::RIGHT_BRACE); break;
        case '[': addToken(STUITokenType::LEFT_BRACKET); break;
        case ']': addToken(STUITokenType::RIGHT_BRACKET); break;
        case ',': addToken(STUITokenType::COMMA); break;
        case '.':
            if (match('.')) {
                addToken(STUITokenType::DOT_DOT);
            } else {
                addToken(STUITokenType::DOT);
            }
            break;
        case ';': addToken(STUITokenType::SEMICOLON); break;
        case ':': addToken(STUITokenType::COLON); break;
        case '+':
            if (match('=')) addToken(STUITokenType::PLUS_EQUAL);
            else addToken(STUITokenType::PLUS);
            break;
        case '-':
            if (match('>')) addToken(STUITokenType::ARROW);
            else if (match('=')) addToken(STUITokenType::MINUS_EQUAL);
            else addToken(STUITokenType::MINUS);
            break;
        case '*':
            if (match('=')) addToken(STUITokenType::STAR_EQUAL);
            else addToken(STUITokenType::STAR);
            break;
        case '%': addToken(STUITokenType::PERCENT); break;
        case '!':
            if (match('=')) addToken(STUITokenType::BANG_EQUAL);
            else addToken(STUITokenType::BANG);
            break;
        case '=':
            if (match('=')) addToken(STUITokenType::EQUAL_EQUAL);
            else if (match('>')) addToken(STUITokenType::FAT_ARROW);
            else addToken(STUITokenType::EQUAL);
            break;
        case '<':
            if (match('=')) addToken(STUITokenType::LESS_EQUAL);
            else addToken(STUITokenType::LESS);
            break;
        case '>':
            if (match('=')) addToken(STUITokenType::GREATER_EQUAL);
            else addToken(STUITokenType::GREATER);
            break;
        case '&':
            if (match('&')) addToken(STUITokenType::AMPERSAND);
            break;
        case '|':
            if (match('|')) addToken(STUITokenType::PIPE_PIPE);
            break;
        case '/':
            if (match('/')) {
                // Single-line comment
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                blockComment();
            } else if (match('=')) {
                addToken(STUITokenType::SLASH_EQUAL);
            } else {
                addToken(STUITokenType::SLASH);
            }
            break;

        // Whitespace
        case ' ':
        case '\r':
        case '\t':
        case '\n':
            break;

        case '"': string(); break;

        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                // Unknown character - add as error
                addToken(STUITokenType::ERROR);
            }
            break;
    }
}

void STUILexer::string() {
    std::string value;
    bool hasInterpolation = false;

    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\\') {
            advance(); // consume backslash
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '"': value += '"'; break;
                case '\\': value += '\\'; break;
                case '$': value += '$'; break;
                default: value += '\\'; value += escaped; break;
            }
        } else if (peek() == '$') {
            hasInterpolation = true;
            value += advance(); // include $ as-is for now
            if (peek() == '{') {
                value += advance(); // include {
                int depth = 1;
                while (depth > 0 && !isAtEnd()) {
                    if (peek() == '{') depth++;
                    if (peek() == '}') depth--;
                    if (depth > 0) value += advance();
                    else advance(); // consume closing }
                }
                value += '}';
            } else {
                // Simple $identifier
                while (isAlphaNumeric(peek())) {
                    value += advance();
                }
            }
        } else {
            if (peek() == '\n') {
                line_++;
                column_ = 0;
            }
            value += advance();
        }
    }

    if (isAtEnd()) {
        addToken(STUITokenType::ERROR, "Unterminated string");
        return;
    }

    advance(); // closing "

    if (hasInterpolation) {
        addToken(STUITokenType::INTERPOLATED_STRING, value);
    } else {
        addToken(STUITokenType::STRING, value);
    }
}

void STUILexer::number() {
    while (isDigit(peek())) advance();

    // Look for fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        advance(); // consume '.'
        while (isDigit(peek())) advance();
    }

    addToken(STUITokenType::NUMBER);
}

void STUILexer::identifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source_.substr(start_, current_ - start_);
    auto it = keywords_.find(text);
    if (it != keywords_.end()) {
        addToken(it->second);
    } else {
        addToken(STUITokenType::IDENTIFIER);
    }
}

void STUILexer::blockComment() {
    int depth = 1;
    while (depth > 0 && !isAtEnd()) {
        if (peek() == '/' && peekNext() == '*') {
            advance(); advance();
            depth++;
        } else if (peek() == '*' && peekNext() == '/') {
            advance(); advance();
            depth--;
        } else {
            advance();
        }
    }
}

bool STUILexer::isDigit(char c) { return c >= '0' && c <= '9'; }
bool STUILexer::isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
bool STUILexer::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

} // namespace stui
} // namespace stratos
