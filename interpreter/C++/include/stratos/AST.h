#ifndef STRATOS_AST_H
#define STRATOS_AST_H

#include "Token.h"
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
class VarDecl;
class FunctionDecl;
class ClassDecl;
class BlockStmt;
class PrintStmt;
class IfStmt;
class WhileStmt;
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

    virtual void visit(VarDecl& stmt) = 0;
    virtual void visit(FunctionDecl& stmt) = 0;
    virtual void visit(ClassDecl& stmt) = 0;
    virtual void visit(PackageDecl& stmt) = 0;
    virtual void visit(UseStmt& stmt) = 0;
    virtual void visit(BlockStmt& stmt) = 0;
    virtual void visit(PrintStmt& stmt) = 0;
    virtual void visit(IfStmt& stmt) = 0;
    virtual void visit(WhileStmt& stmt) = 0;
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

// --- Statements ---

class Stmt : public ASTNode {};

class VarDecl : public Stmt {
public:
    Token name;
    std::string typeName; 
    std::unique_ptr<Expr> initializer;
    bool isMutable;

    VarDecl(Token name, std::string typeName, std::unique_ptr<Expr> initializer, bool isMutable)
        : name(name), typeName(typeName), initializer(std::move(initializer)), isMutable(isMutable) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class FunctionDecl : public Stmt {
public:
    Token name;
    std::vector<Token> params; // Simplified: just names for now, or use a struct for param info
    std::vector<std::string> paramTypes;
    std::string returnType;
    std::unique_ptr<std::vector<std::unique_ptr<Stmt>>> body; // Can be null for interfaces

    FunctionDecl(Token name, std::vector<Token> params, std::vector<std::string> paramTypes, std::string returnType, std::unique_ptr<std::vector<std::unique_ptr<Stmt>>> body)
        : name(name), params(params), paramTypes(paramTypes), returnType(returnType), body(std::move(body)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ClassDecl : public Stmt {
public:
    Token name;
    std::unique_ptr<VariableExpr> superclass; // Optional
    std::vector<std::unique_ptr<Stmt>> methods; // Mix of VarDecl and FunctionDecl

    ClassDecl(Token name, std::unique_ptr<VariableExpr> superclass, std::vector<std::unique_ptr<Stmt>> methods)
        : name(name), superclass(std::move(superclass)), methods(std::move(methods)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class PackageDecl : public Stmt {
public:
    Token name;
    std::vector<std::unique_ptr<Stmt>> declarations;

    PackageDecl(Token name, std::vector<std::unique_ptr<Stmt>> declarations)
        : name(name), declarations(std::move(declarations)) {}

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