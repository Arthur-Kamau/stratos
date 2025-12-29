#include "stratos/Interpreter.h"
#include "stratos/Lexer.h"
#include "stratos/Parser.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace stratos {

Interpreter::Interpreter() {
    // Reserve capacity to prevent vector reallocation (which would invalidate currentEnv pointer)
    // This ensures currentEnv remains valid throughout execution
    environments.reserve(1000);

    // Create global environment
    auto globalEnv = std::make_unique<Environment>();
    currentEnv = globalEnv.get();
    environments.push_back(std::move(globalEnv));
}

void Interpreter::execute(std::vector<std::unique_ptr<Stmt>>&& statements) {
    // Move statements into interpreter for ownership
    for (auto& stmt : statements) {
        mainStatements.push_back(std::move(stmt));
    }

    // Execute the stored statements
    for (const auto& stmt : mainStatements) {
        if (stmt) {
            stmt->accept(*this);
            // Periodically collect garbage to handle cycles
            maybeCollectGarbage();
        }
    }
}

void Interpreter::cleanup() {
    // Clear local environments to allow cycle detection
    // Keep only global environment (first one)
    while (environments.size() > 1) {
        environments.pop_back();
    }
    currentEnv = environments[0].get();

    // Run GC BEFORE clearing global environment
    // This allows GC to detect cycles while objects still exist
    std::vector<RuntimeValue> roots;
    collectRoots(roots);

    size_t collected = gc.collect(roots);

    // Optional: Report cycles broken
    if (collected > 0) {
        std::cerr << "[GC] Broke " << collected << " circular reference(s)\n";
    }

    // Clear global environment variables after GC
    // This allows refcount cleanup to work on non-cyclic objects
    currentEnv->variables.clear();
}

void Interpreter::enterScope() {
    auto newEnv = std::make_unique<Environment>();
    newEnv->parent = currentEnv;
    currentEnv = newEnv.get();
    environments.push_back(std::move(newEnv));
}

void Interpreter::exitScope() {
    if (currentEnv->parent) {
        currentEnv = currentEnv->parent;
    }
}

void Interpreter::error(const std::string& message) {
    throw std::runtime_error("Runtime error: " + message);
}

// --- Expression Visitors ---

void Interpreter::visit(BinaryExpr& expr) {
    // Special handling for assignment (=, +=, -=)
    if (expr.op.type == TokenType::EQUAL || expr.op.type == TokenType::PLUS_EQUAL ||
        expr.op.type == TokenType::MINUS_EQUAL) {

        // Evaluate the right side (the value being assigned)
        expr.right->accept(*this);
        RuntimeValue value = lastValue;

        // Check if left side is member access (this.field)
        if (auto* dotExpr = dynamic_cast<BinaryExpr*>(expr.left.get())) {
            if (dotExpr->op.type == TokenType::DOT) {
                dotExpr->left->accept(*this);
                RuntimeValue target = lastValue;

                if (target.type == "object") {
                    auto instance = target.asObject();
                    if (auto* rightVar = dynamic_cast<VariableExpr*>(dotExpr->right.get())) {
                        std::string fieldName = rightVar->name.lexeme;

                        if (expr.op.type == TokenType::EQUAL) {
                            instance->fields[fieldName] = value;
                        } else if (expr.op.type == TokenType::PLUS_EQUAL) {
                            // Defensive check: ensure field exists before accessing
                            if (!instance->fields.count(fieldName)) {
                                error("Undefined field: " + instance->className + "::" + fieldName);
                                return;
                            }
                            auto& field = instance->fields[fieldName];
                            if (field.type == "int" && value.type == "int") {
                                field = RuntimeValue(std::any(field.asInt() + value.asInt()), "int");
                            } else if (field.type == "double" && value.type == "double") {
                                field = RuntimeValue(std::any(field.asDouble() + value.asDouble()), "double");
                            }
                        } else if (expr.op.type == TokenType::MINUS_EQUAL) {
                            // Defensive check: ensure field exists before accessing
                            if (!instance->fields.count(fieldName)) {
                                error("Undefined field: " + instance->className + "::" + fieldName);
                                return;
                            }
                            auto& field = instance->fields[fieldName];
                            if (field.type == "int" && value.type == "int") {
                                field = RuntimeValue(std::any(field.asInt() - value.asInt()), "int");
                            } else if (field.type == "double" && value.type == "double") {
                                field = RuntimeValue(std::any(field.asDouble() - value.asDouble()), "double");
                            }
                        }

                        lastValue = value;
                        return;
                    }
                }
            }
        }

        // Simple variable assignment
        if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.left.get())) {
            std::string varName = varExpr->name.lexeme;

            if (expr.op.type == TokenType::EQUAL) {
                currentEnv->assign(varName, value);
            } else if (expr.op.type == TokenType::PLUS_EQUAL) {
                RuntimeValue current = currentEnv->get(varName);
                if (current.type == "int" && value.type == "int") {
                    value = RuntimeValue(std::any(current.asInt() + value.asInt()), "int");
                } else if (current.type == "double" && value.type == "double") {
                    value = RuntimeValue(std::any(current.asDouble() + value.asDouble()), "double");
                }
                currentEnv->assign(varName, value);
            } else if (expr.op.type == TokenType::MINUS_EQUAL) {
                RuntimeValue current = currentEnv->get(varName);
                if (current.type == "int" && value.type == "int") {
                    value = RuntimeValue(std::any(current.asInt() - value.asInt()), "int");
                } else if (current.type == "double" && value.type == "double") {
                    value = RuntimeValue(std::any(current.asDouble() - value.asDouble()), "double");
                }
                currentEnv->assign(varName, value);
            }

            lastValue = value;
            return;
        }
    }

    // Special handling for DOT operator - don't evaluate right side as a variable
    if (expr.op.type == TokenType::DOT) {
        expr.left->accept(*this);
        RuntimeValue left = lastValue;

        // Member access for objects
        if (left.type == "object") {
            auto instance = left.asObject();
            if (auto* rightVar = dynamic_cast<VariableExpr*>(expr.right.get())) {
                std::string memberName = rightVar->name.lexeme;

                // Check if it's a field
                if (instance->fields.count(memberName)) {
                    lastValue = instance->fields[memberName];
                    return;
                }
            }
        }

        // Member access for module.constant or module.property
        if (left.type == "module") {
            if (auto* leftVar = dynamic_cast<VariableExpr*>(expr.left.get())) {
                if (auto* rightVar = dynamic_cast<VariableExpr*>(expr.right.get())) {
                    std::string moduleName = leftVar->name.lexeme;
                    std::string memberName = rightVar->name.lexeme;

                    // Try to call as a zero-parameter native function (for constants like PI, E)
                    std::vector<RuntimeValue> emptyArgs;
                    try {
                        lastValue = evaluateNativeCall(moduleName, memberName, emptyArgs);
                        return;
                    } catch (...) {
                        error("Undefined property: " + moduleName + "." + memberName);
                    }
                }
            }
        }
        // DOT operator handled, return
        return;
    }

    // Regular binary operators - evaluate both sides
    expr.left->accept(*this);
    RuntimeValue left = lastValue;

    expr.right->accept(*this);
    RuntimeValue right = lastValue;

    switch (expr.op.type) {
        case TokenType::PLUS:
            if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(std::any(left.asDouble() + right.asDouble()), "double");
            } else if (left.type == "int" && right.type == "int") {
                lastValue = RuntimeValue(std::any(left.asInt() + right.asInt()), "int");
            } else if (left.type == "string" || right.type == "string") {
                // String concatenation
                std::string leftStr;
                if (left.type == "string") leftStr = left.asString();
                else if (left.type == "int") leftStr = std::to_string(left.asInt());
                else if (left.type == "double") leftStr = std::to_string(left.asDouble());
                else if (left.type == "bool") leftStr = left.asBool() ? "true" : "false";

                std::string rightStr;
                if (right.type == "string") rightStr = right.asString();
                else if (right.type == "int") rightStr = std::to_string(right.asInt());
                else if (right.type == "double") rightStr = std::to_string(right.asDouble());
                else if (right.type == "bool") rightStr = right.asBool() ? "true" : "false";

                lastValue = RuntimeValue(std::any(leftStr + rightStr), "string");
            }
            break;

        case TokenType::MINUS:
            if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(std::any(left.asDouble() - right.asDouble()), "double");
            } else {
                lastValue = RuntimeValue(std::any(left.asInt() - right.asInt()), "int");
            }
            break;

        case TokenType::STAR:
            if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(std::any(left.asDouble() * right.asDouble()), "double");
            } else {
                lastValue = RuntimeValue(std::any(left.asInt() * right.asInt()), "int");
            }
            break;

        case TokenType::SLASH:
            if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(std::any(left.asDouble() / right.asDouble()), "double");
            } else {
                lastValue = RuntimeValue(std::any(left.asInt() / right.asInt()), "int");
            }
            break;

        case TokenType::PERCENT:
            // Modulo operator - only works with integers
            if (left.type == "int" && right.type == "int") {
                lastValue = RuntimeValue(std::any(left.asInt() % right.asInt()), "int");
            } else {
                throw std::runtime_error("Modulo operator requires integer operands");
            }
            break;

        case TokenType::EQUAL_EQUAL:
            if (left.type == "int" && right.type == "int") {
                lastValue = RuntimeValue(std::any(left.asInt() == right.asInt()), "bool");
            } else if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(std::any(left.asDouble() == right.asDouble()), "bool");
            } else if (left.type == "string" && right.type == "string") {
                lastValue = RuntimeValue(std::any(left.asString() == right.asString()), "bool");
            }
            break;

        case TokenType::BANG_EQUAL:
            if (left.type == "int" && right.type == "int") {
                lastValue = RuntimeValue(std::any(left.asInt() != right.asInt()), "bool");
            } else if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(std::any(left.asDouble() != right.asDouble()), "bool");
            }
            break;

        case TokenType::LESS:
            if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(std::any(left.asDouble() < right.asDouble()), "bool");
            } else {
                lastValue = RuntimeValue(std::any(left.asInt() < right.asInt()), "bool");
            }
            break;

        case TokenType::LESS_EQUAL:
            if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(std::any(left.asDouble() <= right.asDouble()), "bool");
            } else {
                lastValue = RuntimeValue(std::any(left.asInt() <= right.asInt()), "bool");
            }
            break;

        case TokenType::GREATER:
            if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(std::any(left.asDouble() > right.asDouble()), "bool");
            } else {
                lastValue = RuntimeValue(std::any(left.asInt() > right.asInt()), "bool");
            }
            break;

        case TokenType::GREATER_EQUAL:
            if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(std::any(left.asDouble() >= right.asDouble()), "bool");
            } else {
                lastValue = RuntimeValue(std::any(left.asInt() >= right.asInt()), "bool");
            }
            break;

        default:
            error("Unsupported binary operator");
    }
}

void Interpreter::visit(UnaryExpr& expr) {
    expr.right->accept(*this);
    RuntimeValue operand = lastValue;

    switch (expr.op.type) {
        case TokenType::MINUS:
            if (operand.type == "double") {
                lastValue = RuntimeValue(std::any(-operand.asDouble()), "double");
            } else {
                lastValue = RuntimeValue(std::any(-operand.asInt()), "int");
            }
            break;

        case TokenType::NOT:
        case TokenType::BANG:
            lastValue = RuntimeValue(std::any(!isTruthy(operand)), "bool");
            break;

        default:
            error("Unsupported unary operator");
    }
}

void Interpreter::visit(LiteralExpr& expr) {
    switch (expr.type) {
        case TokenType::NUMBER:
            if (expr.value.find('.') != std::string::npos) {
                lastValue = RuntimeValue(std::any(std::stod(expr.value)), "double");
            } else {
                lastValue = RuntimeValue(std::any(std::stoi(expr.value)), "int");
            }
            break;

        case TokenType::STRING:
            lastValue = RuntimeValue(std::any(expr.value), "string");
            break;

        case TokenType::CHAR:
            // Store char as a single character (first character of the string)
            if (!expr.value.empty()) {
                lastValue = RuntimeValue(std::any(expr.value[0]), "char");
            }
            break;

        case TokenType::TRUE:
            lastValue = RuntimeValue(std::any(true), "bool");
            break;

        case TokenType::FALSE:
            lastValue = RuntimeValue(std::any(false), "bool");
            break;

        case TokenType::NONE:
            lastValue = RuntimeValue(std::any(), "void");
            break;

        default:
            error("Unsupported literal type");
    }
}

void Interpreter::visit(VariableExpr& expr) {
    lastValue = currentEnv->get(expr.name.lexeme);
}

void Interpreter::visit(CallExpr& expr) {
    // Check if this is a dot call (module.function() or object.method())
    if (auto* binExpr = dynamic_cast<BinaryExpr*>(expr.callee.get())) {
        if (binExpr->op.type == TokenType::DOT) {
            // Evaluate the left side
            binExpr->left->accept(*this);
            RuntimeValue leftValue = lastValue;

            if (auto* rightVar = dynamic_cast<VariableExpr*>(binExpr->right.get())) {
                std::string methodName = rightVar->name.lexeme;

                // Evaluate arguments
                std::vector<RuntimeValue> args;
                for (const auto& arg : expr.arguments) {
                    arg->accept(*this);
                    args.push_back(lastValue);
                }

                // Handle object method calls
                if (leftValue.type == "object") {
                    auto instance = leftValue.asObject();

                    // Find the method in the class definition
                    if (classes.count(instance->className)) {
                        Class& cls = classes[instance->className];

                        if (cls.methods) {
                            for (const auto& member : cls.methods->get()) {
                                if (auto* funcDecl = dynamic_cast<FunctionDecl*>(member.get())) {
                                    if (funcDecl->name.lexeme == methodName) {
                                        // Call the method with 'this' bound to the instance
                                        enterScope();

                                        // Bind 'this' to the instance
                                        RuntimeValue thisValue(std::any(instance), "object");
                                        currentEnv->define("this", thisValue);

                                        // Bind method parameters
                                        for (size_t i = 0; i < funcDecl->params.size() && i < args.size(); ++i) {
                                            currentEnv->define(funcDecl->params[i].lexeme, args[i]);
                                        }

                                        // Execute method body
                                        RuntimeValue result;
                                        try {
                                            if (funcDecl->body) {
                                                for (const auto& stmt : *funcDecl->body) {
                                                    if (stmt) {
                                                        stmt->accept(*this);
                                                    }
                                                }
                                            }
                                            result = RuntimeValue(std::any(), "void");
                                        } catch (ReturnException& ret) {
                                            result = ret.value;
                                        }

                                        exitScope();
                                        lastValue = result;
                                        return;
                                    }
                                }
                            }
                        }
                    }

                    error("Undefined method: " + instance->className + "::" + methodName);
                    return;
                }

                // Handle module calls (both user-defined and native)
                if (leftValue.type == "module") {
                    if (auto* leftVar = dynamic_cast<VariableExpr*>(binExpr->left.get())) {
                        std::string moduleName = leftVar->name.lexeme;

                        // FIRST: Check if it's a user-defined module function
                        if (moduleFunctions.count(moduleName) &&
                            moduleFunctions.at(moduleName).count(methodName)) {
                            FunctionDecl& funcDeclRef = moduleFunctions.at(moduleName).at(methodName).get();
                            lastValue = callModuleFunction(moduleName, methodName, args, &funcDeclRef);
                            return;
                        }

                        // SECOND: Check if it's a native function
                        auto& registry = NativeRegistry::getInstance();
                        if (registry.isNative(moduleName, methodName)) {
                            lastValue = evaluateNativeCall(moduleName, methodName, args);
                            return;
                        }

                        // Function not found
                        error("Undefined function: " + moduleName + "::" + methodName);
                        return;
                    }
                }
            }
        }
    }

    // Regular function call
    if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.callee.get())) {
        std::string functionName = varExpr->name.lexeme;

        // Evaluate arguments
        std::vector<RuntimeValue> args;
        for (const auto& arg : expr.arguments) {
            arg->accept(*this);
            args.push_back(lastValue);
        }

        // Check if this is a prelude function (auto-imported)
        auto& registry = NativeRegistry::getInstance();
        if (registry.isNative("prelude", functionName)) {
            lastValue = evaluateNativeCall("prelude", functionName, args);
            return;
        }

        // Check if this is a class instantiation
        if (classes.count(functionName)) {
            lastValue = instantiateClass(functionName, args);
            return;
        }

        // If we're executing within a module, check if the function exists in that module first
        if (!currentExecutingModule.empty() &&
            moduleFunctions.count(currentExecutingModule) &&
            moduleFunctions.at(currentExecutingModule).count(functionName)) {
            FunctionDecl& funcDeclRef = moduleFunctions.at(currentExecutingModule).at(functionName).get();
            lastValue = callModuleFunction(currentExecutingModule, functionName, args, &funcDeclRef);
            return;
        }

        // Call user-defined function
        lastValue = callFunction(functionName, args);
    }
}

void Interpreter::visit(IndexExpr& expr) {
    // Array indexing not yet fully implemented
    // For now, just evaluate both sides and return a default value
    expr.object->accept(*this);
    expr.index->accept(*this);
    // Return a default value (0 for now)
    lastValue = RuntimeValue(std::any(0), "int");
}

void Interpreter::visit(GroupingExpr& expr) {
    expr.expression->accept(*this);
}

// --- Statement Visitors ---

void Interpreter::visit(VarDecl& stmt) {
    RuntimeValue value;

    if (stmt.initializer) {
        stmt.initializer->accept(*this);
        value = lastValue;
    } else {
        // Default initialization
        if (stmt.typeName == "int") {
            value = RuntimeValue(std::any(0), "int");
        } else if (stmt.typeName == "double") {
            value = RuntimeValue(std::any(0.0), "double");
        } else if (stmt.typeName == "string") {
            value = RuntimeValue(std::any(std::string("")), "string");
        } else if (stmt.typeName == "bool") {
            value = RuntimeValue(std::any(false), "bool");
        } else {
            value = RuntimeValue(std::any(), "void");
        }
    }

    currentEnv->define(stmt.name.lexeme, value);
}

void Interpreter::visit(FunctionDecl& stmt) {
    // If we're loading a module, register function in module namespace
    if (!currentModuleName.empty()) {
        moduleFunctions[currentModuleName].emplace(stmt.name.lexeme, std::ref(stmt));  // Safe reference
    } else {
        // Store function for later execution in global scope
        Function func;
        func.params = stmt.params;
        func.paramTypes = stmt.paramTypes;
        func.returnType = stmt.returnType;
        func.body = std::ref(stmt.body);  // Safe reference instead of raw pointer

        functions[stmt.name.lexeme] = func;
    }
}

void Interpreter::visit(ClassDecl& stmt) {
    // Store class definition for later instantiation
    Class cls;
    cls.name = stmt.name.lexeme;
    cls.methods = std::ref(stmt.methods);  // Safe reference instead of raw pointer
    classes[stmt.name.lexeme] = cls;
}

void Interpreter::visit(PackageDecl& stmt) {
    // Execute package contents
    for (const auto& s : stmt.declarations) {
        if (s) s->accept(*this);
    }
}

void Interpreter::visit(UseStmt& stmt) {
    namespace fs = std::filesystem;
    std::string moduleName = stmt.moduleName.lexeme;

    // First, register the module as a module object in the environment
    // This allows module.function() syntax to work
    RuntimeValue moduleValue(std::any(), "module");
    currentEnv->define(moduleName, moduleValue);

    // Search paths in priority order
    std::vector<std::string> searchPaths = {
        "src/" + moduleName,           // Internal packages
        "deps/" + moduleName + "/src", // External dependencies
        "deps/" + moduleName,          // Alternative layout
    };

    // Try each search path until we find the module
    for (const auto& moduleDir : searchPaths) {
        if (fs::exists(moduleDir) && fs::is_directory(moduleDir)) {
            // Set current module name to track module context
            currentModuleName = moduleName;

            // Load all .st files in the module directory
            for (const auto& entry : fs::directory_iterator(moduleDir)) {
                if (entry.path().extension() == ".st") {
                    std::ifstream file(entry.path());
                    if (!file.is_open()) continue;

                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    std::string source = buffer.str();

                    try {
                        // Lex, parse, and execute the module file
                        Lexer lexer(source);
                        std::vector<Token> tokens = lexer.scanTokens();

                        Parser parser(tokens);
                        std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

                        // Store the statements first to keep ClassDecl alive
                        moduleStatements.push_back(std::move(statements));

                        // Execute the module statements (this will register classes, functions, etc.)
                        for (const auto& moduleStmt : moduleStatements.back()) {
                            if (moduleStmt) {
                                moduleStmt->accept(*this);
                            }
                        }
                    } catch (const std::exception& e) {
                        error("Error loading module '" + moduleName + "': " + e.what());
                    }
                }
            }

            // Clear current module name after loading
            currentModuleName = "";
            return; // Module found and loaded
        }
    }

    // If we get here, the module wasn't found in any search path
    // This is okay for native modules (math, log, etc.)
}

void Interpreter::visit(BlockStmt& stmt) {
    enterScope();
    for (const auto& s : stmt.statements) {
        if (s) s->accept(*this);
    }
    exitScope();
}

void Interpreter::visit(ExpressionStmt& stmt) {
    // Just evaluate the expression and discard the result
    stmt.expression->accept(*this);
}

void Interpreter::visit(PrintStmt& stmt) {
    stmt.expression->accept(*this);
    RuntimeValue value = lastValue;

    if (value.type == "string") {
        std::cout << value.asString() << std::endl;
    } else if (value.type == "char") {
        std::cout << value.asChar() << std::endl;
    } else if (value.type == "int") {
        std::cout << value.asInt() << std::endl;
    } else if (value.type == "double") {
        std::cout << value.asDouble() << std::endl;
    } else if (value.type == "bool") {
        std::cout << (value.asBool() ? "true" : "false") << std::endl;
    }
}

void Interpreter::visit(IfStmt& stmt) {
    stmt.condition->accept(*this);
    RuntimeValue condition = lastValue;

    if (isTruthy(condition)) {
        stmt.thenBranch->accept(*this);
    } else if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }
}

void Interpreter::visit(WhileStmt& stmt) {
    while (true) {
        stmt.condition->accept(*this);
        RuntimeValue condition = lastValue;

        if (!isTruthy(condition)) break;

        stmt.body->accept(*this);
    }
}

void Interpreter::visit(ReturnStmt& stmt) {
    RuntimeValue value;

    if (stmt.value) {
        stmt.value->accept(*this);
        value = lastValue;
    } else {
        value = RuntimeValue(std::any(), "void");
    }

    throw ReturnException(value);
}

// --- Helper Methods ---

RuntimeValue Interpreter::evaluateNativeCall(const std::string& moduleName,
                                             const std::string& functionName,
                                             const std::vector<RuntimeValue>& args) {
    auto& registry = NativeRegistry::getInstance();

    if (!registry.isNative(moduleName, functionName)) {
        error("Native function not found: " + moduleName + "::" + functionName);
    }

    // Convert RuntimeValue arguments to std::any for NativeRegistry
    // Extract actual values from variant before converting to std::any
    std::vector<std::any> nativeArgs;
    for (const auto& arg : args) {
        std::any anyValue;
        if (std::holds_alternative<int>(arg.value)) {
            anyValue = std::get<int>(arg.value);
        } else if (std::holds_alternative<double>(arg.value)) {
            anyValue = std::get<double>(arg.value);
        } else if (std::holds_alternative<std::string>(arg.value)) {
            anyValue = std::get<std::string>(arg.value);
        } else if (std::holds_alternative<char>(arg.value)) {
            anyValue = std::get<char>(arg.value);
        } else if (std::holds_alternative<bool>(arg.value)) {
            anyValue = std::get<bool>(arg.value);
        } else if (std::holds_alternative<std::shared_ptr<ClassInstance>>(arg.value)) {
            anyValue = std::get<std::shared_ptr<ClassInstance>>(arg.value);
        }
        // std::monostate (void) remains as empty std::any
        nativeArgs.push_back(anyValue);
    }

    // Call the native function
    auto nativeFunc = registry.getFunction(moduleName, functionName);
    std::any result = nativeFunc(nativeArgs);

    // Convert result back to RuntimeValue
    // Determine type based on module and function
    std::string resultType = "void";

    if (moduleName == "math") {
        if (functionName == "randomInt" || functionName == "sign") {
            resultType = "int";
        } else {
            resultType = "double";
        }
    } else if (moduleName == "strings") {
        if (functionName == "length" || functionName == "indexOf") {
            resultType = "int";
        } else if (functionName == "contains" || functionName == "isEmpty") {
            resultType = "bool";
        } else {
            resultType = "string";
        }
    } else if (moduleName == "log" || moduleName == "io") {
        resultType = "void";
    }

    return RuntimeValue(result, resultType);
}

RuntimeValue Interpreter::callFunction(const std::string& name,
                                       const std::vector<RuntimeValue>& args) {
    if (functions.find(name) == functions.end()) {
        error("Undefined function: " + name);
    }

    Function& func = functions[name];

    // Check if this is "main" function - special case
    if (name == "main") {
        enterScope();

        try {
            if (func.body && func.body->get()) {
                for (const auto& stmt : *func.body->get()) {
                    if (stmt) stmt->accept(*this);
                }
            }
        } catch (ReturnException& ret) {
            exitScope();
            return ret.value;
        }

        exitScope();
        return RuntimeValue(std::any(0), "int");
    }

    // Regular function call
    enterScope();

    // Bind parameters
    for (size_t i = 0; i < func.params.size() && i < args.size(); ++i) {
        currentEnv->define(func.params[i].lexeme, args[i]);
    }

    RuntimeValue result;
    try {
        if (func.body && func.body->get()) {
            for (const auto& stmt : *func.body->get()) {
                if (stmt) stmt->accept(*this);
            }
        }
        result = RuntimeValue(std::any(), "void");
    } catch (ReturnException& ret) {
        result = ret.value;
    }

    exitScope();
    return result;
}

RuntimeValue Interpreter::callModuleFunction(const std::string& moduleName,
                                             const std::string& functionName,
                                             const std::vector<RuntimeValue>& args,
                                             FunctionDecl* funcDecl) {
    // Save previous executing module context
    std::string previousModule = currentExecutingModule;
    currentExecutingModule = moduleName;

    // Create new scope for function execution
    enterScope();

    // Bind parameters
    for (size_t i = 0; i < funcDecl->params.size() && i < args.size(); ++i) {
        currentEnv->define(funcDecl->params[i].lexeme, args[i]);
    }

    // Execute function body
    RuntimeValue result;
    try {
        if (funcDecl->body) {
            for (const auto& stmt : *funcDecl->body) {
                if (stmt) {
                    stmt->accept(*this);
                }
            }
        }
        result = RuntimeValue(std::any(), "void");
    } catch (ReturnException& ret) {
        result = ret.value;
    }

    exitScope();

    // Restore previous executing module context
    currentExecutingModule = previousModule;

    return result;
}

RuntimeValue Interpreter::instantiateClass(const std::string& className,
                                           const std::vector<RuntimeValue>& args) {
    if (classes.find(className) == classes.end()) {
        error("Undefined class: " + className);
    }

    Class& cls = classes[className];

    // Create a new class instance
    auto instance = std::make_shared<ClassInstance>();
    instance->className = className;

    // Register with garbage collector for cycle detection
    gc.registerObject(instance);

    // Process class members (fields and methods)
    // Find constructor and field declarations
    FunctionDecl* constructor = nullptr;

    if (cls.methods) {
        for (const auto& member : cls.methods->get()) {
            if (auto* fieldDecl = dynamic_cast<VarDecl*>(member.get())) {
            // Initialize field with default value
            RuntimeValue defaultValue;
            if (fieldDecl->typeName == "int") {
                defaultValue = RuntimeValue(std::any(0), "int");
            } else if (fieldDecl->typeName == "double") {
                defaultValue = RuntimeValue(std::any(0.0), "double");
            } else if (fieldDecl->typeName == "string") {
                defaultValue = RuntimeValue(std::any(std::string("")), "string");
            } else if (fieldDecl->typeName == "bool") {
                defaultValue = RuntimeValue(std::any(false), "bool");
            } else {
                defaultValue = RuntimeValue(std::any(), "void");
            }
            instance->fields[fieldDecl->name.lexeme] = defaultValue;
        } else if (auto* funcDecl = dynamic_cast<FunctionDecl*>(member.get())) {
            // Check if this is the constructor
            if (funcDecl->name.lexeme == "constructor") {
                constructor = funcDecl;
            }
        }
        }
    }

    // Call constructor if exists
    if (constructor) {
        // Create a temporary environment with 'this' bound to the instance
        enterScope();

        // Bind 'this' to the instance
        RuntimeValue thisValue(std::any(instance), "object");
        currentEnv->define("this", thisValue);

        // Bind constructor parameters
        for (size_t i = 0; i < constructor->params.size() && i < args.size(); ++i) {
            currentEnv->define(constructor->params[i].lexeme, args[i]);
        }

        // Execute constructor body
        try {
            if (constructor->body) {
                for (const auto& stmt : *constructor->body) {
                    if (stmt) {
                        stmt->accept(*this);
                    }
                }
            }
        } catch (ReturnException& ret) {
            // Constructor shouldn't return, but handle it anyway
        }

        exitScope();
    }

    // Return the instance wrapped in a RuntimeValue
    return RuntimeValue(std::any(instance), "object");
}

bool Interpreter::isTruthy(const RuntimeValue& value) {
    if (value.type == "bool") {
        return value.asBool();
    } else if (value.type == "int") {
        return value.asInt() != 0;
    } else if (value.type == "double") {
        return value.asDouble() != 0.0;
    } else if (value.type == "string") {
        return !value.asString().empty();
    }
    return false;
}

void Interpreter::collectRoots(std::vector<RuntimeValue>& roots) {
    // Collect RuntimeValues from the active environment chain only
    // Walk from current environment up to root via parent pointers
    Environment* env = currentEnv;
    while (env) {
        for (const auto& var : env->variables) {
            roots.push_back(var.second);
        }
        env = env->parent;
    }

    // Add lastValue as a root (it might be on the "stack")
    if (lastValue.type == "object") {
        roots.push_back(lastValue);
    }
}

void Interpreter::maybeCollectGarbage() {
    // Only collect if threshold is reached
    if (!gc.shouldCollect()) {
        return;
    }

    // Collect roots
    std::vector<RuntimeValue> roots;
    collectRoots(roots);

    // Run GC
    size_t collected = gc.collect(roots);

    // Optional: print GC statistics in verbose mode
    // Uncomment for debugging:
    // if (collected > 0) {
    //     std::cerr << "[GC] Collected " << collected << " cyclic objects\n";
    // }
}

} // namespace stratos
