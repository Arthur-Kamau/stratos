#ifndef STRATOS_RUNTIME_IR_GENERATOR_H
#define STRATOS_RUNTIME_IR_GENERATOR_H

#include "stratos/IRGenerator.h"

namespace stratos {

class RuntimeIRGenerator : public IRGenerator {
public:
    RuntimeIRGenerator(const std::string& filename);

    // Override to inject GC runtime declarations
    void generate(const std::vector<std::unique_ptr<Stmt>>& statements); // Hide base? No, it's not virtual.
    // Actually generate in base is not virtual, but we can just use the new one if we instantiate RuntimeIRGenerator.
    // However, to use polymorphism properly, base generate() calls virtual visit() methods.
    // But generate() mainly does setup. We need to override the setup part.
    // Since it's not virtual, we'll just define a new one or rely on the caller calling the derived one.
    // Better: Define a new entry point or shadow it. 
    // Let's shadow it for now as the CLI will instantiate RuntimeIRGenerator directly.

    void generateRuntime(const std::vector<std::unique_ptr<Stmt>>& statements);

    // Override Visitors to inject GC hooks
    void visit(FunctionDecl& stmt) override;
    void visit(ReturnStmt& stmt) override;

private:
    // Stack of vectors to keep track of pointer variables in each scope for manual freeing
    std::vector<std::vector<VarInfo>> scopePtrVars;

    // Helper to determine if a Stratos type is a pointer type requiring 'free'
    bool isPointerType(const std::string& stratosType) const;
};

} // namespace stratos

#endif // STRATOS_RUNTIME_IR_GENERATOR_H
