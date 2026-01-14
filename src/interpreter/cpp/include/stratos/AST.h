#ifndef STRATOS_AST_H
#define STRATOS_AST_H

#include "Token.h"
#include "DocComment.h"
#include <memory>
#include <vector>
#include <string>

namespace stratos {

// Forward declarations
class Expr;
class Stmt;
class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class VariableExpr;
class CallExpr;
class IndexExpr;
class GroupingExpr;
class CastExpr; // Forward declaration
class AwaitExpr; // Forward declaration
class MapLiteralExpr; // Forward declaration
class ArrayLiteralExpr; // Forward declaration
class LambdaExpr; // Forward declaration
class StructInitExpr; // Forward declaration
class VarDecl;
class FunctionDecl;
class ClassDecl;
class EnumDecl;
class BlockStmt;
class ExpressionStmt;
class PrintStmt;
class IfStmt;
class WhileStmt;
class ForStmt;
class ReturnStmt;
class PackageDecl;
class UseStmt;

// Visitor Interface
class ASTVisitor {
public:
    virtual void visit(BinaryExpr& expr) = 0;
    virtual void visit(UnaryExpr& expr) = 0;
    virtual void visit(LiteralExpr& expr) = 0;
    virtual void visit(VariableExpr& expr) = 0;
    virtual void visit(CallExpr& expr) = 0;
    virtual void visit(IndexExpr& expr) = 0;
    virtual void visit(GroupingExpr& expr) = 0;
    virtual void visit(CastExpr& expr) = 0; // Visit method for CastExpr
    virtual void visit(AwaitExpr& expr) = 0; // Visit method for AwaitExpr
    virtual void visit(MapLiteralExpr& expr) = 0; // Visit method for MapLiteralExpr
    virtual void visit(ArrayLiteralExpr& expr) = 0; // Visit method for ArrayLiteralExpr
    virtual void visit(LambdaExpr& expr) = 0; // Visit method for LambdaExpr
    virtual void visit(StructInitExpr& expr) = 0; // Visit method for StructInitExpr

    virtual void visit(VarDecl& stmt) = 0;
    virtual void visit(FunctionDecl& stmt) = 0;
    virtual void visit(ClassDecl& stmt) = 0;
    virtual void visit(EnumDecl& stmt) = 0;
    virtual void visit(PackageDecl& stmt) = 0;
    virtual void visit(UseStmt& stmt) = 0;
    virtual void visit(BlockStmt& stmt) = 0;
    virtual void visit(ExpressionStmt& stmt) = 0;
    virtual void visit(PrintStmt& stmt) = 0;
    virtual void visit(IfStmt& stmt) = 0;
    virtual void visit(WhileStmt& stmt) = 0;
    virtual void visit(ForStmt& stmt) = 0;
    virtual void visit(ReturnStmt& stmt) = 0;
};

// Base Node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// --- Expressions ---

class Expr : public ASTNode {};

class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class UnaryExpr : public Expr {
public:
    Token op;
    std::unique_ptr<Expr> right;

    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class LiteralExpr : public Expr {
public:
    std::string value; 
    TokenType type;

    LiteralExpr(std::string value, TokenType type) : value(value), type(type) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class VariableExpr : public Expr {
public:
    Token name;

    VariableExpr(Token name) : name(name) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class CallExpr : public Expr {
public:
    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<Expr>> arguments;

    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class IndexExpr : public Expr {
public:
    std::unique_ptr<Expr> object;
    std::unique_ptr<Expr> index;
    Token bracket;

    IndexExpr(std::unique_ptr<Expr> object, std::unique_ptr<Expr> index, Token bracket)
        : object(std::move(object)), index(std::move(index)), bracket(bracket) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class GroupingExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;

    GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class CastExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;
    Token typeToken; // The token representing the target type (e.g., INT, DOUBLE)
    bool isSafe;

    CastExpr(std::unique_ptr<Expr> expression, Token typeToken, bool isSafe = false)
        : expression(std::move(expression)), typeToken(typeToken), isSafe(isSafe) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class AwaitExpr : public Expr {
public:
    Token keyword;
    std::unique_ptr<Expr> expression;

    AwaitExpr(Token keyword, std::unique_ptr<Expr> expression)
        : keyword(keyword), expression(std::move(expression)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class MapLiteralExpr : public Expr {
public:
    std::vector<std::pair<std::string, std::unique_ptr<Expr>>> entries;

    MapLiteralExpr(std::vector<std::pair<std::string, std::unique_ptr<Expr>>> entries)
        : entries(std::move(entries)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ArrayLiteralExpr : public Expr {
public:
    std::vector<std::unique_ptr<Expr>> elements;

    ArrayLiteralExpr(std::vector<std::unique_ptr<Expr>> elements)
        : elements(std::move(elements)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class LambdaExpr : public Expr {
public:
    std::vector<Token> params;
    std::unique_ptr<Stmt> body; // Usually BlockStmt
    bool isAsync = false;

    LambdaExpr(std::vector<Token> params, std::unique_ptr<Stmt> body, bool isAsync = false)
        : params(std::move(params)), body(std::move(body)), isAsync(isAsync) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class StructInitExpr : public Expr {
public:
    Token name;
    std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields;

    StructInitExpr(Token name, std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields)
        : name(name), fields(std::move(fields)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

// --- Statements ---

class Stmt : public ASTNode {};

class VarDecl : public Stmt {
public:
    Token name;
    std::string typeName;
    std::unique_ptr<Expr> initializer;
    bool isMutable;
    std::unique_ptr<DocComment> documentation;

    VarDecl(Token name, std::string typeName, std::unique_ptr<Expr> initializer, bool isMutable)
        : name(name), typeName(typeName), initializer(std::move(initializer)), isMutable(isMutable), documentation(nullptr) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

struct Parameter {
    Token name;
    std::string type;
    std::unique_ptr<Expr> defaultValue;
    bool isVariadic;

    // Move constructor for Parameter
    Parameter(Token name, std::string type, std::unique_ptr<Expr> defaultValue = nullptr, bool isVariadic = false)
        : name(name), type(type), defaultValue(std::move(defaultValue)), isVariadic(isVariadic) {}
        
    // Default move operations
    Parameter(Parameter&&) = default;
    Parameter& operator=(Parameter&&) = default;
};

class FunctionDecl : public Stmt {
public:
    Token name;
    std::vector<Parameter> parameters;
    std::string returnType;
    std::unique_ptr<std::vector<std::unique_ptr<Stmt>>> body; // Can be null for interfaces
    std::unique_ptr<DocComment> documentation;
    bool isPublic;
    bool isAsync;

    FunctionDecl(Token name, std::vector<Parameter> parameters, std::string returnType, std::unique_ptr<std::vector<std::unique_ptr<Stmt>>> body, bool isPublic = false, bool isAsync = false)
        : name(name), parameters(std::move(parameters)), returnType(returnType), body(std::move(body)), documentation(nullptr), isPublic(isPublic), isAsync(isAsync) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ClassDecl : public Stmt {
public:
    Token name;
    std::unique_ptr<VariableExpr> superclass; // Optional
    std::vector<std::unique_ptr<Stmt>> methods; // Mix of VarDecl and FunctionDecl
    std::unique_ptr<DocComment> documentation;

    ClassDecl(Token name, std::unique_ptr<VariableExpr> superclass, std::vector<std::unique_ptr<Stmt>> methods)
        : name(name), superclass(std::move(superclass)), methods(std::move(methods)), documentation(nullptr) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class EnumDecl : public Stmt {
public:
    Token name;
    std::vector<Token> values; // Enum member names (e.g., RED, GREEN, BLUE)
    std::unique_ptr<DocComment> documentation;

    EnumDecl(Token name, std::vector<Token> values)
        : name(name), values(std::move(values)), documentation(nullptr) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class PackageDecl : public Stmt {
public:
    Token name;
    std::vector<std::unique_ptr<Stmt>> declarations;
    std::unique_ptr<DocComment> documentation;

    PackageDecl(Token name, std::vector<std::unique_ptr<Stmt>> declarations)
        : name(name), declarations(std::move(declarations)), documentation(nullptr) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class UseStmt : public Stmt {
public:
    Token moduleName; // The module being imported (e.g., "math", "log", "strings")

    UseStmt(Token moduleName)
        : moduleName(moduleName) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class BlockStmt : public Stmt {
public:
    std::vector<std::unique_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ExpressionStmt : public Stmt {
public:
    std::unique_ptr<Expr> expression;

    ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class PrintStmt : public Stmt {
public:
    std::unique_ptr<Expr> expression;

    PrintStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class IfStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class WhileStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ForStmt : public Stmt {
public:
    Token variable;         // Loop variable name (e.g., 'file')
    bool isMutable;         // true if 'var', false if 'val'
    std::string varType;    // Type annotation if provided
    std::unique_ptr<Expr> iterable;  // Expression to iterate over
    std::unique_ptr<Stmt> body;      // Loop body

    ForStmt(Token variable, bool isMutable, std::string varType,
            std::unique_ptr<Expr> iterable, std::unique_ptr<Stmt> body)
        : variable(variable), isMutable(isMutable), varType(varType),
          iterable(std::move(iterable)), body(std::move(body)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ReturnStmt : public Stmt {
public:
    Token keyword;
    std::unique_ptr<Expr> value;

    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(keyword), value(std::move(value)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

} // namespace stratos

#endif // STRATOS_AST_H