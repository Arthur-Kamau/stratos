#include "stratos/Parser.h"
#include <iostream>

namespace stratos {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        try {
            statements.push_back(declaration());
        } catch (ParseError& error) {
            std::cerr << "[Error] " << error.line << ":" << error.column << ": " << error.what() << std::endl;
            synchronize();
        }
    }
    return statements;
}

// --- Declarations ---

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::VAR, TokenType::VAL})) return varDeclaration();
        if (match({TokenType::FN})) return fnDeclaration("function");
        if (match({TokenType::CLASS, TokenType::STRUCT, TokenType::INTERFACE})) return classDeclaration();
        if (match({TokenType::NAMESPACE})) return namespaceDeclaration();
        return statement();
    } catch (ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    bool isMutable = (previous().type == TokenType::VAR);
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::string typeName = "";
    if (match({TokenType::COLON})) {
        typeName = parseType();
    }

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<VarDecl>(name, typeName, std::move(initializer), isMutable);
}

std::unique_ptr<Stmt> Parser::fnDeclaration(const std::string& kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");

    std::vector<Token> params;
    std::vector<std::string> paramTypes;

    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            params.push_back(paramName);
            
            std::string type = "any";
            if (match({TokenType::COLON})) {
                type = parseType();
            }
            paramTypes.push_back(type);
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    std::string returnType = "void";
    if (check(TokenType::IDENTIFIER) || check(TokenType::INT) || check(TokenType::DOUBLE) || check(TokenType::STRING) || check(TokenType::BOOL)) {
        // Optional return type parsing without colon if it's just the type
        // The grammar says `fn name() type {` or `fn name() {`
        // We need to be careful not to mistake the start of the block for a type if it's an identifier (unlikely)
        // But types are usually keywords or Capitalized.
        if (!check(TokenType::LEFT_BRACE)) {
             returnType = parseType();
        }
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<std::unique_ptr<Stmt>> body;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        body.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after " + kind + " body.");

    auto bodyPtr = std::make_unique<std::vector<std::unique_ptr<Stmt>>>(std::move(body));
    return std::make_unique<FunctionDecl>(name, params, paramTypes, returnType, std::move(bodyPtr));
}

std::unique_ptr<Stmt> Parser::classDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
    
    std::unique_ptr<VariableExpr> superclass = nullptr;
    if (match({TokenType::COLON})) {
        consume(TokenType::IDENTIFIER, "Expect superclass name.");
        superclass = std::make_unique<VariableExpr>(previous());
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

    std::vector<std::unique_ptr<Stmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match({TokenType::VAR, TokenType::VAL})) {
            methods.push_back(varDeclaration());
        } else if (match({TokenType::FN})) {
             methods.push_back(fnDeclaration("method"));
        } else if (match({TokenType::CONSTRUCTOR})) {
            // Treat constructor like a function named "constructor"
            // Hack for now to reuse fnDeclaration logic logic mostly
            // But fnDeclaration expects name.
            Token ctorName = previous(); // "constructor"
            consume(TokenType::LEFT_PAREN, "Expect '(' after constructor.");
             // ... duplicate logic or refactor. For now, skip body
             while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) advance(); // SKIP
             advance(); // consume }
        } else {
             // Skip unknown things inside class for now to avoid infinite loops
             advance();
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");

    return std::make_unique<ClassDecl>(name, std::move(superclass), std::move(methods));
}

std::unique_ptr<Stmt> Parser::namespaceDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect namespace name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before namespace body.");
    std::vector<std::unique_ptr<Stmt>> body;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        body.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after namespace body.");
    return std::make_unique<NamespaceDecl>(name, std::move(body));
}

// --- Statements ---

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::WHEN})) return whenStatement();
    if (match({TokenType::LEFT_BRACE})) return block();
    
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::whenStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'when'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after when condition.");
    
    consume(TokenType::LEFT_BRACE, "Expect '{' before when body.");
    
    // Skip body for now to ensure parsing completes
    int braces = 1;
    while (braces > 0 && !isAtEnd()) {
        Token t = peek();
        if (t.type == TokenType::LEFT_BRACE) braces++;
        if (t.type == TokenType::RIGHT_BRACE) braces--;
        advance();
    }
    
    return std::make_unique<BlockStmt>(std::vector<std::unique_ptr<Stmt>>{});
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    std::unique_ptr<Stmt> thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;

    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
    std::unique_ptr<Stmt> body = statement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

std::unique_ptr<Stmt> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<PrintStmt>(std::move(expr));
}

// --- Expressions ---

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    std::unique_ptr<Expr> expr = pipe(); 

    if (match({TokenType::EQUAL, TokenType::PLUS_EQUAL, TokenType::MINUS_EQUAL})) {
        Token equals = previous();
        std::unique_ptr<Expr> value = assignment(); // Right-associative

        // In a full compiler, we'd check if 'expr' is a valid assignment target (VariableExpr)
        // For AST generation, we can just return a BinaryExpr or specialized AssignExpr
        // For now, treat as BinaryExpr to keep it simple, or cast.
        return std::make_unique<BinaryExpr>(std::move(expr), equals, std::move(value));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::pipe() {
    std::unique_ptr<Expr> expr = logicOr();
    while (match({TokenType::PIPE})) {
        Token op = previous();
        std::unique_ptr<Expr> right = logicOr();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::logicOr() {
    std::unique_ptr<Expr> expr = logicAnd();
    while (match({TokenType::OR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = logicAnd();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::logicAnd() {
    std::unique_ptr<Expr> expr = equality();
    while (match({TokenType::AND})) {
        Token op = previous();
        std::unique_ptr<Expr> right = equality();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> expr = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();
    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return call();
}

std::unique_ptr<Expr> Parser::call() {
    std::unique_ptr<Expr> expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(std::move(expr));
        } else if (match({TokenType::DOT})) {
             Token dot = previous();
             Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
             // Transform into property access expr (omitted for brevity, simpler to assume variable expr in this context implies access if not call)
             // But strictly we need a GetExpr. Reusing VariableExpr logic or generic BinaryExpr for dot?
             // Let's cheat and make it a BinaryExpr with DOT op
             // Right side needs to be an Expr.
             expr = std::make_unique<BinaryExpr>(std::move(expr), dot, std::make_unique<VariableExpr>(name));
        } else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee) {
    std::vector<std::unique_ptr<Expr>> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            // Handle named arguments: identifier '=' expression
            // Simplified: just parse expression.
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_unique<CallExpr>(std::move(callee), paren, std::move(arguments));
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) return std::make_unique<LiteralExpr>("false", TokenType::FALSE);
    if (match({TokenType::TRUE})) return std::make_unique<LiteralExpr>("true", TokenType::TRUE);
    if (match({TokenType::NONE})) return std::make_unique<LiteralExpr>("None", TokenType::NONE);

    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous().lexeme, previous().type);
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableExpr>(previous());
    }
    
    // Handle 'this'
    if (match({TokenType::THIS})) {
        return std::make_unique<VariableExpr>(previous());
    }

    // Handle If-Expression (Ternary)
    if (match({TokenType::IF})) {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        std::unique_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        std::unique_ptr<Expr> thenExpr = expression();
        consume(TokenType::ELSE, "Expect 'else' for if-expression.");
        std::unique_ptr<Expr> elseExpr = expression();
        
        // Map to a specialized TernaryExpr or (mis)use CallExpr/IfStmt?
        // Ideally we need TernaryExpr in AST.
        // For now, let's treat it as a Call to a special intrinsic "if_expr(cond, then, else)"
        // This avoids changing AST structure mid-flight.
        std::vector<std::unique_ptr<Expr>> args;
        args.push_back(std::move(condition));
        args.push_back(std::move(thenExpr));
        args.push_back(std::move(elseExpr));
        return std::make_unique<CallExpr>(
            std::make_unique<VariableExpr>(Token{TokenType::IDENTIFIER, "__if_expr", 0, 0}), 
            Token{TokenType::RIGHT_PAREN, ")", 0, 0}, 
            std::move(args)
        );
    }

    if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    throw ParseError("Expect expression. Found: " + peek().lexeme, peek().line, peek().column);
}

// --- Helpers ---

std::string Parser::parseType() {
    if (match({TokenType::OPTIONAL})) {
        consume(TokenType::LESS, "Expect <");
        std::string inner = parseType();
        consume(TokenType::GREATER, "Expect >");
        return "Optional<" + inner + ">";
    }
    if (match({TokenType::IDENTIFIER, TokenType::INT, TokenType::STRING, TokenType::BOOL, TokenType::DOUBLE, TokenType::VOID})) {
        return previous().lexeme;
    }
    Token t = peek();
    throw ParseError("Expect type. Found: " + t.lexeme, t.line, t.column);
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::consume(TokenType type, std::string message) {
    if (check(type)) return advance();
    Token t = peek();
    throw ParseError(message + " Found: " + t.lexeme, t.line, t.column);
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FN:
            case TokenType::VAR:
            case TokenType::VAL:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default: // Ignore
                break;
        }
        advance();
    }
}

} // namespace stratos