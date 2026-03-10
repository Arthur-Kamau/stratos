#ifndef STRATOS_STUI_TRANSPILER_H
#define STRATOS_STUI_TRANSPILER_H

#include "STUIAST.h"
#include "AST.h"
#include "Token.h"
#include <memory>
#include <vector>
#include <string>

namespace stratos {
namespace stui {

// Transpiles STUI AST into Stratos AST (std/gui calls).
// The output can be fed directly into the normal Stratos pipeline
// (SemanticAnalyzer → Interpreter or IRGenerator).
class STUITranspiler {
public:
    STUITranspiler();

    // Transpile an entire STUI file into Stratos statements
    std::vector<std::unique_ptr<Stmt>> transpile(const STUIFile& file);

private:
    int syntheticLine_ = 0; // For generating synthetic tokens

    // Generate Stratos AST from STUI components
    std::vector<std::unique_ptr<Stmt>> transpileImports(const STUIFile& file);
    std::unique_ptr<Stmt> transpileComponent(const ComponentDecl& component);

    // Generate the main() function that creates the app and runs it
    std::unique_ptr<Stmt> generateMain(const STUIFile& file);

    // Widget tree → std/gui calls
    std::unique_ptr<Expr> transpileWidget(const WidgetNode& widget);
    std::unique_ptr<Expr> transpileWidget(const WidgetNode& widget, std::vector<std::unique_ptr<Stmt>>& setupStmts);
    std::unique_ptr<Expr> transpileWidgetArgs(const WidgetNode& widget);
    std::unique_ptr<Expr> transpileWidgetProps(const WidgetNode& widget);
    std::unique_ptr<Expr> transpileWidgetChildren(const WidgetNode& widget);

    int tempVarCounter_ = 0;
    std::string nextTempVar();

    // Expression conversion: STUI expr → Stratos expr
    std::unique_ptr<Expr> transpileExpr(const STUIExpr& expr);

    // State declarations → gui.State() calls
    std::unique_ptr<Stmt> transpileStateDecl(const StateDecl& state);

    // Helper: create synthetic tokens for generated AST nodes
    Token makeToken(TokenType type, const std::string& lexeme);
    Token makeIdentifier(const std::string& name);
    Token makeString(const std::string& value);
    Token makeNumber(const std::string& value);

    // Helper: create common AST patterns
    std::unique_ptr<Expr> makeCall(const std::string& callee, std::vector<std::unique_ptr<Expr>> args);
    std::unique_ptr<Expr> makeMemberCall(const std::string& object, const std::string& method, std::vector<std::unique_ptr<Expr>> args);
    std::unique_ptr<Expr> makeVar(const std::string& name);
    std::unique_ptr<Expr> makeString(const std::string& value, bool dummy);
    std::unique_ptr<Expr> makeNumber(int value);
    std::unique_ptr<Expr> makeNumber(double value);
    std::unique_ptr<Expr> makeMapLiteral(std::vector<std::pair<std::string, std::unique_ptr<Expr>>> entries);
    std::unique_ptr<Expr> makeArrayLiteral(std::vector<std::unique_ptr<Expr>> elements);
    std::unique_ptr<Stmt> makeVarDecl(const std::string& name, std::unique_ptr<Expr> init, bool isMutable = false);
    std::unique_ptr<Stmt> makeExprStmt(std::unique_ptr<Expr> expr);
};

} // namespace stui
} // namespace stratos

#endif // STRATOS_STUI_TRANSPILER_H
