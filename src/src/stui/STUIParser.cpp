#include "stratos/STUIParser.h"
#include <iostream>
#include <algorithm>

namespace stratos {
namespace stui {

STUIParser::STUIParser(const std::vector<STUIToken>& tokens, const std::string& sourceFile)
    : tokens_(tokens), sourceFile_(sourceFile) {}

// ============================================================================
// Token Navigation
// ============================================================================

bool STUIParser::isAtEnd() const {
    return peek().type == STUITokenType::END_OF_FILE;
}

const STUIToken& STUIParser::peek() const {
    return tokens_[current_];
}

const STUIToken& STUIParser::previous() const {
    return tokens_[current_ - 1];
}

const STUIToken& STUIParser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool STUIParser::check(STUITokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool STUIParser::match(STUITokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool STUIParser::match(std::initializer_list<STUITokenType> types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

const STUIToken& STUIParser::consume(STUITokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

STUIParseError STUIParser::error(const STUIToken& token, const std::string& message) {
    std::string fullMsg = sourceFile_ + ":" + std::to_string(token.line) + ":" +
                          std::to_string(token.column) + ": " + message +
                          " (got '" + token.lexeme + "')";
    return STUIParseError(fullMsg, token.line, token.column);
}

// ============================================================================
// Top-Level Parsing
// ============================================================================

STUIFile STUIParser::parse() {
    STUIFile file;

    // Parse optional package declaration
    if (check(STUITokenType::PACKAGE)) {
        parsePackage(file);
    }

    // Parse imports
    while (check(STUITokenType::IMPORT)) {
        parseImport(file);
    }

    // Parse components
    while (!isAtEnd()) {
        if (check(STUITokenType::COMPONENT)) {
            file.components.push_back(parseComponent());
        } else {
            throw error(peek(), "Expected 'component' declaration");
        }
    }

    return file;
}

void STUIParser::parsePackage(STUIFile& file) {
    consume(STUITokenType::PACKAGE, "Expected 'package'");
    file.packageName = consume(STUITokenType::IDENTIFIER, "Expected package name").lexeme;
    consume(STUITokenType::SEMICOLON, "Expected ';' after package name");
}

void STUIParser::parseImport(STUIFile& file) {
    consume(STUITokenType::IMPORT, "Expected 'import'");
    std::string path = consume(STUITokenType::STRING, "Expected import path string").lexeme;
    // Optional alias
    if (match(STUITokenType::AS)) {
        advance(); // consume alias name (we store just the path)
    }
    consume(STUITokenType::SEMICOLON, "Expected ';' after import");
    file.imports.push_back(path);
}

// ============================================================================
// Component Parsing
// ============================================================================

ComponentDecl STUIParser::parseComponent() {
    consume(STUITokenType::COMPONENT, "Expected 'component'");
    auto nameToken = consume(STUITokenType::IDENTIFIER, "Expected component name");
    ComponentDecl component(nameToken.lexeme, nameToken.line);

    consume(STUITokenType::LEFT_BRACE, "Expected '{' after component name");

    while (!check(STUITokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (check(STUITokenType::STATE)) {
            component.states.push_back(parseStateDecl());
        } else if (check(STUITokenType::COMPUTED)) {
            component.computeds.push_back(parseComputedDecl());
        } else if (check(STUITokenType::PROP)) {
            component.props.push_back(parsePropDecl());
        } else if (check(STUITokenType::VIEW)) {
            component.viewRoot = parseViewBlock();
        } else if (check(STUITokenType::FN)) {
            // Helper function — collect raw source for now
            advance(); // consume 'fn'
            std::string fnName = consume(STUITokenType::IDENTIFIER, "Expected function name").lexeme;
            // Skip to matching brace
            int startPos = current_;
            consume(STUITokenType::LEFT_PAREN, "Expected '(' after function name");
            // Skip params
            int parenDepth = 1;
            while (parenDepth > 0 && !isAtEnd()) {
                if (peek().type == STUITokenType::LEFT_PAREN) parenDepth++;
                if (peek().type == STUITokenType::RIGHT_PAREN) parenDepth--;
                if (parenDepth > 0) advance();
            }
            advance(); // consume closing )
            // Optional return type
            if (!check(STUITokenType::LEFT_BRACE)) {
                advance(); // consume return type
            }
            consume(STUITokenType::LEFT_BRACE, "Expected '{' for function body");
            int braceDepth = 1;
            while (braceDepth > 0 && !isAtEnd()) {
                if (peek().type == STUITokenType::LEFT_BRACE) braceDepth++;
                if (peek().type == STUITokenType::RIGHT_BRACE) braceDepth--;
                if (braceDepth > 0) advance();
            }
            advance(); // consume closing }
            // Store function name -> "raw" (reconstruction happens in transpiler)
            component.functions.push_back({fnName, ""});
        } else {
            throw error(peek(), "Expected 'state', 'computed', 'prop', 'view', or 'fn' inside component");
        }
    }

    consume(STUITokenType::RIGHT_BRACE, "Expected '}' to close component");
    return component;
}

StateDecl STUIParser::parseStateDecl() {
    consume(STUITokenType::STATE, "Expected 'state'");
    int line = previous().line;
    std::string name = consume(STUITokenType::IDENTIFIER, "Expected state variable name").lexeme;
    consume(STUITokenType::COLON, "Expected ':' after state name");
    std::string typeName = parseTypeName();

    std::unique_ptr<STUIExpr> initialValue;
    if (match(STUITokenType::EQUAL)) {
        initialValue = parseExpression();
    }

    consume(STUITokenType::SEMICOLON, "Expected ';' after state declaration");
    return StateDecl(name, typeName, std::move(initialValue), line);
}

ComputedDecl STUIParser::parseComputedDecl() {
    consume(STUITokenType::COMPUTED, "Expected 'computed'");
    int line = previous().line;
    std::string name = consume(STUITokenType::IDENTIFIER, "Expected computed variable name").lexeme;
    consume(STUITokenType::COLON, "Expected ':' after computed name");
    std::string typeName = parseTypeName();

    consume(STUITokenType::EQUAL, "Expected '=' after computed type");
    auto expression = parseExpression();

    consume(STUITokenType::SEMICOLON, "Expected ';' after computed declaration");
    return ComputedDecl(name, typeName, std::move(expression), line);
}

PropDecl STUIParser::parsePropDecl() {
    consume(STUITokenType::PROP, "Expected 'prop'");
    int line = previous().line;
    std::string name = consume(STUITokenType::IDENTIFIER, "Expected prop name").lexeme;
    consume(STUITokenType::COLON, "Expected ':' after prop name");
    std::string typeName = parseTypeName();

    std::unique_ptr<STUIExpr> defaultValue;
    if (match(STUITokenType::EQUAL)) {
        defaultValue = parseExpression();
    }

    consume(STUITokenType::SEMICOLON, "Expected ';' after prop declaration");
    return PropDecl(name, typeName, std::move(defaultValue), line);
}

// ============================================================================
// View Block & Widget Tree
// ============================================================================

std::unique_ptr<WidgetNode> STUIParser::parseViewBlock() {
    consume(STUITokenType::VIEW, "Expected 'view'");
    consume(STUITokenType::LEFT_BRACE, "Expected '{' after 'view'");

    // The view block contains a single root widget (or we wrap multiple in a Column)
    std::vector<std::unique_ptr<WidgetNode>> children;
    while (!check(STUITokenType::RIGHT_BRACE) && !isAtEnd()) {
        children.push_back(parseWidget());
    }

    consume(STUITokenType::RIGHT_BRACE, "Expected '}' to close view block");

    if (children.size() == 1) {
        return std::move(children[0]);
    }

    // Multiple root widgets → wrap in implicit Column
    auto column = std::make_unique<WidgetNode>("Column", children[0]->line);
    column->children = std::move(children);
    return column;
}

std::unique_ptr<WidgetNode> STUIParser::parseWidget() {
    // Widget: Identifier ( args ) { props/children }
    //     or: Identifier ( args )
    //     or: Identifier { props/children }
    auto nameToken = consume(STUITokenType::IDENTIFIER, "Expected widget name");
    auto widget = std::make_unique<WidgetNode>(nameToken.lexeme, nameToken.line);

    // Optional parenthesized arguments: Widget(arg1, arg2, key: value)
    if (match(STUITokenType::LEFT_PAREN)) {
        // Parse arguments — can be positional or named (key: value)
        while (!check(STUITokenType::RIGHT_PAREN) && !isAtEnd()) {
            // Check for named argument: identifier followed by ':'
            if (check(STUITokenType::IDENTIFIER) && current_ + 1 < static_cast<int>(tokens_.size()) &&
                tokens_[current_ + 1].type == STUITokenType::COLON) {
                // Named argument → treat as property
                std::string propName = advance().lexeme;
                advance(); // consume ':'
                auto value = parseExpression();
                widget->properties.push_back(Property(propName, std::move(value), nameToken.line));
            } else {
                // Positional argument
                widget->arguments.push_back(parseExpression());
            }
            if (!check(STUITokenType::RIGHT_PAREN)) {
                consume(STUITokenType::COMMA, "Expected ',' between arguments");
            }
        }
        consume(STUITokenType::RIGHT_PAREN, "Expected ')' after arguments");
    }

    // Optional body block: { properties, events, children }
    if (match(STUITokenType::LEFT_BRACE)) {
        parseWidgetBody(*widget);
        consume(STUITokenType::RIGHT_BRACE, "Expected '}' to close widget body");
    }

    return widget;
}

void STUIParser::parseWidgetBody(WidgetNode& widget) {
    while (!check(STUITokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Determine what this is:
        // 1. Property/event: identifier ':' value ';'
        // 2. Child widget: Identifier ( ... ) { ... }
        // 3. if/for control flow (future)

        if (check(STUITokenType::IDENTIFIER)) {
            // Look ahead to distinguish property from child widget
            // Properties: lowercase name followed by ':'
            // Widgets: PascalCase name (starts with uppercase)
            const std::string& name = peek().lexeme;

            if (!name.empty() && name[0] >= 'A' && name[0] <= 'Z') {
                // PascalCase → child widget
                widget.children.push_back(parseWidget());
            } else if (current_ + 1 < static_cast<int>(tokens_.size()) &&
                       tokens_[current_ + 1].type == STUITokenType::COLON) {
                // lowercase + ':' → property or event handler
                int line = peek().line;
                std::string propName = advance().lexeme;
                advance(); // consume ':'

                if (isEventName(propName)) {
                    widget.events.push_back(parseEventHandler(propName, line));
                } else {
                    auto value = parseExpression();
                    consume(STUITokenType::SEMICOLON, "Expected ';' after property value");
                    widget.properties.push_back(Property(propName, std::move(value), line));
                }
            } else {
                // Could be a component call (lowercase component name)
                widget.children.push_back(parseWidget());
            }
        } else if (check(STUITokenType::IF)) {
            // TODO: conditional rendering
            throw error(peek(), "Conditional rendering in widget body not yet supported");
        } else if (check(STUITokenType::FOR)) {
            // TODO: list rendering
            throw error(peek(), "List rendering in widget body not yet supported");
        } else {
            throw error(peek(), "Unexpected token in widget body");
        }
    }
}

EventHandler STUIParser::parseEventHandler(const std::string& name, int line) {
    std::unique_ptr<STUIExpr> handler;

    if (match(STUITokenType::LEFT_BRACE)) {
        // Inline handler: onClick: { count += 1; }
        // Collect all statements as a lambda body
        auto lambda = std::make_unique<LambdaExpr>();

        // Capture raw tokens until matching '}'
        int braceDepth = 1;
        std::string bodySource;
        while (braceDepth > 0 && !isAtEnd()) {
            if (peek().type == STUITokenType::LEFT_BRACE) braceDepth++;
            if (peek().type == STUITokenType::RIGHT_BRACE) {
                braceDepth--;
                if (braceDepth == 0) break;
            }
            bodySource += peek().lexeme + " ";
            advance();
        }
        consume(STUITokenType::RIGHT_BRACE, "Expected '}' to close event handler");

        lambda->bodySource = bodySource;
        handler = std::move(lambda);
    } else if (check(STUITokenType::FN)) {
        // Named lambda: onChange: fn(value) { ... }
        advance(); // consume 'fn'
        auto lambda = std::make_unique<LambdaExpr>();

        if (match(STUITokenType::LEFT_PAREN)) {
            while (!check(STUITokenType::RIGHT_PAREN) && !isAtEnd()) {
                lambda->params.push_back(consume(STUITokenType::IDENTIFIER, "Expected parameter name"));
                if (!check(STUITokenType::RIGHT_PAREN)) {
                    consume(STUITokenType::COMMA, "Expected ',' between parameters");
                }
            }
            consume(STUITokenType::RIGHT_PAREN, "Expected ')' after parameters");
        }

        consume(STUITokenType::LEFT_BRACE, "Expected '{' for lambda body");
        int braceDepth = 1;
        std::string bodySource;
        while (braceDepth > 0 && !isAtEnd()) {
            if (peek().type == STUITokenType::LEFT_BRACE) braceDepth++;
            if (peek().type == STUITokenType::RIGHT_BRACE) {
                braceDepth--;
                if (braceDepth == 0) break;
            }
            bodySource += peek().lexeme + " ";
            advance();
        }
        consume(STUITokenType::RIGHT_BRACE, "Expected '}' to close lambda");
        lambda->bodySource = bodySource;
        handler = std::move(lambda);
    } else {
        // Expression reference: onClick: handleClick;
        handler = parseExpression();
        consume(STUITokenType::SEMICOLON, "Expected ';' after event handler reference");
    }

    return EventHandler(name, std::move(handler), line);
}

// ============================================================================
// Expression Parsing (recursive descent, standard precedence)
// ============================================================================

std::unique_ptr<STUIExpr> STUIParser::parseExpression() {
    return parseAssignment();
}

std::unique_ptr<STUIExpr> STUIParser::parseAssignment() {
    auto expr = parseOr();

    if (match({STUITokenType::EQUAL, STUITokenType::PLUS_EQUAL,
               STUITokenType::MINUS_EQUAL, STUITokenType::STAR_EQUAL,
               STUITokenType::SLASH_EQUAL})) {
        auto op = previous();
        auto value = parseAssignment();
        return std::make_unique<AssignExpr>(std::move(expr), op, std::move(value));
    }

    return expr;
}

std::unique_ptr<STUIExpr> STUIParser::parseOr() {
    auto left = parseAnd();
    while (match(STUITokenType::PIPE_PIPE)) {
        auto op = previous();
        auto right = parseAnd();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<STUIExpr> STUIParser::parseAnd() {
    auto left = parseEquality();
    while (match(STUITokenType::AMPERSAND)) {
        auto op = previous();
        auto right = parseEquality();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<STUIExpr> STUIParser::parseEquality() {
    auto left = parseComparison();
    while (match({STUITokenType::EQUAL_EQUAL, STUITokenType::BANG_EQUAL})) {
        auto op = previous();
        auto right = parseComparison();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<STUIExpr> STUIParser::parseComparison() {
    auto left = parseTerm();
    while (match({STUITokenType::GREATER, STUITokenType::GREATER_EQUAL,
                  STUITokenType::LESS, STUITokenType::LESS_EQUAL})) {
        auto op = previous();
        auto right = parseTerm();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<STUIExpr> STUIParser::parseTerm() {
    auto left = parseFactor();
    while (match({STUITokenType::PLUS, STUITokenType::MINUS})) {
        auto op = previous();
        auto right = parseFactor();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<STUIExpr> STUIParser::parseFactor() {
    auto left = parseUnary();
    while (match({STUITokenType::STAR, STUITokenType::SLASH, STUITokenType::PERCENT})) {
        auto op = previous();
        auto right = parseUnary();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<STUIExpr> STUIParser::parseUnary() {
    if (match({STUITokenType::BANG, STUITokenType::MINUS})) {
        auto op = previous();
        auto operand = parseUnary();
        return std::make_unique<UnaryExpr>(op, std::move(operand));
    }
    return parseCall();
}

std::unique_ptr<STUIExpr> STUIParser::parseCall() {
    auto expr = parsePrimary();

    while (true) {
        if (match(STUITokenType::LEFT_PAREN)) {
            // Function call
            std::vector<std::unique_ptr<STUIExpr>> args;
            if (!check(STUITokenType::RIGHT_PAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match(STUITokenType::COMMA));
            }
            consume(STUITokenType::RIGHT_PAREN, "Expected ')' after arguments");
            expr = std::make_unique<CallExpr>(std::move(expr), std::move(args));
        } else if (match(STUITokenType::DOT)) {
            auto member = consume(STUITokenType::IDENTIFIER, "Expected member name after '.'");
            expr = std::make_unique<MemberAccessExpr>(std::move(expr), member);
        } else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<STUIExpr> STUIParser::parsePrimary() {
    if (match(STUITokenType::NUMBER)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    if (match(STUITokenType::STRING)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    if (match(STUITokenType::INTERPOLATED_STRING)) {
        return std::make_unique<InterpolatedStringExpr>(previous().lexeme);
    }
    if (match(STUITokenType::TRUE) || match(STUITokenType::FALSE)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    if (match(STUITokenType::NONE)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    if (match(STUITokenType::THIS)) {
        return std::make_unique<IdentifierExpr>(previous());
    }
    if (match(STUITokenType::IDENTIFIER)) {
        return std::make_unique<IdentifierExpr>(previous());
    }

    // Parenthesized expression
    if (match(STUITokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        consume(STUITokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }

    // Array literal
    if (match(STUITokenType::LEFT_BRACKET)) {
        std::vector<std::unique_ptr<STUIExpr>> elements;
        if (!check(STUITokenType::RIGHT_BRACKET)) {
            do {
                elements.push_back(parseExpression());
            } while (match(STUITokenType::COMMA));
        }
        consume(STUITokenType::RIGHT_BRACKET, "Expected ']' after array literal");
        return std::make_unique<ArrayExpr>(std::move(elements));
    }

    // Map literal: { key: value, ... }
    if (match(STUITokenType::LEFT_BRACE)) {
        std::vector<std::pair<std::string, std::unique_ptr<STUIExpr>>> entries;
        if (!check(STUITokenType::RIGHT_BRACE)) {
            do {
                std::string key = consume(STUITokenType::IDENTIFIER, "Expected map key").lexeme;
                consume(STUITokenType::COLON, "Expected ':' after map key");
                auto value = parseExpression();
                entries.push_back({key, std::move(value)});
            } while (match(STUITokenType::COMMA));
        }
        consume(STUITokenType::RIGHT_BRACE, "Expected '}' after map literal");
        return std::make_unique<MapExpr>(std::move(entries));
    }

    // Lambda: fn(params) { body } or fn() => expr
    if (match(STUITokenType::FN)) {
        auto lambda = std::make_unique<LambdaExpr>();
        if (match(STUITokenType::LEFT_PAREN)) {
            while (!check(STUITokenType::RIGHT_PAREN) && !isAtEnd()) {
                lambda->params.push_back(consume(STUITokenType::IDENTIFIER, "Expected parameter name"));
                if (!check(STUITokenType::RIGHT_PAREN)) {
                    consume(STUITokenType::COMMA, "Expected ',' between parameters");
                }
            }
            consume(STUITokenType::RIGHT_PAREN, "Expected ')' after parameters");
        }
        // Optional return type
        if (check(STUITokenType::IDENTIFIER) || check(STUITokenType::INT) ||
            check(STUITokenType::STRING_TYPE) || check(STUITokenType::BOOL) ||
            check(STUITokenType::VOID)) {
            advance(); // consume return type
        }
        if (match(STUITokenType::FAT_ARROW)) {
            // Expression body
            auto body = parseExpression();
            lambda->bodyExprs.push_back(std::move(body));
        } else {
            consume(STUITokenType::LEFT_BRACE, "Expected '{' for lambda body");
            int braceDepth = 1;
            std::string bodySource;
            while (braceDepth > 0 && !isAtEnd()) {
                if (peek().type == STUITokenType::LEFT_BRACE) braceDepth++;
                if (peek().type == STUITokenType::RIGHT_BRACE) {
                    braceDepth--;
                    if (braceDepth == 0) break;
                }
                bodySource += peek().lexeme + " ";
                advance();
            }
            consume(STUITokenType::RIGHT_BRACE, "Expected '}' to close lambda");
            lambda->bodySource = bodySource;
        }
        return lambda;
    }

    throw error(peek(), "Expected expression");
}

// ============================================================================
// Helpers
// ============================================================================

std::string STUIParser::parseTypeName() {
    std::string typeName;

    // Handle basic types and identifiers
    if (match({STUITokenType::INT, STUITokenType::FLOAT, STUITokenType::DOUBLE,
               STUITokenType::STRING_TYPE, STUITokenType::BOOL, STUITokenType::VOID})) {
        typeName = previous().lexeme;
    } else if (match(STUITokenType::IDENTIFIER)) {
        typeName = previous().lexeme;
    } else if (match(STUITokenType::ARRAY)) {
        typeName = "array";
    } else if (match(STUITokenType::MAP)) {
        typeName = "map";
    } else {
        throw error(peek(), "Expected type name");
    }

    // Handle generic parameters: Type<T> or Array<int>
    if (match(STUITokenType::LESS)) {
        typeName += "<";
        typeName += parseTypeName();
        while (match(STUITokenType::COMMA)) {
            typeName += ", ";
            typeName += parseTypeName();
        }
        consume(STUITokenType::GREATER, "Expected '>' after type parameter");
        typeName += ">";
    }

    return typeName;
}

bool STUIParser::isWidgetName() const {
    if (!check(STUITokenType::IDENTIFIER)) return false;
    const std::string& name = peek().lexeme;
    return !name.empty() && name[0] >= 'A' && name[0] <= 'Z';
}

bool STUIParser::isEventName(const std::string& name) const {
    // Event names start with "on" followed by uppercase
    if (name.length() < 3) return false;
    return name[0] == 'o' && name[1] == 'n' && name[2] >= 'A' && name[2] <= 'Z';
}

} // namespace stui
} // namespace stratos
