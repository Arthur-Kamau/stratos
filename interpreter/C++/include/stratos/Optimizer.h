#ifndef STRATOS_OPTIMIZER_H
#define STRATOS_OPTIMIZER_H

#include "stratos/AST.h"
#include <memory>
#include <vector>

namespace stratos {

class Optimizer : public ASTVisitor {
public:
    void optimize(const std::vector<std::unique_ptr<Stmt>>& statements);

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
    // Helper to replace expression with optimized version (constant folding)
    // We can't easily replace unique_ptr in place without pointer to pointer.
    // For this prototype, we'll modify the AST nodes in place where possible, 
    // or store optimized result in a temp member.
    
    // Simplification: We will just optimize specific sub-trees like BinaryExpr.
    
    // Holds the result of evaluating a constant expression if applicable.
    // Type: 0=None, 1=Int, 2=Double, 3=String, 4=Bool
    struct ConstValue {
        int type = 0;
        int iVal = 0;
        double dVal = 0.0;
        std::string sVal;
        bool bVal = false;
    } lastConst;

    bool isConstant(Expr* expr);
};

} // namespace stratos

#endif // STRATOS_OPTIMIZER_H
