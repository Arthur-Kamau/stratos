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
    std::unique_ptr<Expr> parseExpression(); // Parse a single expression (for interpolation)

private:
    const std::vector<Token>& tokens;
    int current = 0;
    std::unique_ptr<DocComment> pendingDocComment;  // Track pending doc comment

    // Declarations
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> varDeclaration(bool isPublic = false);
    std::unique_ptr<Stmt> fnDeclaration(const std::string& kind, bool isPublic = false, bool isAsync = false);
    std::unique_ptr<Stmt> classDeclaration();
    std::unique_ptr<Stmt> structDeclaration(); // New
    std::unique_ptr<Stmt> enumDeclaration();
    std::unique_ptr<Stmt> typeAliasDeclaration();
    std::unique_ptr<Stmt> packageDeclaration();
    std::unique_ptr<Stmt> useStatement();

    // Statements
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> ifStatement();
    // std::unique_ptr<Stmt> whenStatement(); // Removed
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> forStatement();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Stmt> breakStatement();
    std::unique_ptr<Stmt> continueStatement();
    std::unique_ptr<Stmt> deferStatement();
    std::unique_ptr<Stmt> block();
    std::unique_ptr<Stmt> expressionStatement();

    // Expressions
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> range(); // New: Range expression
    std::unique_ptr<Expr> pipe(); // New
    std::unique_ptr<Expr> logicOr();
    std::unique_ptr<Expr> logicAnd();
    std::unique_ptr<Expr> bitwiseOr();
    std::unique_ptr<Expr> bitwiseXor();
    std::unique_ptr<Expr> bitwiseAnd();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> bitwiseShift();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> call(); // Updated
    std::unique_ptr<Expr> finishCall(std::unique_ptr<Expr> callee);
    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr> cast(); // New: Parse cast expressions
    std::unique_ptr<Expr> whenExpression(); // New: Parse when expressions
    std::unique_ptr<Expr> parseInterpolatedString(const std::string& template_str); // Parse interpolated strings

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

    // Documentation
    void consumeDocComment();
    std::unique_ptr<DocComment> parseDocComment(const std::string& rawText);
    std::unique_ptr<DocComment> takePendingDoc();
    void saveTag(std::unique_ptr<DocComment>& doc, const std::string& tagName, const std::string& tagContent);

    // Helpers
    std::unique_ptr<Stmt> parseLambdaBody(); // New helper

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