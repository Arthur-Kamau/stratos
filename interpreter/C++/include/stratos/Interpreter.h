#ifndef STRATOS_INTERPRETER_H
#define STRATOS_INTERPRETER_H

#include "stratos/AST.h"
#include "stratos/NativeRegistry.h"
#include <any>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>

namespace stratos {

// Class instance (object) representation
struct ClassInstance {
    std::string className;
    std::unordered_map<std::string, struct RuntimeValue> fields;
};

// Runtime value representation
struct RuntimeValue {
    std::any value;
    std::string type; // "int", "double", "string", "bool", "void", "function", "object"

    RuntimeValue() : type("void") {}
    RuntimeValue(std::any val, std::string t) : value(val), type(t) {}

    // Helper methods to extract typed values
    int asInt() const { return std::any_cast<int>(value); }
    double asDouble() const { return std::any_cast<double>(value); }
    std::string asString() const { return std::any_cast<std::string>(value); }
    char asChar() const { return std::any_cast<char>(value); }
    bool asBool() const { return std::any_cast<bool>(value); }
    std::shared_ptr<ClassInstance> asObject() const {
        return std::any_cast<std::shared_ptr<ClassInstance>>(value);
    }
};

// Exception for return statements
class ReturnException : public std::runtime_error {
public:
    RuntimeValue value;
    ReturnException(RuntimeValue val) : std::runtime_error("return"), value(val) {}
};

// AST Interpreter - executes Stratos programs directly
class Interpreter : public ASTVisitor {
public:
    Interpreter();

    // Execute a program (takes ownership of statements)
    void execute(std::vector<std::unique_ptr<Stmt>>&& statements);

    // Call a function by name
    RuntimeValue callFunction(const std::string& name, const std::vector<RuntimeValue>& args);

    // Visitor Implementation
    void visit(BinaryExpr& expr) override;
    void visit(UnaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VariableExpr& expr) override;
    void visit(CallExpr& expr) override;
    void visit(IndexExpr& expr) override;
    void visit(GroupingExpr& expr) override;

    void visit(VarDecl& stmt) override;
    void visit(FunctionDecl& stmt) override;
    void visit(ClassDecl& stmt) override;
    void visit(PackageDecl& stmt) override;
    void visit(UseStmt& stmt) override;
    void visit(BlockStmt& stmt) override;
    void visit(ExpressionStmt& stmt) override;
    void visit(PrintStmt& stmt) override;
    void visit(IfStmt& stmt) override;
    void visit(WhileStmt& stmt) override;
    void visit(ReturnStmt& stmt) override;

private:
    // Runtime environment (variable storage)
    struct Environment {
        std::unordered_map<std::string, RuntimeValue> variables;
        Environment* parent = nullptr;

        void define(const std::string& name, RuntimeValue value) {
            variables[name] = value;
        }

        RuntimeValue get(const std::string& name) {
            if (variables.count(name)) {
                return variables[name];
            }
            if (parent) {
                return parent->get(name);
            }
            throw std::runtime_error("Undefined variable: " + name);
        }

        void assign(const std::string& name, RuntimeValue value) {
            if (variables.count(name)) {
                variables[name] = value;
                return;
            }
            if (parent) {
                parent->assign(name, value);
                return;
            }
            throw std::runtime_error("Undefined variable: " + name);
        }
    };

    Environment* currentEnv;
    std::vector<std::unique_ptr<Environment>> environments;

    // Function storage
    struct Function {
        std::vector<Token> params;
        std::vector<std::string> paramTypes;
        std::string returnType;
        std::unique_ptr<std::vector<std::unique_ptr<Stmt>>>* body;
    };
    std::unordered_map<std::string, Function> functions;

    // Class storage
    struct Class {
        std::string name;
        std::vector<std::unique_ptr<Stmt>>* methods; // Pointer to methods in ClassDecl
    };
    std::unordered_map<std::string, Class> classes;

    // Storage for parsed module statements to keep them alive
    std::vector<std::vector<std::unique_ptr<Stmt>>> moduleStatements;

    // Storage for main file statements to keep them alive
    std::vector<std::unique_ptr<Stmt>> mainStatements;

    // Module function registry: moduleName -> functionName -> FunctionDecl*
    std::unordered_map<std::string, std::unordered_map<std::string, FunctionDecl*>> moduleFunctions;

    // Track which module we're currently loading
    std::string currentModuleName;

    // Track which module function is currently executing (for recursive calls)
    std::string currentExecutingModule;

    // Result of last expression evaluation
    RuntimeValue lastValue;

    // Helper methods
    void enterScope();
    void exitScope();

    RuntimeValue evaluateNativeCall(const std::string& moduleName,
                                    const std::string& functionName,
                                    const std::vector<RuntimeValue>& args);

    RuntimeValue callModuleFunction(const std::string& moduleName,
                                    const std::string& functionName,
                                    const std::vector<RuntimeValue>& args,
                                    FunctionDecl* funcDecl);

    RuntimeValue instantiateClass(const std::string& className, const std::vector<RuntimeValue>& args);

    bool isTruthy(const RuntimeValue& value);

    void error(const std::string& message);
};

} // namespace stratos

#endif // STRATOS_INTERPRETER_H
