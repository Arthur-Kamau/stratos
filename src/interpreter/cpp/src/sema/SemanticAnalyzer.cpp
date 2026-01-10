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

SemanticAnalyzer::SemanticAnalyzer(std::string root) : projectRoot(root) {
    defineNativeFunctions();
}

void SemanticAnalyzer::defineNativeFunctions() {
    // Don't pre-define 'print' or 'println' - they're defined in std/io/console.st
    // symbolTable.define(Symbol::Function("print", {"any"}, "void"));
    // symbolTable.define(Symbol::Function("println", {"any"}, "void"));

    // Define standard types/constructors
    symbolTable.define(Symbol::Variable("Some", "constructor", false)); // Mock for now
    symbolTable.define(Symbol::Variable("None", "Optional", false));
    symbolTable.define(Symbol::Function("Array", {}, "any")); // Generic array constructor

    // Internal intrinsics
    symbolTable.define(Symbol::Function("__if_expr", {"bool", "any", "any"}, "any"));
}

bool SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<Stmt>>& statements) {
    hadError = false;

    // Zero-th pass: Register all packages present in the compilation unit as loaded
    for (const auto& stmt : statements) {
        if (auto* pkgDecl = dynamic_cast<PackageDecl*>(stmt.get())) {
            // Check if already in loadedModules to avoid duplicates (e.g. multiple files in same package)
            if (std::find(loadedModules.begin(), loadedModules.end(), pkgDecl->name.lexeme) == loadedModules.end()) {
                loadedModules.push_back(pkgDecl->name.lexeme);
            }
        }
    }

    // First pass: Collect all function, class, and enum declarations (including inside packages)
    for (size_t i = 0; i < statements.size(); ++i) {
        if (!statements[i]) continue;

        // Check for PackageDecl and process its contents
        if (auto* pkgDecl = dynamic_cast<PackageDecl*>(statements[i].get())) {
            for (const auto& decl : pkgDecl->declarations) {
                if (auto* funcDecl = dynamic_cast<FunctionDecl*>(decl.get())) {
                    std::vector<std::string> paramTypes;
                    for (const auto& param : funcDecl->parameters) {
                        paramTypes.push_back(param.type);
                    }
                    Symbol funcSymbol = Symbol::Function(funcDecl->name.lexeme, paramTypes, funcDecl->returnType);
                    if (!symbolTable.define(funcSymbol)) {
                        error(funcDecl->name, "Function '" + funcDecl->name.lexeme + "' is already defined.");
                    }
                } else if (auto* classDecl = dynamic_cast<ClassDecl*>(decl.get())) {
                    if (!symbolTable.define(Symbol{classDecl->name.lexeme, SymbolKind::CLASS, classDecl->name.lexeme, false})) {
                        error(classDecl->name, "Class '" + classDecl->name.lexeme + "' is already defined.");
                    }
                } else if (auto* enumDecl = dynamic_cast<EnumDecl*>(decl.get())) {
                    if (!symbolTable.define(Symbol{enumDecl->name.lexeme, SymbolKind::CLASS, enumDecl->name.lexeme, false})) {
                        error(enumDecl->name, "Enum '" + enumDecl->name.lexeme + "' is already defined.");
                    }
                    for (const auto& value : enumDecl->values) {
                        std::string fullName = enumDecl->name.lexeme + "." + value.lexeme;
                        symbolTable.define(Symbol::Variable(fullName, enumDecl->name.lexeme, false));
                    }
                }
            }
            continue;
        }

        // Only process declarations, not their bodies (for top-level non-packaged code)
        if (auto* funcDecl = dynamic_cast<FunctionDecl*>(statements[i].get())) {
            std::vector<std::string> paramTypes;
            for (const auto& param : funcDecl->parameters) {
                paramTypes.push_back(param.type);
            }
            Symbol funcSymbol = Symbol::Function(funcDecl->name.lexeme, paramTypes, funcDecl->returnType);
            if (!symbolTable.define(funcSymbol)) {
                error(funcDecl->name, "Function '" + funcDecl->name.lexeme + "' is already defined.");
            }
        } else if (auto* classDecl = dynamic_cast<ClassDecl*>(statements[i].get())) {
            if (!symbolTable.define(Symbol{classDecl->name.lexeme, SymbolKind::CLASS, classDecl->name.lexeme, false})) {
                error(classDecl->name, "Class '" + classDecl->name.lexeme + "' is already defined.");
            }
        } else if (auto* enumDecl = dynamic_cast<EnumDecl*>(statements[i].get())) {
            if (!symbolTable.define(Symbol{enumDecl->name.lexeme, SymbolKind::CLASS, enumDecl->name.lexeme, false})) {
                error(enumDecl->name, "Enum '" + enumDecl->name.lexeme + "' is already defined.");
            }
            // Also register enum values in first pass
            for (const auto& value : enumDecl->values) {
                std::string fullName = enumDecl->name.lexeme + "." + value.lexeme;
                symbolTable.define(Symbol::Variable(fullName, enumDecl->name.lexeme, false));
            }
        }
    }

    // Second pass: Analyze all statements including bodies
    for (size_t i = 0; i < statements.size(); ++i) {
        if (!statements[i]) {
            continue;
        }
        statements[i]->accept(*this);
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
    expr.left->accept(*this);
    std::string leftType = inferType(expr.left.get());

    if (expr.op.type == TokenType::DOT) {
        // Handle member access (e.g., obj.field, Enum.VALUE, array.length())
        
        // String methods
        if (leftType == "string") {
            if (auto* rightVar = dynamic_cast<VariableExpr*>(expr.right.get())) {
                std::string memberName = rightVar->name.lexeme;
                // Whitelist standard string methods
                if (memberName == "split" || memberName == "repeat" || memberName == "length" || 
                    memberName == "trim" || memberName == "toUpper" || memberName == "toLower" ||
                    memberName == "startsWith" || memberName == "endsWith" || memberName == "replace") {
                    return; // Resolved as valid string method
                }
            }
        }

        // Check if this is an array method call
        if (leftType.find("Array<") == 0 || leftType == "Array" || leftType == "array") { // Simple check for Array type
            if (auto* rightVar = dynamic_cast<VariableExpr*>(expr.right.get())) {
                std::string memberName = rightVar->name.lexeme;
                if (memberName == "length") { // Array has a .length() method
                    // Define a dummy symbol for length for type checking purposes
                    symbolTable.define(Symbol::Function("Array::length", {}, "int")); // Transient definition
                    return; // Resolved
                }
                 if (memberName == "add") { // Array has an .add() method
                    // Define a dummy symbol for add for type checking purposes
                    symbolTable.define(Symbol::Function("Array::add", {"any"}, "void")); // Transient definition
                    return; // Resolved
                }
                
                // Other array methods
                if (memberName == "isEmpty" || memberName == "first" || memberName == "last" || 
                    memberName == "join" || memberName == "contains" || memberName == "indexOf" || 
                    memberName == "reverse" || memberName == "clear") {
                    return; // Resolved
                }
            }
        }
        
        // Check if this is an enum value access (e.g., Color.RED)
        if (auto* leftVar = dynamic_cast<VariableExpr*>(expr.left.get())) {
            if (auto* rightVar = dynamic_cast<VariableExpr*>(expr.right.get())) {
                std::string enumName = leftVar->name.lexeme;
                std::string valueName = rightVar->name.lexeme;
                std::string fullName = enumName + "." + valueName;

                // Check if this is a registered enum value
                if (symbolTable.resolve(fullName)) {
                    // Valid enum value access
                    return;
                }

                // If full name resolution failed, check if the base (left side) is a valid enum/class
                // Only error if the base is actually a class/enum (static access attempt)
                // If it's a variable (instance), we fall through to regular member access handling
                auto resolvedBase = symbolTable.resolve(enumName);
                if (resolvedBase && resolvedBase->kind == SymbolKind::CLASS) {
                    // The enum/class exists, but the member does not
                    error(rightVar->name, "Member '" + valueName + "' not found in enum '" + enumName + "'.");
                    return;
                }
            }
        }
        
        // Check for class field access (e.g. file.name)
        // If the left side type is a known class, we assume the field exists for now
        // (Full field validation would require storing class structure in SymbolTable)
        // std::cerr << "[DEBUG] BinaryExpr DOT: leftType=" << leftType << std::endl;
        auto classSymbol = symbolTable.resolve(leftType);
        if (classSymbol) {
             // std::cerr << "[DEBUG] Resolved leftType to symbol kind=" << static_cast<int>(classSymbol->kind) << std::endl;
        } else {
             // std::cerr << "[DEBUG] Failed to resolve leftType=" << leftType << std::endl;
        }
        
        if (classSymbol && classSymbol->kind == SymbolKind::CLASS) {
            return; // Valid member access on object
        }

        // For other dot accesses (like module.function), just validate right side
        // The left side (module name) was already analyzed above
        expr.right->accept(*this);
        return;
    }

    // Default processing for non-dot binary expressions
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

                                // Allow any type if expectedType is "any", or if argType is unknown
                                if (argType != expectedType && argType != "unknown" && expectedType != "any") {
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

    // Check if this is a prelude function call (auto-imported)
    if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.callee.get())) {
        std::string functionName = varExpr->name.lexeme;
        auto& registry = NativeRegistry::getInstance();

        // Check if this function exists in the prelude module
        if (registry.isNative("prelude", functionName)) {
            // Validate arguments exist
            for (const auto& arg : expr.arguments) {
                arg->accept(*this);
            }
            return;  // Valid prelude function call
        }
    }

    // Not a module call or prelude call, process as regular function call
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

void SemanticAnalyzer::visit(CastExpr& expr) {
    expr.expression->accept(*this);
    // Future: Verify if cast is valid (e.g. primitive to primitive)
}

void SemanticAnalyzer::visit(MapLiteralExpr& expr) {
    for (const auto& pair : expr.entries) {
        pair.second->accept(*this);
    }
}

void SemanticAnalyzer::visit(LambdaExpr& expr) {
    symbolTable.enterScope();
    for (const auto& param : expr.params) {
        symbolTable.define(Symbol::Variable(param.lexeme, "any", false));
    }
    if (expr.body) expr.body->accept(*this);
    symbolTable.exitScope();
}

// --- Statements ---

void SemanticAnalyzer::visit(VarDecl& stmt) {
    // 1. Analyze initializer first (so it can't refer to the variable being declared)
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    }

    // 2. Define variable in current scope
    std::string type = stmt.typeName;
    if (type.empty()) {
        if (stmt.initializer) {
            type = inferType(stmt.initializer.get());
        } else {
            type = "any";
        }
    }
    Symbol symbol = Symbol::Variable(stmt.name.lexeme, type, stmt.isMutable);

    if (!symbolTable.define(symbol)) {
        error(stmt.name, "Variable '" + stmt.name.lexeme + "' is already defined in this scope.");
    }
}

void SemanticAnalyzer::visit(FunctionDecl& stmt) {
    // Try to define function name in current scope (may already be defined from first pass for top-level functions)
    std::vector<std::string> paramTypes;
    for (const auto& param : stmt.parameters) {
        paramTypes.push_back(param.type);
    }
    
    Symbol funcSymbol = Symbol::Function(stmt.name.lexeme, paramTypes, stmt.returnType);
    if (!symbolTable.define(funcSymbol)) {
        // Only error if we're in a class (class methods should be defined fresh)
        // Top-level functions are expected to fail here due to first pass registration
        if (!currentClassName.empty()) {
            error(stmt.name, "Function '" + stmt.name.lexeme + "' is already defined.");
        }
    }

    // Only analyze body if it exists (skip function declarations without bodies)
    if (!stmt.body) {
        return;
    }

    // Enter new scope for function body
    symbolTable.enterScope();

    // If we're in a class, define 'this' in the method scope
    if (!currentClassName.empty()) {
        symbolTable.define(Symbol::Variable("this", currentClassName, false));
    }

    // Define parameters as variables
    for (const auto& param : stmt.parameters) {
        Symbol paramSym = Symbol::Variable(param.name.lexeme, param.type, false);
        symbolTable.define(paramSym);
        
        // Analyze default value if present
        if (param.defaultValue) {
            param.defaultValue->accept(*this);
        }
    }

    // Analyze body
    for (const auto& s : *stmt.body) {
        if (s) s->accept(*this);
    }

    // Exit scope
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(ClassDecl& stmt) {
    // Try to define class (may already be defined from first pass)
    symbolTable.define(Symbol{stmt.name.lexeme, SymbolKind::CLASS, stmt.name.lexeme, false});

    // Set current class context
    std::string previousClassName = currentClassName;
    currentClassName = stmt.name.lexeme;

    symbolTable.enterScope();
    // Define 'this'
    symbolTable.define(Symbol::Variable("this", stmt.name.lexeme, false));

    for (const auto& member : stmt.methods) {
        if (member) {
            member->accept(*this);
        }
    }

    symbolTable.exitScope();

    // Restore previous class context
    currentClassName = previousClassName;
}

void SemanticAnalyzer::visit(EnumDecl& stmt) {
    // Register the enum type itself (may already be defined from first pass)
    symbolTable.define(Symbol{stmt.name.lexeme, SymbolKind::CLASS, stmt.name.lexeme, false});

    // Register each enum value as a constant (may already be defined from first pass)
    // Enum values are accessed as EnumName.VALUE, so we store them with dotted names
    for (const auto& value : stmt.values) {
        std::string fullName = stmt.name.lexeme + "." + value.lexeme;
        // Register as a variable (constant) of the enum type
        symbolTable.define(Symbol::Variable(fullName, stmt.name.lexeme, false));
    }
}

void SemanticAnalyzer::visit(PackageDecl& stmt) {
    // std::cout << "  [PackageDecl] Package: " << stmt.name.lexeme << " with " << stmt.declarations.size() << " declarations" << std::endl;
    // Package declaration (like "package main;") just declares what package
    // this file belongs to - it doesn't create a symbol in the namespace.
    // Don't define the package name as a symbol to avoid conflicts.
    // NOTE: Package imports (via 'use' keyword) would be different.

    // Process declarations within the package without creating a new scope
    // or defining the package name as a symbol
    for (size_t i = 0; i < stmt.declarations.size(); ++i) {
        // std::cout << "  [PackageDecl] Processing declaration " << (i+1) << "/" << stmt.declarations.size() << std::endl;
        if (stmt.declarations[i]) stmt.declarations[i]->accept(*this);
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
    for (size_t i = 0; i < stmt.statements.size(); ++i) {
        if (stmt.statements[i]) {
            stmt.statements[i]->accept(*this);
        }
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
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

void SemanticAnalyzer::visit(ForStmt& stmt) {
    // Analyze the iterable expression first (in outer scope)
    stmt.iterable->accept(*this);

    // Enter new scope for loop variable and body
    symbolTable.enterScope();

    // Define the loop variable in this scope
    // For now, use the provided type or infer from iterable if possible
    std::string loopVarType = stmt.varType;
    if (loopVarType.empty()) {
        std::string iterableType = inferType(stmt.iterable.get());
        if (iterableType.find("Array<") == 0 && iterableType.back() == '>') {
             loopVarType = iterableType.substr(6, iterableType.length() - 7);
        } else if (iterableType == "string") {
             loopVarType = "char";
        } else {
             loopVarType = "any";
        }
    }
    
    Symbol loopVar = Symbol::Variable(stmt.variable.lexeme, loopVarType, stmt.isMutable);
    if (!symbolTable.define(loopVar)) {
        error(stmt.variable, "Loop variable '" + stmt.variable.lexeme + "' is already defined in this scope.");
    }

    // Analyze loop body
    stmt.body->accept(*this);

    // Exit loop scope
    symbolTable.exitScope();
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
        // Internal project packages (highest priority)
        projectRoot + "/src/" + moduleName + "/init.st",
        projectRoot + "/src/" + moduleName + "/" + moduleName + ".st",

        // Dependencies directory
        projectRoot + "/deps/" + moduleName + "/src/init.st",
        projectRoot + "/deps/" + moduleName + "/src/" + moduleName + ".st",
        projectRoot + "/../deps/" + moduleName + "/src/init.st",
        projectRoot + "/../deps/" + moduleName + "/src/" + moduleName + ".st",

        // Current directory (fallback)
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
    bool isUserModule = false;

    for (const auto& path : searchPaths) {
        if (fs::exists(path)) {
            moduleFilePath = path;
            // Check if this is a user-defined module (in src/ directory)
            isUserModule = path.starts_with("src/");
            break;
        }
    }

    // If no specific module file was found, check if module directory exists
    if (moduleFilePath.empty()) {
        std::string moduleDir = projectRoot + "/src/" + moduleName;
        if (fs::exists(moduleDir) && fs::is_directory(moduleDir)) {
            moduleFilePath = moduleDir;
            isUserModule = true;
        }
    }

    if (moduleFilePath.empty()) {
        error("Could not find module '" + moduleName + "'. Searched in std directories.");
        return;
    }

    // UPDATED: Now we parse ALL modules (std and user-defined) to support pure Stratos implementations
    // /* OLD BEHAVIOR - commented out to preserve history
    // // For built-in/native modules, just register the module name
    // if (!isUserModule) {
    //     // Module file exists - just register it
    //     // We don't parse the module file because:
    //     // 1. Native functions are implemented in C++ (NativeRegistry)
    //     // 2. Module files may use advanced syntax not yet fully supported
    //     // 3. We only need to know the module exists and is available
    //     // 4. Actual function validation happens during CallExpr analysis via NativeRegistry
    //
    //     // Register the module in the symbol table
    //     symbolTable.define(Symbol::Variable(moduleName, "module", false));
    //     return;
    // }
    // */

    // Register the module in the symbol table
    symbolTable.define(Symbol::Variable(moduleName, "module", false));

    // Parse all files in the module directory (both std and user-defined)
    // This enables pure Stratos implementations alongside native C++ functions
    std::vector<std::string> moduleFiles;

    // If moduleFilePath is a directory, scan for all .st files
    // Otherwise, it's a file and we also need to scan its directory
    fs::path modulePath(moduleFilePath);

    if (fs::is_directory(modulePath)) {
        // moduleFilePath is already the directory, scan it
        for (const auto& entry : fs::directory_iterator(modulePath)) {
            if (entry.path().extension() == ".st") {
                moduleFiles.push_back(entry.path().string());
            }
        }
    } else if (fs::is_regular_file(modulePath)) {
        // moduleFilePath is a specific file, scan its parent directory
        fs::path moduleDir = modulePath.parent_path();
        for (const auto& entry : fs::directory_iterator(moduleDir)) {
            if (entry.path().extension() == ".st") {
                moduleFiles.push_back(entry.path().string());
            }
        }
    } else {
        // Fallback: just use the file itself if it exists
        moduleFiles.push_back(moduleFilePath);
    }

    // Sort files to ensure consistent processing order (e.g., console.st before init.st)
    std::sort(moduleFiles.begin(), moduleFiles.end());

    // Parse and process each file in the module
    for (const auto& filePath : moduleFiles) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            continue; // Skip files we can't open
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();

        try {
            // Lex and parse the module file
            Lexer lexer(source);
            std::vector<Token> tokens = lexer.scanTokens();

            Parser parser(tokens);
            std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

            // Two-pass processing like analyze() does:
            // First pass: Collect all function, class, and enum declarations
            for (size_t i = 0; i < statements.size(); ++i) {
                if (!statements[i]) continue;

                // Only process declarations, not their bodies
                if (auto* funcDecl = dynamic_cast<FunctionDecl*>(statements[i].get())) {
                    std::vector<std::string> paramTypes;
                    for (const auto& param : funcDecl->parameters) {
                        paramTypes.push_back(param.type);
                    }
                    Symbol funcSymbol = Symbol::Function(funcDecl->name.lexeme, paramTypes, funcDecl->returnType);
                    if (!symbolTable.define(funcSymbol)) {
                        error(funcDecl->name, "Function '" + funcDecl->name.lexeme + "' is already defined.");
                    }
                } else if (auto* classDecl = dynamic_cast<ClassDecl*>(statements[i].get())) {
                    if (!symbolTable.define(Symbol{classDecl->name.lexeme, SymbolKind::CLASS, classDecl->name.lexeme, false})) {
                        error(classDecl->name, "Class '" + classDecl->name.lexeme + "' is already defined.");
                    }
                } else if (auto* enumDecl = dynamic_cast<EnumDecl*>(statements[i].get())) {
                    if (!symbolTable.define(Symbol{enumDecl->name.lexeme, SymbolKind::CLASS, enumDecl->name.lexeme, false})) {
                        error(enumDecl->name, "Enum '" + enumDecl->name.lexeme + "' is already defined.");
                    }
                    // Also register enum values in first pass
                    for (const auto& value : enumDecl->values) {
                        std::string fullName = enumDecl->name.lexeme + "." + value.lexeme;
                        symbolTable.define(Symbol::Variable(fullName, enumDecl->name.lexeme, false));
                    }
                }
            }

            // Second pass: Process all statements including bodies
            for (const auto& stmt : statements) {
                if (stmt) {
                    stmt->accept(*this);
                }
            }
        } catch (const std::exception& e) {
            error("Error loading module '" + moduleName + "' from " + filePath + ": " + e.what());
        }
    }
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
        auto symbolOpt = symbolTable.resolve(varExpr->name.lexeme);
        if (symbolOpt) {
            return symbolOpt->type;
        }
    } else if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
        std::string leftType = inferType(binary->left.get());
        
        if (binary->op.type == TokenType::DOT) {
            if (leftType.find("Array<") == 0 || leftType == "Array") {
                if (auto* rightVar = dynamic_cast<VariableExpr*>(binary->right.get())) {
                    std::string memberName = rightVar->name.lexeme;
                    if (memberName == "length") return "int";
                    if (memberName == "add") return "void"; // Add typically returns void or the element
                }
            }
        }

        // Infer type from binary operation
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
        // Check for Array constructor
        if (auto* varExpr = dynamic_cast<VariableExpr*>(callExpr->callee.get())) {
            if (varExpr->name.lexeme == "Array") {
                 return "Array";
            }
            
            // Handle simple function call (local or prelude)
            auto symbolOpt = symbolTable.resolve(varExpr->name.lexeme);
            if (symbolOpt) {
                if (symbolOpt->kind == SymbolKind::FUNCTION) {
                    return symbolOpt->returnType;
                }
                return symbolOpt->type;
            }
        }

        // Try to infer return type from function signature
        if (auto* binExpr = dynamic_cast<BinaryExpr*>(callExpr->callee.get())) {
            if (binExpr->op.type == TokenType::DOT) {
                // Check if it's a method call on an object
                std::string leftType = inferType(binExpr->left.get());
                if (auto* rightVar = dynamic_cast<VariableExpr*>(binExpr->right.get())) {
                     std::string methodName = rightVar->name.lexeme;
                     
                     if (leftType == "string") {
                         if (methodName == "split") return "Array<string>";
                         if (methodName == "length") return "int";
                         if (methodName == "repeat") return "string";
                         if (methodName == "trim" || methodName == "trimLeft" || methodName == "trimRight" || 
                             methodName == "trimPrefix" || methodName == "trimSuffix") return "string";
                         if (methodName == "toUpper" || methodName == "toLower" || methodName == "toTitle") return "string";
                         if (methodName == "startsWith" || methodName == "endsWith") return "bool";
                     }
                     if (leftType.find("Array") == 0 || leftType == "array") {
                         if (methodName == "length") return "int";
                         if (methodName == "join") return "string";
                         if (methodName == "isEmpty") return "bool";
                         if (methodName == "contains") return "bool";
                         if (methodName == "indexOf") return "int";
                         // For first/last/get, we should return the element type, but we don't track generics well yet.
                         // For Array<string>, element is string.
                         if (methodName == "first" || methodName == "last" || methodName == "get") {
                             if (leftType.find("Array<") == 0 && leftType.back() == '>') {
                                 return leftType.substr(6, leftType.length() - 7);
                             }
                             return "any";
                         }
                         if (methodName == "reverse" || methodName == "clear") return leftType;
                     }
                }

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
    } else if (auto* structExpr = dynamic_cast<StructInitExpr*>(expr)) {
        return structExpr->name.lexeme;
    }

    return "unknown";
}

void SemanticAnalyzer::visit(StructInitExpr& expr) {
    // 1. Verify struct existence
    // Since we don't have full type checking on structs yet, we just check if it's available in symbol table if possible
    // But struct definitions are treated as ClassDecl which might be in classes map in Interpreter, 
    // Here in SemanticAnalyzer we track symbols.
    
    // 2. Analyze fields
    for (const auto& field : expr.fields) {
        field.second->accept(*this);
        // lastExprType is set by visit
    }
    
    lastExprType = expr.name.lexeme; // Tentative type name
}

} // namespace stratos
