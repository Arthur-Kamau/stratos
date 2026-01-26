#ifndef STRATOS_SEMANTIC_ANALYZER_H
#define STRATOS_SEMANTIC_ANALYZER_H

#include "AST.h"
#include "SymbolTable.h"
#include <vector>
#include <string>

namespace stratos {

class SemanticAnalyzer : public ASTVisitor {
public:
    SemanticAnalyzer(std::string projectRoot = ".");
    
    // Returns true if no errors were found
    bool analyze(const std::vector<std::unique_ptr<Stmt>>& statements);

    // Visitor Implementation
    void visit(BinaryExpr& expr) override;
    void visit(UnaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VariableExpr& expr) override;
    void visit(CallExpr& expr) override;
    void visit(IndexExpr& expr) override;
    void visit(GroupingExpr& expr) override;
    void visit(CastExpr& expr) override;
    void visit(AwaitExpr& expr) override;
    void visit(MapLiteralExpr& expr) override;
    void visit(ArrayLiteralExpr& expr) override;
    void visit(LambdaExpr& expr) override;
    void visit(StructInitExpr& expr) override;
    void visit(WhenExpr& expr) override;

    void visit(VarDecl& stmt) override;
    void visit(FunctionDecl& stmt) override;
    void visit(ClassDecl& stmt) override;
    void visit(EnumDecl& stmt) override;
    void visit(TypeAliasDecl& stmt) override;
    void visit(PackageDecl& stmt) override;
    void visit(UseStmt& stmt) override;
    void visit(BlockStmt& stmt) override;
    void visit(ExpressionStmt& stmt) override;
    void visit(PrintStmt& stmt) override;
    void visit(IfStmt& stmt) override;
    void visit(WhileStmt& stmt) override;
    void visit(ForStmt& stmt) override;
    void visit(ReturnStmt& stmt) override;
    void visit(BreakStmt& stmt) override;
    void visit(ContinueStmt& stmt) override;

private:
    SymbolTable symbolTable;
    bool hadError = false;
    std::vector<std::string> loadedModules; // Track already loaded modules
    std::string lastExprType; // Track the type of the last evaluated expression
    std::string currentClassName; // Track current class name for 'this' context
    bool currentFunctionIsAsync = false; // Track if we're inside an async function/lambda
    int loopDepth = 0; // Track if we are inside a loop

    std::string projectRoot;
    std::unordered_map<std::string, std::unordered_map<std::string, Symbol>> classMembers; // Store members of classes for access control 

    void error(const std::string& message); // Generic (no loc)
    void error(Token token, const std::string& message); // With loc
    void defineNativeFunctions(); // define print, etc.
    void loadModule(const std::string& moduleName); // Load module definitions
    std::string inferType(Expr* expr); // Infer the type of an expression
};

} // namespace stratos

#endif // STRATOS_SEMANTIC_ANALYZER_H
