#ifndef STRATOS_DOC_EXTRACTOR_H
#define STRATOS_DOC_EXTRACTOR_H

#include "stratos/AST.h"
#include "stratos/DocModel.h"
#include <memory>
#include <string>

namespace stratos {

/**
 * Extracts documentation from AST nodes
 * Implements visitor pattern to traverse AST and build documentation model
 */
class DocExtractor : public ASTVisitor {
public:
    DocExtractor(const std::string& sourceFile);

    /**
     * Extract documentation from AST statements
     * @param statements AST statements to process
     * @return Complete documentation structure
     */
    std::unique_ptr<Documentation> extract(
        const std::vector<std::unique_ptr<Stmt>>& statements
    );

    // Expression visitors (no-op for documentation)
    void visit(BinaryExpr& expr) override {}
    void visit(UnaryExpr& expr) override {}
    void visit(LiteralExpr& expr) override {}
    void visit(VariableExpr& expr) override {}
    void visit(CallExpr& expr) override {}
    void visit(IndexExpr& expr) override {}
    void visit(GroupingExpr& expr) override {}
    void visit(CastExpr& expr) override {}
    void visit(AwaitExpr& expr) override {}
    void visit(MapLiteralExpr& expr) override {}
    void visit(ArrayLiteralExpr& expr) override {}
    void visit(LambdaExpr& expr) override {}
    void visit(StructInitExpr& expr) override {}
    void visit(WhenExpr& expr) override {}

    // Statement visitors
    void visit(VarDecl& stmt) override;
    void visit(FunctionDecl& stmt) override;
    void visit(ClassDecl& stmt) override;
    void visit(EnumDecl& stmt) override;
    void visit(TypeAliasDecl& stmt) override {}
    void visit(PackageDecl& stmt) override;
    void visit(UseStmt& stmt) override;
    void visit(BlockStmt& stmt) override {}
    void visit(ExpressionStmt& stmt) override {}
    void visit(PrintStmt& stmt) override {}
    void visit(IfStmt& stmt) override {}
    void visit(WhileStmt& stmt) override {}
    void visit(ForStmt& stmt) override {}
    void visit(ReturnStmt& stmt) override;
    void visit(BreakStmt& stmt) override;
    void visit(ContinueStmt& stmt) override;

private:
    std::string sourceFile_;
    std::unique_ptr<Documentation> documentation_;
    DocPackage* currentPackage_;
    DocClass* currentClass_;

    void ensureDefaultPackage();
};

} // namespace stratos

#endif // STRATOS_DOC_EXTRACTOR_H
