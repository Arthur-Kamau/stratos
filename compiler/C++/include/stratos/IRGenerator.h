#ifndef STRATOS_IR_GENERATOR_H
#define STRATOS_IR_GENERATOR_H

#include "stratos/AST.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_map>

namespace stratos {

class IRGenerator : public ASTVisitor {
public:
    IRGenerator(const std::string& filename);
    void generate(const std::vector<std::unique_ptr<Stmt>>& statements);

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
    void visit(NamespaceDecl& stmt) override;
    void visit(BlockStmt& stmt) override;
    void visit(PrintStmt& stmt) override;
    void visit(IfStmt& stmt) override;
    void visit(WhileStmt& stmt) override;
    void visit(ReturnStmt& stmt) override;

private:
    std::ofstream out;
    int regCount = 0;       // Temporary register counter (%0, %1...)
    int labelCount = 0;     // Label counter (L0, L1...)
    std::string lastReg;    // The register holding the result of the last expression

    // We need a simple map to track variable registers
    // In a real compiler, this would be more complex (SymbolTable with allocas)
    // Map: variableName -> stackPtrRegister (e.g., %x)
    std::vector<std::unordered_map<std::string, std::string>> scopes;

    void enterScope();
    void exitScope();
    std::string getVarPtr(const std::string& name);
    void defineVar(const std::string& name, const std::string& ptr);

    std::string nextReg();
    std::string nextLabel();
    void emit(const std::string& code);
    void emitRaw(const std::string& code);
};

} // namespace stratos

#endif // STRATOS_IR_GENERATOR_H
