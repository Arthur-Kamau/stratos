#ifndef STRATOS_IR_GENERATOR_H
#define STRATOS_IR_GENERATOR_H

#include "stratos/AST.h"
#include "stratos/NativeRegistry.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_map>

namespace stratos {

struct IRValue {
    std::string reg;  // The register name (e.g., "%t1") or literal ("3.14")
    std::string type; // The LLVM type (e.g., "i32", "double")
};

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
    void visit(PackageDecl& stmt) override;
    void visit(UseStmt& stmt) override;
    void visit(BlockStmt& stmt) override;
    void visit(PrintStmt& stmt) override;
    void visit(IfStmt& stmt) override;
    void visit(WhileStmt& stmt) override;
    void visit(ReturnStmt& stmt) override;

private:
    std::ofstream out;
    int regCount = 0;
    int labelCount = 0;
    int strCount = 0;

    IRValue lastVal; // Holds result of last expression

    // Scopes track variable register locations AND their types
    struct VarInfo {
        std::string ptr; // Stack pointer register
        std::string type; // LLVM type
    };
    std::vector<std::unordered_map<std::string, VarInfo>> scopes;

    // Cache for string literals to generate global constants
    // Content -> GlobalName (e.g. "Hello" -> @.str0)
    std::unordered_map<std::string, std::string> stringLiterals;

    // OOP Support - Class metadata
    struct FieldInfo {
        std::string name;
        std::string type; // Stratos type
        int index;        // Index in struct
    };

    struct MethodInfo {
        std::string name;
        std::vector<std::string> paramTypes;
        std::string returnType;
        bool hasBody;
    };

    struct ClassInfo {
        std::string name;
        std::vector<FieldInfo> fields;
        std::vector<MethodInfo> methods;
        std::string superclass;
        bool isInterface;

        // Constructor info
        bool hasConstructor = false;
        std::vector<std::string> constructorParams;
    };

    std::unordered_map<std::string, ClassInfo> classes;
    std::string currentClass; // Track which class we're currently generating

    void enterScope();
    void exitScope();
    VarInfo getVarInfo(const std::string& name);
    void defineVar(const std::string& name, const std::string& ptr, const std::string& type);

    std::string nextReg();
    std::string nextLabel();
    std::string getOrCreateStringLiteral(const std::string& text);
    std::string getLLVMType(const std::string& stratosType);
    std::string getDefaultValue(const std::string& llvmType);

    void emit(const std::string& code);
    void emitRaw(const std::string& code);

    // Helper to generate explicit main wrapper
    void generateMainWrapper(const std::vector<Stmt*>& topLevelStmts);

    // OOP helpers
    void generateClassStructs();
    void generateConstructor(const std::string& className, ClassDecl& classDecl);
    void generateMethod(const std::string& className, FunctionDecl& method);
    std::string getStructType(const std::string& className);
    int getFieldIndex(const std::string& className, const std::string& fieldName);
    ClassInfo* getClassInfo(const std::string& className);

    // Native function helpers
    bool isNativeFunction(const std::string& moduleName, const std::string& functionName);
    void generateNativeCall(const std::string& moduleName, const std::string& functionName,
                           const std::vector<IRValue>& args);

    std::string currentModule; // Track current module/package context
};

} // namespace stratos

#endif // STRATOS_IR_GENERATOR_H