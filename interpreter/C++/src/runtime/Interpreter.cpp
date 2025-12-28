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
    // Create global environment
    auto globalEnv = std::make_unique<Environment>();
    currentEnv = globalEnv.get();
    environments.push_back(std::move(globalEnv));
}

void Interpreter::execute(const std::vector<std::unique_ptr<Stmt>>& statements) {
    // Execute all statements
    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->accept(*this);
        }
    }
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
                            auto& field = instance->fields[fieldName];
                            if (field.type == "int" && value.type == "int") {
                                field = RuntimeValue(std::any(field.asInt() + value.asInt()), "int");
                            } else if (field.type == "double" && value.type == "double") {
                                field = RuntimeValue(std::any(field.asDouble() + value.asDouble()), "double");
                            }
                        } else if (expr.op.type == TokenType::MINUS_EQUAL) {
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

                        for (const auto& member : *cls.methods) {
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

                    error("Undefined method: " + instance->className + "::" + methodName);
                    return;
                }

                // Handle native module calls
                if (leftValue.type == "module") {
                    if (auto* leftVar = dynamic_cast<VariableExpr*>(binExpr->left.get())) {
                        std::string moduleName = leftVar->name.lexeme;
                        lastValue = evaluateNativeCall(moduleName, methodName, args);
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
    // Store function for later execution
    Function func;
    func.params = stmt.params;
    func.paramTypes = stmt.paramTypes;
    func.returnType = stmt.returnType;
    func.body = &stmt.body;

    functions[stmt.name.lexeme] = func;
}

void Interpreter::visit(ClassDecl& stmt) {
    // Store class definition for later instantiation
    Class cls;
    cls.name = stmt.name.lexeme;
    cls.methods = &stmt.methods;
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

    // Check if this is a user-defined module that needs to be loaded
    std::string moduleDir = "src/" + moduleName;
    if (fs::exists(moduleDir) && fs::is_directory(moduleDir)) {
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
    }
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
    std::vector<std::any> nativeArgs;
    for (const auto& arg : args) {
        nativeArgs.push_back(arg.value);
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
            if (func.body && *func.body) {
                for (const auto& stmt : **func.body) {
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
        if (func.body && *func.body) {
            for (const auto& stmt : **func.body) {
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

RuntimeValue Interpreter::instantiateClass(const std::string& className,
                                           const std::vector<RuntimeValue>& args) {
    if (classes.find(className) == classes.end()) {
        error("Undefined class: " + className);
    }

    Class& cls = classes[className];

    // Create a new class instance
    auto instance = std::make_shared<ClassInstance>();
    instance->className = className;

    // Process class members (fields and methods)
    // Find constructor and field declarations
    FunctionDecl* constructor = nullptr;

    for (const auto& member : *cls.methods) {
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

} // namespace stratos
