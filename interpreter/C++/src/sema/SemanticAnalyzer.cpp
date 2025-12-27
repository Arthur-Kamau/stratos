#include "stratos/SemanticAnalyzer.h"
#include "stratos/Lexer.h"
#include "stratos/Parser.h"
#include "stratos/NativeRegistry.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace stratos {

SemanticAnalyzer::SemanticAnalyzer() {
    defineNativeFunctions();
}

void SemanticAnalyzer::defineNativeFunctions() {
    // Define 'print' as a native function
    // fn print(any) -> void
    symbolTable.define(Symbol::Function("print", {"any"}, "void"));

    // Define standard types/constructors
    symbolTable.define(Symbol::Variable("Some", "constructor", false)); // Mock for now
    symbolTable.define(Symbol::Variable("None", "Optional", false));
    
    // Internal intrinsics
    symbolTable.define(Symbol::Function("__if_expr", {"bool", "any", "any"}, "any"));
}

bool SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<Stmt>>& statements) {
    hadError = false;
    for (const auto& stmt : statements) {
        if (stmt) stmt->accept(*this);
    }
    return !hadError;
}

void SemanticAnalyzer::error(const std::string& message) {
    std::cerr << "[Error] 0:0: " << message << std::endl; // Fallback
    hadError = true;
}

void SemanticAnalyzer::error(Token token, const std::string& message) {
    std::cerr << "[Error] " << token.line << ":" << token.column << ": " << message << std::endl;
    hadError = true;
}

// --- Expressions ---

void SemanticAnalyzer::visit(BinaryExpr& expr) {
    if (expr.op.type == TokenType::DOT) {
        expr.left->accept(*this);
        return; 
    }

    expr.left->accept(*this);
    expr.right->accept(*this);
    // TODO: Type checking (e.g., ensure left/right are numbers for +)
}

void SemanticAnalyzer::visit(UnaryExpr& expr) {
    expr.right->accept(*this);
}

void SemanticAnalyzer::visit(LiteralExpr& expr) {
    // Literals are self-contained
}

void SemanticAnalyzer::visit(VariableExpr& expr) {
    // 1. Resolution: Variable must exist
    if (!symbolTable.resolve(expr.name.lexeme)) {
        error(expr.name, "Undefined variable '" + expr.name.lexeme + "'.");
    }
}

void SemanticAnalyzer::visit(CallExpr& expr) {
    // Check if this is a module function call (e.g., math.sqrt())
    if (auto* binExpr = dynamic_cast<BinaryExpr*>(expr.callee.get())) {
        if (binExpr->op.type == TokenType::DOT) {
            if (auto* leftVar = dynamic_cast<VariableExpr*>(binExpr->left.get())) {
                if (auto* rightVar = dynamic_cast<VariableExpr*>(binExpr->right.get())) {
                    std::string moduleName = leftVar->name.lexeme;
                    std::string functionName = rightVar->name.lexeme;

                    // Check if this is a native function call via NativeRegistry
                    auto& registry = NativeRegistry::getInstance();
                    if (registry.isNative(moduleName, functionName)) {
                        // Check if we have type signature for this function
                        if (registry.hasSignature(moduleName, functionName)) {
                            auto signature = registry.getSignature(moduleName, functionName);

                            // Check argument count
                            if (expr.arguments.size() != signature.paramTypes.size()) {
                                error(rightVar->name, "Function '" + functionName + "' expects " +
                                     std::to_string(signature.paramTypes.size()) + " arguments, got " +
                                     std::to_string(expr.arguments.size()));
                                return;
                            }

                            // Check argument types
                            for (size_t i = 0; i < expr.arguments.size(); i++) {
                                std::string argType = inferType(expr.arguments[i].get());
                                std::string expectedType = signature.paramTypes[i];

                                if (argType != expectedType && argType != "unknown") {
                                    error(rightVar->name, "Function '" + moduleName + "." + functionName +
                                         "' expects argument " + std::to_string(i + 1) +
                                         " to be of type '" + expectedType + "', found '" + argType + "'");
                                }
                            }
                        } else {
                            // No signature available, just validate arguments exist
                            for (const auto& arg : expr.arguments) {
                                arg->accept(*this);
                            }
                        }
                        return;
                    }
                }
            }
        }
    }

    // Not a module call, process as regular function call
    expr.callee->accept(*this); // Verify function exists (if it's a variable access)

    // TODO: Advanced check - ensure callee is actually a callable type

    for (const auto& arg : expr.arguments) {
        arg->accept(*this);
    }
}

void SemanticAnalyzer::visit(IndexExpr& expr) {
    // Validate object and index expressions
    expr.object->accept(*this);
    expr.index->accept(*this);
}

void SemanticAnalyzer::visit(GroupingExpr& expr) {
    expr.expression->accept(*this);
}

// --- Statements ---

void SemanticAnalyzer::visit(VarDecl& stmt) {
    // 1. Analyze initializer first (so it can't refer to the variable being declared)
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    }

    // 2. Define variable in current scope
    std::string type = stmt.typeName.empty() ? "inferred" : stmt.typeName;
    Symbol symbol = Symbol::Variable(stmt.name.lexeme, type, stmt.isMutable);
    
    if (!symbolTable.define(symbol)) {
        error(stmt.name, "Variable '" + stmt.name.lexeme + "' is already defined in this scope.");
    }
}

void SemanticAnalyzer::visit(FunctionDecl& stmt) {
    // 1. Define function name in current scope (so it can recurse)
    Symbol funcSymbol = Symbol::Function(stmt.name.lexeme, stmt.paramTypes, stmt.returnType);
    if (!symbolTable.define(funcSymbol)) {
        error(stmt.name, "Function '" + stmt.name.lexeme + "' is already defined.");
    }

    // 2. Enter new scope for function body
    symbolTable.enterScope();

    // 3. Define parameters as variables
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        std::string pType = stmt.paramTypes[i];
        // Params are usually immutable (val) by default in many modern langs, let's assume so or var
        Symbol paramSym = Symbol::Variable(stmt.params[i].lexeme, pType, false); // val
        symbolTable.define(paramSym);
    }

    // 4. Analyze body
    if (stmt.body) {
        for (const auto& s : *stmt.body) {
            if (s) s->accept(*this);
        }
    }

    // 5. Exit scope
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(ClassDecl& stmt) {
    if (!symbolTable.define(Symbol{stmt.name.lexeme, SymbolKind::CLASS, stmt.name.lexeme, false})) {
         error(stmt.name, "Class '" + stmt.name.lexeme + "' is already defined.");
    }

    symbolTable.enterScope();
    // Define 'this'
    symbolTable.define(Symbol::Variable("this", stmt.name.lexeme, false));
    
    for (const auto& member : stmt.methods) {
        if (member) member->accept(*this);
    }
    
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(PackageDecl& stmt) {
    // Package declaration (like "package main;") just declares what package
    // this file belongs to - it doesn't create a symbol in the namespace.
    // Don't define the package name as a symbol to avoid conflicts.
    // NOTE: Package imports (via 'use' keyword) would be different.

    // Process declarations within the package without creating a new scope
    // or defining the package name as a symbol
    for (const auto& s : stmt.declarations) {
        if (s) s->accept(*this);
    }
}

void SemanticAnalyzer::visit(UseStmt& stmt) {
    // Load the module if not already loaded
    std::string moduleName = stmt.moduleName.lexeme;

    // Check if already loaded
    if (std::find(loadedModules.begin(), loadedModules.end(), moduleName) != loadedModules.end()) {
        return; // Already loaded
    }

    // Load the module
    loadModule(moduleName);
    loadedModules.push_back(moduleName);
}

void SemanticAnalyzer::visit(BlockStmt& stmt) {
    symbolTable.enterScope();
    for (const auto& s : stmt.statements) {
        if (s) s->accept(*this);
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(PrintStmt& stmt) {
    stmt.expression->accept(*this);
}

void SemanticAnalyzer::visit(IfStmt& stmt) {
    stmt.condition->accept(*this);
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) stmt.elseBranch->accept(*this);
}

void SemanticAnalyzer::visit(WhileStmt& stmt) {
    stmt.condition->accept(*this);
    stmt.body->accept(*this);
}

void SemanticAnalyzer::visit(ReturnStmt& stmt) {
    if (stmt.value) {
        stmt.value->accept(*this);
    }
    // TODO: Check if return type matches function signature
}

void SemanticAnalyzer::loadModule(const std::string& moduleName) {
    namespace fs = std::filesystem;

    // Possible module file locations
    std::vector<std::string> searchPaths = {
        // Current directory
        "std/" + moduleName + "/init.st",
        "std/encoding/" + moduleName + "/init.st",

        // Build directory
        "build/std/" + moduleName + "/init.st",
        "build/std/encoding/" + moduleName + "/init.st",

        // One level up (from samples/ or similar)
        "../std/" + moduleName + "/init.st",
        "../std/encoding/" + moduleName + "/init.st",
        "../build/std/" + moduleName + "/init.st",
        "../build/std/encoding/" + moduleName + "/init.st",
        "../interpreter/C++/build/std/" + moduleName + "/init.st",
        "../interpreter/C++/build/std/encoding/" + moduleName + "/init.st",

        // Two levels up
        "../../std/" + moduleName + "/init.st",
        "../../std/encoding/" + moduleName + "/init.st",
        "../../interpreter/C++/build/std/" + moduleName + "/init.st",
        "../../interpreter/C++/build/std/encoding/" + moduleName + "/init.st",

        // Three levels up
        "../../../std/" + moduleName + "/init.st",
        "../../../std/encoding/" + moduleName + "/init.st"
    };

    std::string moduleFilePath;
    for (const auto& path : searchPaths) {
        if (fs::exists(path)) {
            moduleFilePath = path;
            break;
        }
    }

    if (moduleFilePath.empty()) {
        error("Could not find module '" + moduleName + "'. Searched in std directories.");
        return;
    }

    // Module file exists - just register it
    // We don't parse the module file because:
    // 1. Native functions are implemented in C++ (NativeRegistry)
    // 2. Module files may use advanced syntax not yet fully supported
    // 3. We only need to know the module exists and is available
    // 4. Actual function validation happens during CallExpr analysis via NativeRegistry

    // Register the module in the symbol table
    symbolTable.define(Symbol::Variable(moduleName, "module", false));
}

std::string SemanticAnalyzer::inferType(Expr* expr) {
    // Infer the type of an expression
    if (auto* literal = dynamic_cast<LiteralExpr*>(expr)) {
        if (literal->type == TokenType::NUMBER) {
            // Check if it's a double (has decimal point) or int
            if (literal->value.find('.') != std::string::npos) {
                return "double";
            } else {
                return "int";
            }
        } else if (literal->type == TokenType::STRING) {
            return "string";
        } else if (literal->type == TokenType::TRUE || literal->type == TokenType::FALSE) {
            return "bool";
        }
    } else if (auto* varExpr = dynamic_cast<VariableExpr*>(expr)) {
        // Look up variable type from symbol table
        if (symbolTable.isDefined(varExpr->name.lexeme)) {
            auto symbol = symbolTable.lookup(varExpr->name.lexeme);
            return symbol.type;
        }
    } else if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
        // Infer type from binary operation
        std::string leftType = inferType(binary->left.get());
        std::string rightType = inferType(binary->right.get());

        // If either side is double, result is double
        if (leftType == "double" || rightType == "double") {
            return "double";
        }
        // If both are int, result is int
        if (leftType == "int" && rightType == "int") {
            return "int";
        }
        // String concatenation
        if (leftType == "string" || rightType == "string") {
            return "string";
        }
    } else if (auto* callExpr = dynamic_cast<CallExpr*>(expr)) {
        // Try to infer return type from function signature
        if (auto* binExpr = dynamic_cast<BinaryExpr*>(callExpr->callee.get())) {
            if (binExpr->op.type == TokenType::DOT) {
                if (auto* leftVar = dynamic_cast<VariableExpr*>(binExpr->left.get())) {
                    if (auto* rightVar = dynamic_cast<VariableExpr*>(binExpr->right.get())) {
                        std::string moduleName = leftVar->name.lexeme;
                        std::string functionName = rightVar->name.lexeme;

                        auto& registry = NativeRegistry::getInstance();
                        if (registry.hasSignature(moduleName, functionName)) {
                            auto signature = registry.getSignature(moduleName, functionName);
                            return signature.returnType;
                        }
                    }
                }
            }
        }
    }

    return "unknown";
}

} // namespace stratos
