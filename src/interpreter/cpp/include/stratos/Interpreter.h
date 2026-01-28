#ifndef STRATOS_INTERPRETER_H
#define STRATOS_INTERPRETER_H

#include "stratos/AST.h"
#include "stratos/NativeRegistry.h"
#include "stratos/GarbageCollector.h"
#include "stratos/HttpServer.h"
#include <any>
#include <optional>
#include <variant>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

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
        std::shared_ptr<ClassInstance>,      // object
        std::any                             // array (stores vector<string>, vector<int>, etc.)
    >;

    ValueType value;
    std::string type; // "int", "double", "string", "char", "bool", "void", "object", "array<T>"

    // Default constructor: void value
    RuntimeValue() : value(std::monostate{}), type("void") {}

    // Backward compatibility constructor (accepts std::any for migration)
    RuntimeValue(std::any val, std::string t) : type(t) {
        // Convert std::any to std::variant based on type string
        try {
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
            } else if (t.starts_with("array")) {
                // For arrays, store the std::any directly
                value = val;
            } else if (t.starts_with("map")) {
                // For maps, store the std::any directly
                value = val;
            } else if (t == "function") {
                // For functions/closures, store the std::any directly
                value = val;
            } else if (t == "any") {
                // For any type, store the std::any directly
                value = val;
            } else if (t == "void") {
                value = std::monostate{};
            } else {
                value = std::monostate{};
            }
        } catch (const std::bad_any_cast& e) {
            // Failed to cast - set to void
            value = std::monostate{};
            type = "void";
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

class BreakException : public std::runtime_error {
public:
    BreakException() : std::runtime_error("break") {}
};

class ContinueException : public std::runtime_error {
public:
    ContinueException() : std::runtime_error("continue") {}
};

// AST Interpreter - executes Stratos programs directly
class Interpreter : public ASTVisitor {
public:
    Interpreter();
    explicit Interpreter(const std::string& projectRoot);
    ~Interpreter();

    // Execute a program (takes ownership of statements)
    void execute(std::vector<std::unique_ptr<Stmt>>&& statements);

    // Cleanup after program execution (run GC, clear environments)
    void cleanup();

    // Call a function by name
    RuntimeValue callFunction(const std::string& name, const std::vector<RuntimeValue>& args);

    // HTTP handler execution (public for HttpServerManager access)
    void executeHttpHandler(const RuntimeValue& handler, HttpRequestInternal& req, HttpResponseInternal& res);

    // Visitor Implementation
    void visit(BinaryExpr& expr) override;
    void visit(UnaryExpr& expr) override;
    void visit(LiteralExpr& expr) override;
    void visit(VariableExpr& expr) override;
    void visit(CallExpr& expr) override;
    void visit(IndexExpr& expr) override;
    void visit(GroupingExpr& expr) override;
    void visit(CastExpr& expr) override; // Visit method for CastExpr
    void visit(AwaitExpr& expr) override; // Visit method for AwaitExpr
    void visit(MapLiteralExpr& expr) override; // Visit method for MapLiteralExpr
    void visit(ArrayLiteralExpr& expr) override; // Visit method for ArrayLiteralExpr
    void visit(LambdaExpr& expr) override; // Visit method for LambdaExpr
    void visit(StructInitExpr& expr) override; // Visit method for StructInitExpr
    void visit(WhenExpr& expr) override; // Visit method for WhenExpr
    void visit(InterpolatedStringExpr& expr) override; // Visit method for InterpolatedStringExpr

    void visit(VarDecl& stmt) override;
    void visit(FunctionDecl& stmt) override;
    void visit(ClassDecl& stmt) override;
    void visit(EnumDecl& stmt) override;
    void visit(TypeAliasDecl& stmt) override;
    void visit(PackageDecl& stmt) override;
    void visit(UseStmt& stmt) override;
    void visit(BlockStmt& stmt) override;
    void visit(ExpressionStmt& stmt) override;
    void visit(PrintStmt& stmt) override;
    void visit(IfStmt& stmt) override;
    void visit(WhileStmt& stmt) override;
    void visit(ForStmt& stmt) override;
    void visit(ReturnStmt& stmt) override;
    void visit(BreakStmt& stmt) override;
    void visit(ContinueStmt& stmt) override;
    void visit(DeferStmt& stmt) override;

private:
    // Forward declaration
    struct Environment;

    // Closure for lambdas/first-class functions
    struct Closure {
        std::vector<Token> params;
        Stmt* body; // Non-owning pointer to AST
        Environment* env; // Captured environment
    };

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
        std::vector<const Parameter*> parameters;
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
        std::string moduleName;  // Module this class belongs to (for native function resolution)
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

    // Project root directory for module resolution
    std::string projectRoot;

    // Result of last expression evaluation
    RuntimeValue lastValue;

    // Garbage collector for cycle detection
    GarbageCollector gc;

    // Deferred statements (Go-style defer)
    std::vector<Stmt*> deferredStatements;

    // Thread management for concurrent.go()
    mutable std::mutex interpreterMutex;
    std::vector<std::thread> goroutines;
    bool shuttingDown = false;

    // Worker pool support
    struct WorkerPool {
        std::vector<std::thread> workers;
        std::queue<RuntimeValue> jobQueue;
        std::shared_ptr<ClassInstance> resultsChannel;
        std::mutex poolMutex;
        std::condition_variable jobAvailable;
        std::condition_variable jobDone;
        bool closed = false;
        int activeWorkers = 0;
    };
    std::vector<std::shared_ptr<WorkerPool>> workerPools;

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

    RuntimeValue executeCallback(const RuntimeValue& closureValue, const std::vector<RuntimeValue>& args);

    // Concurrency support
    void spawnGoroutine(const RuntimeValue& closureValue);
    RuntimeValue createWorkerPool(int numWorkers, const RuntimeValue& workerFunc);
    RuntimeValue createPipeline(const RuntimeValue& inputChannel, const std::vector<RuntimeValue>& stages);
    void joinAllGoroutines();

    bool isTruthy(const RuntimeValue& value);

    void error(const std::string& message);
};

} // namespace stratos

#endif // STRATOS_INTERPRETER_H
