#ifndef STRATOS_INTERPRETER_H
#define STRATOS_INTERPRETER_H

#include "stratos/AST.h"
#include "stratos/NativeRegistry.h"
#include "stratos/GarbageCollector.h"
#include <any>
#include <optional>
#include <variant>
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

// Runtime value representation using std::variant for better performance and type safety
struct RuntimeValue {
    using ValueType = std::variant<
        std::monostate,                      // void
        int,                                 // int
        double,                              // double
        std::string,                         // string
        char,                                // char
        bool,                                // bool
        std::shared_ptr<ClassInstance>       // object
    >;

    ValueType value;
    std::string type; // "int", "double", "string", "char", "bool", "void", "object"

    // Default constructor: void value
    RuntimeValue() : value(std::monostate{}), type("void") {}

    // Backward compatibility constructor (accepts std::any for migration)
    RuntimeValue(std::any val, std::string t) : type(t) {
        // Convert std::any to std::variant based on type string
        if (t == "int") {
            value = std::any_cast<int>(val);
        } else if (t == "double") {
            value = std::any_cast<double>(val);
        } else if (t == "string") {
            value = std::any_cast<std::string>(val);
        } else if (t == "char") {
            value = std::any_cast<char>(val);
        } else if (t == "bool") {
            value = std::any_cast<bool>(val);
        } else if (t == "object") {
            value = std::any_cast<std::shared_ptr<ClassInstance>>(val);
        } else {
            value = std::monostate{};
        }
    }

    // Direct variant constructors for better performance
    RuntimeValue(int v) : value(v), type("int") {}
    RuntimeValue(double v) : value(v), type("double") {}
    RuntimeValue(const std::string& v) : value(v), type("string") {}
    RuntimeValue(const char* v) : value(std::string(v)), type("string") {}
    RuntimeValue(char v) : value(v), type("char") {}
    RuntimeValue(bool v) : value(v), type("bool") {}
    RuntimeValue(std::shared_ptr<ClassInstance> v) : value(v), type("object") {}

    // Helper methods to extract typed values with compile-time type safety
    int asInt() const {
        if (auto* v = std::get_if<int>(&value)) {
            return *v;
        }
        throw std::runtime_error("Type error: expected int, got " + type);
    }

    double asDouble() const {
        if (auto* v = std::get_if<double>(&value)) {
            return *v;
        }
        throw std::runtime_error("Type error: expected double, got " + type);
    }

    std::string asString() const {
        if (auto* v = std::get_if<std::string>(&value)) {
            return *v;
        }
        throw std::runtime_error("Type error: expected string, got " + type);
    }

    char asChar() const {
        if (auto* v = std::get_if<char>(&value)) {
            return *v;
        }
        throw std::runtime_error("Type error: expected char, got " + type);
    }

    bool asBool() const {
        if (auto* v = std::get_if<bool>(&value)) {
            return *v;
        }
        throw std::runtime_error("Type error: expected bool, got " + type);
    }

    std::shared_ptr<ClassInstance> asObject() const {
        if (auto* v = std::get_if<std::shared_ptr<ClassInstance>>(&value)) {
            if (!(*v)) {
                throw std::runtime_error("Null object reference");
            }
            return *v;
        }
        throw std::runtime_error("Type error: expected object, got " + type);
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

    // Cleanup after program execution (run GC, clear environments)
    void cleanup();

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
        // LIFETIME: Non-owning pointer to parent environment in the environments vector
        // Valid as long as parent environment exists in environments vector
        // Parent is never removed during execution, only appended
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

    // LIFETIME: Non-owning pointer to current environment in environments vector
    // SAFETY: environments.reserve(1000) called in constructor prevents reallocation
    // Valid as long as environments vector is not reallocated (guaranteed by reserve)
    // Points to an element in environments vector
    Environment* currentEnv;

    // Owns all Environment objects
    // SAFETY: Reserved capacity (1000) in constructor ensures no reallocation
    std::vector<std::unique_ptr<Environment>> environments;

    // Function storage
    struct Function {
        std::vector<Token> params;
        std::vector<std::string> paramTypes;
        std::string returnType;
        // LIFETIME: Non-owning reference to FunctionDecl::body (stored in AST)
        // SAFETY: The AST in mainStatements/moduleStatements keeps the referenced body alive
        // Valid as long as the owning FunctionDecl exists in mainStatements or moduleStatements
        // Uses std::optional<std::reference_wrapper<>> for safe non-owning semantics
        std::optional<std::reference_wrapper<std::unique_ptr<std::vector<std::unique_ptr<Stmt>>>>> body;
    };
    std::unordered_map<std::string, Function> functions;

    // Class storage
    struct Class {
        std::string name;
        // LIFETIME: Non-owning reference to ClassDecl::methods (stored in AST)
        // SAFETY: The AST in mainStatements/moduleStatements keeps the referenced methods alive
        // Valid as long as the owning ClassDecl exists in mainStatements or moduleStatements
        // Uses std::optional<std::reference_wrapper<>> for safe non-owning semantics
        std::optional<std::reference_wrapper<std::vector<std::unique_ptr<Stmt>>>> methods;
    };
    std::unordered_map<std::string, Class> classes;

    // Storage for parsed module statements to keep them alive
    std::vector<std::vector<std::unique_ptr<Stmt>>> moduleStatements;

    // Storage for main file statements to keep them alive
    std::vector<std::unique_ptr<Stmt>> mainStatements;

    // Module function registry: moduleName -> functionName -> FunctionDecl reference
    // LIFETIME: Non-owning references to FunctionDecl nodes in moduleStatements
    // SAFETY: The AST in moduleStatements vector keeps FunctionDecl objects alive
    // Valid as long as moduleStatements is not cleared (guaranteed during execution)
    // Uses std::reference_wrapper instead of raw pointers for safer semantics
    std::unordered_map<std::string, std::unordered_map<std::string, std::reference_wrapper<FunctionDecl>>> moduleFunctions;

    // Track which module we're currently loading
    std::string currentModuleName;

    // Track which module function is currently executing (for recursive calls)
    std::string currentExecutingModule;

    // Result of last expression evaluation
    RuntimeValue lastValue;

    // Garbage collector for cycle detection
    GarbageCollector gc;

    // Helper methods
    void enterScope();
    void exitScope();

    // GC helper methods
    void collectRoots(std::vector<RuntimeValue>& roots);
    void maybeCollectGarbage();

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
