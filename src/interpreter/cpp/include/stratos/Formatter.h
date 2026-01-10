#ifndef STRATOS_FORMATTER_H
#define STRATOS_FORMATTER_H

#include "stratos/AST.h"
#include <string>
#include <sstream>
#include <vector>
#include <memory>

namespace stratos {

/**
 * Formats Stratos source code according to standard style
 *
 * Style Guidelines:
 * - 4 spaces for indentation (no tabs)
 * - Opening braces on same line
 * - Space after keywords (if, while, for, fn, class)
 * - No space before opening parenthesis in function calls
 * - Space around operators (=, +, -, *, /, ==, !=, etc.)
 * - Blank line between top-level declarations
 */
class Formatter : public ASTVisitor {
public:
    Formatter();

    /**
     * Format a list of statements
     * @param statements AST statements to format
     * @return Formatted source code
     */
    std::string format(const std::vector<std::unique_ptr<Stmt>>& statements);

    // Expression visitors
    void visit(BinaryExpr& expr) override;
    void visit(UnaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VariableExpr& expr) override;
    void visit(CallExpr& expr) override;
    void visit(IndexExpr& expr) override;
    void visit(GroupingExpr& expr) override;
    void visit(CastExpr& expr) override;
    void visit(MapLiteralExpr& expr) override;
    void visit(LambdaExpr& expr) override;
    void visit(StructInitExpr& expr) override;

    // Statement visitors
    void visit(VarDecl& stmt) override;
    void visit(FunctionDecl& stmt) override;
    void visit(ClassDecl& stmt) override;
    void visit(EnumDecl& stmt) override;
    void visit(PackageDecl& stmt) override;
    void visit(UseStmt& stmt) override;
    void visit(BlockStmt& stmt) override;
    void visit(ExpressionStmt& stmt) override;
    void visit(PrintStmt& stmt) override;
    void visit(IfStmt& stmt) override;
    void visit(WhileStmt& stmt) override;
    void visit(ForStmt& stmt) override;
    void visit(ReturnStmt& stmt) override;

private:
    std::ostringstream output_;
    int indentLevel_;
    bool needsBlankLine_;

    void indent();
    void newline();
    void space();
    void write(const std::string& text);
    void formatBlock(const std::vector<std::unique_ptr<Stmt>>& statements, bool addBraces = true);
    void formatArguments(const std::vector<std::unique_ptr<Expr>>& args);
};

} // namespace stratos

#endif // STRATOS_FORMATTER_H
