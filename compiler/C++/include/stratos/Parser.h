#ifndef STRATOS_PARSER_H
#define STRATOS_PARSER_H

#include "stratos/Token.h"
#include "stratos/AST.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace stratos {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    const std::vector<Token>& tokens;
    int current = 0;

    // Declarations
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> fnDeclaration(const std::string& kind);
    std::unique_ptr<Stmt> classDeclaration();
    std::unique_ptr<Stmt> namespaceDeclaration();
    
    // Statements
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whenStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Stmt> block();
    std::unique_ptr<Stmt> expressionStatement();

    // Expressions
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> pipe(); // New
    std::unique_ptr<Expr> logicOr();
    std::unique_ptr<Expr> logicAnd();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> call(); // Updated
    std::unique_ptr<Expr> finishCall(std::unique_ptr<Expr> callee);
    std::unique_ptr<Expr> primary();

    // Type Parsing
    std::string parseType();

    // Helpers
    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    Token consume(TokenType type, std::string message);

    void synchronize();
};

class ParseError : public std::runtime_error {
public:
    int line;
    int column;
    ParseError(const std::string& msg, int line, int column) 
        : std::runtime_error(msg), line(line), column(column) {}
};

} // namespace stratos

#endif // STRATOS_PARSER_H