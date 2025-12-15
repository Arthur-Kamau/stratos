#ifndef STRATOS_SEMANTIC_ANALYZER_H
#define STRATOS_SEMANTIC_ANALYZER_H

#include "AST.h"
#include "SymbolTable.h"
#include <vector>
#include <string>

namespace stratos {

class SemanticAnalyzer : public ASTVisitor {
public:
    SemanticAnalyzer();
    
    // Returns true if no errors were found
    bool analyze(const std::vector<std::unique_ptr<Stmt>>& statements);

    // Visitor Implementation
    void visit(BinaryExpr& expr) override;
    void visit(UnaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VariableExpr& expr) override;
    void visit(CallExpr& expr) override;
    void visit(GroupingExpr& expr) override;

    void visit(VarDecl& stmt) override;
    void visit(FunctionDecl& stmt) override;
    void visit(ClassDecl& stmt) override;
    void visit(PackageDecl& stmt) override;
    void visit(BlockStmt& stmt) override;
    void visit(PrintStmt& stmt) override;
    void visit(IfStmt& stmt) override;
    void visit(WhileStmt& stmt) override;
    void visit(ReturnStmt& stmt) override;

private:
    SymbolTable symbolTable;
    bool hadError = false;

    void error(const std::string& message); // Generic (no loc)
    void error(Token token, const std::string& message); // With loc
    void defineNativeFunctions(); // define print, etc.
};

} // namespace stratos

#endif // STRATOS_SEMANTIC_ANALYZER_H
