#include "stratos/Interpreter.h"
#include <iostream>
#include <cmath>

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
                std::string leftStr = (left.type == "string") ? left.asString() : std::to_string(left.asInt());
                std::string rightStr = (right.type == "string") ? right.asString() : std::to_string(right.asInt());
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

        case TokenType::DOT:
            // Member access - handled in CallExpr for module.function()
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
    // Check if this is a native module call (module.function())
    if (auto* binExpr = dynamic_cast<BinaryExpr*>(expr.callee.get())) {
        if (binExpr->op.type == TokenType::DOT) {
            if (auto* leftVar = dynamic_cast<VariableExpr*>(binExpr->left.get())) {
                if (auto* rightVar = dynamic_cast<VariableExpr*>(binExpr->right.get())) {
                    std::string moduleName = leftVar->name.lexeme;
                    std::string functionName = rightVar->name.lexeme;

                    // Evaluate arguments
                    std::vector<RuntimeValue> args;
                    for (const auto& arg : expr.arguments) {
                        arg->accept(*this);
                        args.push_back(lastValue);
                    }

                    // Call native function
                    lastValue = evaluateNativeCall(moduleName, functionName, args);
                    return;
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

        // Handle built-in print function
        if (functionName == "print") {
            if (args.size() > 0) {
                if (args[0].type == "string") {
                    std::cout << args[0].asString() << std::endl;
                } else if (args[0].type == "int") {
                    std::cout << args[0].asInt() << std::endl;
                } else if (args[0].type == "double") {
                    std::cout << args[0].asDouble() << std::endl;
                } else if (args[0].type == "bool") {
                    std::cout << (args[0].asBool() ? "true" : "false") << std::endl;
                }
            }
            lastValue = RuntimeValue(std::any(), "void");
            return;
        }

        // Call user-defined function
        lastValue = callFunction(functionName, args);
    }
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
    // Class declarations not fully supported in interpreter yet
    // For now, just skip
}

void Interpreter::visit(PackageDecl& stmt) {
    // Execute package contents
    for (const auto& s : stmt.declarations) {
        if (s) s->accept(*this);
    }
}

void Interpreter::visit(UseStmt& stmt) {
    // Module imports are handled - no runtime action needed
}

void Interpreter::visit(BlockStmt& stmt) {
    enterScope();
    for (const auto& s : stmt.statements) {
        if (s) s->accept(*this);
    }
    exitScope();
}

void Interpreter::visit(PrintStmt& stmt) {
    stmt.expression->accept(*this);
    RuntimeValue value = lastValue;

    if (value.type == "string") {
        std::cout << value.asString() << std::endl;
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
