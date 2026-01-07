#include "stratos/Interpreter.h"
#include "stratos/Lexer.h"
#include "stratos/Parser.h"
#include "stratos/MemoryProfiler.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

namespace stratos {

Interpreter::Interpreter() {
    // Reserve capacity to prevent vector reallocation (which would invalidate currentEnv pointer)
    // This ensures currentEnv remains valid throughout execution
    environments.reserve(1000);

    // Create global environment
    auto globalEnv = std::make_unique<Environment>();
    currentEnv = globalEnv.get();
    environments.push_back(std::move(globalEnv));

    // Auto-import core modules (prelude modules - available without 'use' statement)
    RuntimeValue mapsModule(std::any(), "module");
    currentEnv->define("maps", mapsModule);

    // Connect MemoryProfiler with GC
    MemoryProfiler::instance().setGarbageCollector(&gc);
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

    // Measure GC pause time
    auto start = std::chrono::high_resolution_clock::now();
    size_t collected = gc.collect(roots);
    auto end = std::chrono::high_resolution_clock::now();

    double pauseMs = std::chrono::duration<double, std::milli>(end - start).count();

    // Report to MemoryProfiler
    MemoryProfiler::instance().recordCollection(pauseMs, collected, collected);

    // Optional: Report cycles broken
    if (collected > 0) {
        std::cerr << "[GC] Broke " << collected << " circular reference(s) in "
                  << pauseMs << "ms\n";
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
        // Check for enum value access FIRST (e.g., Color.RED)
        // Don't evaluate left side yet, as enum names aren't variables
        if (auto* leftVar = dynamic_cast<VariableExpr*>(expr.left.get())) {
            if (auto* rightVar = dynamic_cast<VariableExpr*>(expr.right.get())) {
                std::string enumName = leftVar->name.lexeme;
                std::string valueName = rightVar->name.lexeme;
                std::string fullName = enumName + "." + valueName;

                // Try to look up the enum value
                try {
                    lastValue = currentEnv->get(fullName);
                    return;
                } catch (...) {
                    // Not an enum value, continue to evaluate normally
                }
            }
        }

        // Evaluate left side for other DOT access patterns
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

    // Handle logical operators with short-circuit evaluation
    if (expr.op.type == TokenType::AND) {
        expr.left->accept(*this);
        RuntimeValue left = lastValue;

        // Short-circuit: if left is false, don't evaluate right
        if (!isTruthy(left)) {
            lastValue = RuntimeValue(std::any(false), "bool");
            return;
        }

        expr.right->accept(*this);
        RuntimeValue right = lastValue;
        lastValue = RuntimeValue(std::any(isTruthy(right)), "bool");
        return;
    }

    if (expr.op.type == TokenType::OR) {
        expr.left->accept(*this);
        RuntimeValue left = lastValue;

        // Short-circuit: if left is true, don't evaluate right
        if (isTruthy(left)) {
            lastValue = RuntimeValue(std::any(true), "bool");
            return;
        }

        expr.right->accept(*this);
        RuntimeValue right = lastValue;
        lastValue = RuntimeValue(std::any(isTruthy(right)), "bool");
        return;
    }

    // Regular binary operators - evaluate both sides
   // std::cerr << "[DEBUG] BinaryExpr: operator type=" << static_cast<int>(expr.op.type) << std::endl;
   // std::cerr << "[DEBUG] Evaluating left side..." << std::endl;
    expr.left->accept(*this);
    RuntimeValue left = lastValue;
 //   std::cerr << "[DEBUG] Left side evaluated, type=" << left.type << std::endl;

  //  std::cerr << "[DEBUG] Evaluating right side..." << std::endl;
    expr.right->accept(*this);
    RuntimeValue right = lastValue;
   // std::cerr << "[DEBUG] Right side evaluated, type=" << right.type << std::endl;

  //  std::cerr << "[DEBUG] Entering switch statement..." << std::endl;
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
                lastValue = RuntimeValue(left.asInt() == right.asInt());
            } else if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(left.asDouble() == right.asDouble());
            } else if (left.type == "string" && right.type == "string") {
                lastValue = RuntimeValue(left.asString() == right.asString());
            }
            break;

        case TokenType::BANG_EQUAL:
            if (left.type == "int" && right.type == "int") {
                lastValue = RuntimeValue(left.asInt() != right.asInt());
            } else if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(left.asDouble() != right.asDouble());
            }
            break;

        case TokenType::LESS:
          //  std::cerr << "[DEBUG] LESS operator: left.type=" << left.type << ", right.type=" << right.type << std::endl;
            if (left.type == "double" && right.type == "double") {
              //  std::cerr << "[DEBUG] Taking double path" << std::endl;
                bool result = left.asDouble() < right.asDouble();
              //  std::cerr << "[DEBUG] Comparison result: " << result << std::endl;
                lastValue = RuntimeValue(result);
             //   std::cerr << "[DEBUG] Created RuntimeValue" << std::endl;
            } else if (left.type == "int" && right.type == "int") {
              //  std::cerr << "[DEBUG] Taking int path" << std::endl;
              //  std::cerr << "[DEBUG] About to call asInt() on left" << std::endl;
                int leftVal = left.asInt();
              //  std::cerr << "[DEBUG] left value: " << leftVal << std::endl;
              //  std::cerr << "[DEBUG] About to call asInt() on right" << std::endl;
                int rightVal = right.asInt();
              //  std::cerr << "[DEBUG] right value: " << rightVal << std::endl;
                bool result = leftVal < rightVal;
              //  std::cerr << "[DEBUG] Comparison result: " << result << std::endl;
                lastValue = RuntimeValue(result);
             //   std::cerr << "[DEBUG] Created RuntimeValue" << std::endl;
            } else {
              //  std::cerr << "[DEBUG] Type mismatch!" << std::endl;
                error("Type error in < comparison: cannot compare " + left.type + " with " + right.type);
            }
            //std::cerr << "[DEBUG] LESS operator complete" << std::endl;
            break;

        case TokenType::LESS_EQUAL:
            if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(left.asDouble() <= right.asDouble());
            } else {
                lastValue = RuntimeValue(left.asInt() <= right.asInt());
            }
            break;

        case TokenType::GREATER:
            if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(left.asDouble() > right.asDouble());
            } else {
                lastValue = RuntimeValue(left.asInt() > right.asInt());
            }
            break;

        case TokenType::GREATER_EQUAL:
            if (left.type == "double" && right.type == "double") {
                lastValue = RuntimeValue(left.asDouble() >= right.asDouble());
            } else {
                lastValue = RuntimeValue(left.asInt() >= right.asInt());
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
            lastValue = RuntimeValue(!isTruthy(operand));
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
            lastValue = RuntimeValue(true);
            break;

        case TokenType::FALSE:
            lastValue = RuntimeValue(false);
            break;

        case TokenType::NONE:
            lastValue = RuntimeValue(std::any(), "void");
            break;

        default:
            error("Unsupported literal type");
    }
}

void Interpreter::visit(VariableExpr& expr) {
   // std::cerr << "[DEBUG] VariableExpr: looking up '" << expr.name.lexeme << "'" << std::endl;
    lastValue = currentEnv->get(expr.name.lexeme);
  //  std::cerr << "[DEBUG] VariableExpr: found type=" << lastValue.type << std::endl;
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

                // Handle string method calls (content.split(), content.length(), etc.)
                if (leftValue.type == "string") {
                    // Map string methods to strings module functions
                    // The string itself becomes the first argument
                    args.insert(args.begin(), leftValue);
                    lastValue = evaluateNativeCall("strings", methodName, args);
                    return;
                }

                // Handle array method calls (array.length(), array.first(), etc.)
                if (leftValue.type.starts_with("array")) {
                    if (auto* anyPtr = std::get_if<std::any>(&leftValue.value)) {
                        if (leftValue.type == "array<string>") {
                            try {
                                auto& vec = std::any_cast<std::vector<std::string>&>(*anyPtr);

                                if (methodName == "length") {
                                    lastValue = RuntimeValue(static_cast<int>(vec.size()));
                                    return;
                                } else if (methodName == "isEmpty") {
                                    lastValue = RuntimeValue(vec.empty());
                                    return;
                                } else if (methodName == "first") {
                                    if (!vec.empty()) {
                                        lastValue = RuntimeValue(vec.front());
                                    } else {
                                        lastValue = RuntimeValue(std::string(""));
                                    }
                                    return;
                                } else if (methodName == "last") {
                                    if (!vec.empty()) {
                                        lastValue = RuntimeValue(vec.back());
                                    } else {
                                        lastValue = RuntimeValue(std::string(""));
                                    }
                                    return;
                                } else if (methodName == "clear") {
                                    lastValue = RuntimeValue(std::any(std::vector<std::string>()), "array<string>");
                                    return;
                                } else if (methodName == "contains") {
                                    if (!args.empty()) {
                                        std::string searchValue = args[0].asString();
                                        bool found = std::find(vec.begin(), vec.end(), searchValue) != vec.end();
                                        lastValue = RuntimeValue(found);
                                    } else {
                                        error("contains() requires a value argument");
                                    }
                                    return;
                                } else if (methodName == "indexOf") {
                                    if (!args.empty()) {
                                        std::string searchValue = args[0].asString();
                                        auto it = std::find(vec.begin(), vec.end(), searchValue);
                                        if (it != vec.end()) {
                                            lastValue = RuntimeValue(static_cast<int>(std::distance(vec.begin(), it)));
                                        } else {
                                            lastValue = RuntimeValue(-1);
                                        }
                                    } else {
                                        error("indexOf() requires a value argument");
                                    }
                                    return;
                                } else if (methodName == "reverse") {
                                    std::vector<std::string> reversed(vec.rbegin(), vec.rend());
                                    lastValue = RuntimeValue(std::any(reversed), "array<string>");
                                    return;
                                } else if (methodName == "join") {
                                    std::string separator = args.empty() ? "," : args[0].asString();
                                    std::string result;
                                    for (size_t i = 0; i < vec.size(); i++) {
                                        result += vec[i];
                                        if (i < vec.size() - 1) result += separator;
                                    }
                                    lastValue = RuntimeValue(result);
                                    return;
                                }
                            } catch (const std::bad_any_cast&) {
                                error("Internal error: Failed to cast array value");
                            }
                        }
                        // Add more array types here as needed
                    }
                    error("Unknown array method: " + methodName);
                }

                // Handle map method calls (map.get(), map.set(), map.has(), etc.)
                if (leftValue.type.starts_with("map")) {
                    // Map methods forward to the maps module with map as first argument
                    args.insert(args.begin(), leftValue);
                    lastValue = evaluateNativeCall("maps", methodName, args);
                    return;
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
                                        // Save and set executing module context
                                        std::string previousModule = currentExecutingModule;
                                        currentExecutingModule = cls.moduleName;

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

                                        // Restore previous executing module context
                                        currentExecutingModule = previousModule;

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

        // Check if this is a native function in the current executing module
        if (!currentExecutingModule.empty() && registry.isNative(currentExecutingModule, functionName)) {
            lastValue = evaluateNativeCall(currentExecutingModule, functionName, args);
            return;
        }

        // Call user-defined function
        lastValue = callFunction(functionName, args);
    }
}

void Interpreter::visit(IndexExpr& expr) {
    // Evaluate the object being indexed
    expr.object->accept(*this);
    RuntimeValue arrayValue = lastValue;

    // Evaluate the index
    expr.index->accept(*this);
    RuntimeValue indexValue = lastValue;

    // Check if this is an array type
    if (arrayValue.type.starts_with("array")) {
        // Extract the std::any containing the vector
        if (auto* anyPtr = std::get_if<std::any>(&arrayValue.value)) {
            // Get the index as an integer
            int idx = indexValue.asInt();

            // Try to cast to vector<string> (most common from split)
            if (arrayValue.type == "array<string>") {
                try {
                    auto& vec = std::any_cast<std::vector<std::string>&>(*anyPtr);
                    if (idx >= 0 && idx < static_cast<int>(vec.size())) {
                        lastValue = RuntimeValue(vec[idx]);
                        return;
                    } else {
                        error("Array index out of bounds: " + std::to_string(idx));
                    }
                } catch (const std::bad_any_cast&) {
                    error("Internal error: Failed to cast array value");
                }
            }
            // Add more array types here as needed
        }
    }

    // Check if this is a map type
    if (arrayValue.type.starts_with("map")) {
        // Extract the std::any containing the map
        if (auto* anyPtr = std::get_if<std::any>(&arrayValue.value)) {
            // Get the key as a string
            std::string key = indexValue.asString();

            // Try to cast to unordered_map<string, string>
            if (arrayValue.type == "map<string,string>") {
                try {
                    auto& map = std::any_cast<std::unordered_map<std::string, std::string>&>(*anyPtr);
                    auto it = map.find(key);
                    if (it != map.end()) {
                        lastValue = RuntimeValue(it->second);
                        return;
                    } else {
                        // Return empty string for missing keys (similar to maps.get)
                        lastValue = RuntimeValue(std::string(""));
                        return;
                    }
                } catch (const std::bad_any_cast&) {
                    error("Internal error: Failed to cast map value");
                }
            }
            // Add more map types here as needed
        }
    }

    error("Cannot index type: " + arrayValue.type);
}

void Interpreter::visit(GroupingExpr& expr) {
    expr.expression->accept(*this);
}

// --- Statement Visitors ---

void Interpreter::visit(VarDecl& stmt) {
    //std::cerr << "[DEBUG] VarDecl: declaring variable '" << stmt.name.lexeme << "'" << std::endl;
    RuntimeValue value;

    if (stmt.initializer) {
       // std::cerr << "[DEBUG] VarDecl: evaluating initializer..." << std::endl;
        stmt.initializer->accept(*this);
        value = lastValue;
       // std::cerr << "[DEBUG] VarDecl: initializer evaluated, type=" << value.type << std::endl;
    } else {
        //std::cerr << "[DEBUG] VarDecl: no initializer, using default" << std::endl;
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

  //  std::cerr << "[DEBUG] VarDecl: defining variable with type=" << value.type << std::endl;
    currentEnv->define(stmt.name.lexeme, value);
  //  std::cerr << "[DEBUG] VarDecl: variable defined successfully" << std::endl;
}

void Interpreter::visit(FunctionDecl& stmt) {
    // Only register functions that have bodies
    // Function declarations without bodies are meant to be native function declarations
    if (!stmt.body) {
        return;  // Skip registration - this is a native function declaration
    }

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
    cls.moduleName = currentModuleName;  // Track which module this class belongs to
    cls.methods = std::ref(stmt.methods);  // Safe reference instead of raw pointer
    classes[stmt.name.lexeme] = cls;
}

void Interpreter::visit(EnumDecl& stmt) {
    // Store enum values in the environment
    // Each enum value is stored as EnumName.VALUE = integer index
    for (size_t i = 0; i < stmt.values.size(); ++i) {
        std::string fullName = stmt.name.lexeme + "." + stmt.values[i].lexeme;
        // Store the enum value as an integer (its index)
        RuntimeValue value(std::any(static_cast<int>(i)), "int");
        currentEnv->define(fullName, value);
    }
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
        "src/" + moduleName,                                  // Internal packages (highest priority)
        "deps/" + moduleName + "/src",                        // External dependencies
        "deps/" + moduleName,                                  // Alternative layout
        "std/" + moduleName,                                  // Standard library
        "std/encoding/" + moduleName,                         // Encoding modules
        "../std/" + moduleName,                               // One level up
        "../std/encoding/" + moduleName,
        "../../std/" + moduleName,                            // Two levels up
        "../../std/encoding/" + moduleName,
        "../../../std/" + moduleName,                         // Three levels up
        "../../../std/encoding/" + moduleName,
    };

    // Try each search path until we find the module
    for (const auto& modulePath : searchPaths) {
        std::vector<std::string> filesToLoad;

        // Check if it's a directory
        if (fs::exists(modulePath) && fs::is_directory(modulePath)) {
            // Load all .st files in the module directory
            for (const auto& entry : fs::directory_iterator(modulePath)) {
                if (entry.path().extension() == ".st") {
                    filesToLoad.push_back(entry.path().string());
                }
            }
        }
        // Check if init.st exists in this path
        else {
            std::string initFile = modulePath + "/init.st";
            if (fs::exists(initFile)) {
                filesToLoad.push_back(initFile);
            }
        }

        // If we found files to load, process them
        if (!filesToLoad.empty()) {
            // Set current module name to track module context
            currentModuleName = moduleName;

            for (const auto& filePath : filesToLoad) {
                std::ifstream file(filePath);
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
                    error("Error loading module '" + moduleName + "' from " + filePath + ": " + e.what());
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
   // std::cerr << "[DEBUG] IfStmt: evaluating condition..." << std::endl;
    stmt.condition->accept(*this);
    RuntimeValue condition = lastValue;
   // std::cerr << "[DEBUG] IfStmt: condition type=" << condition.type << std::endl;

    bool truthiness = isTruthy(condition);
  //  std::cerr << "[DEBUG] IfStmt: isTruthy=" << truthiness << std::endl;

    if (truthiness) {
     //   std::cerr << "[DEBUG] IfStmt: executing then branch" << std::endl;
        stmt.thenBranch->accept(*this);
    } else if (stmt.elseBranch) {
     //   std::cerr << "[DEBUG] IfStmt: executing else branch" << std::endl;
        stmt.elseBranch->accept(*this);
    } else {
        std::cerr << "[DEBUG] IfStmt: no branch executed" << std::endl;
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

void Interpreter::visit(ForStmt& stmt) {
    // Evaluate the iterable expression
    stmt.iterable->accept(*this);
    RuntimeValue iterableValue = lastValue;

    // Handle array iteration
    if (iterableValue.type.starts_with("array")) {
        if (!std::holds_alternative<std::any>(iterableValue.value)) {
            error("Invalid array value in for loop");
        }

        auto& anyValue = std::get<std::any>(iterableValue.value);

        // Handle array<string>
        if (iterableValue.type == "array<string>") {
            try {
                auto& vec = std::any_cast<std::vector<std::string>&>(anyValue);

                // Iterate over each element
                for (const auto& element : vec) {
                    // Create new scope for loop body
                    enterScope();

                    // Define loop variable with current element
                    currentEnv->define(stmt.variable.lexeme, RuntimeValue(element));

                    // Execute loop body
                    stmt.body->accept(*this);

                    exitScope();
                }
            } catch (const std::bad_any_cast&) {
                error("Failed to iterate over array in for loop");
            }
        } else {
            error("Unsupported array type in for loop: " + iterableValue.type);
        }
    }
    // Handle map iteration (iterate over keys)
    else if (iterableValue.type.starts_with("map")) {
        if (!std::holds_alternative<std::any>(iterableValue.value)) {
            error("Invalid map value in for loop");
        }

        auto& anyValue = std::get<std::any>(iterableValue.value);

        if (iterableValue.type == "map<string,string>") {
            try {
                auto& map = std::any_cast<std::unordered_map<std::string, std::string>&>(anyValue);

                // Iterate over each key
                for (const auto& pair : map) {
                    // Create new scope for loop body
                    enterScope();

                    // Define loop variable with current key
                    currentEnv->define(stmt.variable.lexeme, RuntimeValue(pair.first));

                    // Execute loop body
                    stmt.body->accept(*this);

                    exitScope();
                }
            } catch (const std::bad_any_cast&) {
                error("Failed to iterate over map in for loop");
            }
        } else {
            error("Unsupported map type in for loop: " + iterableValue.type);
        }
    }
    else {
        error("For loop requires an iterable (array or map), got: " + iterableValue.type);
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
        } else if (std::holds_alternative<std::any>(arg.value)) {
            // Handle arrays and maps (stored as std::any)
            anyValue = std::get<std::any>(arg.value);
        }
        // std::monostate (void) remains as empty std::any
        nativeArgs.push_back(anyValue);
    }

    // Call the native function
    auto nativeFunc = registry.getFunction(moduleName, functionName);
    std::any result = nativeFunc(nativeArgs);

    // Convert result back to RuntimeValue
    // Use FunctionSignature if available, otherwise infer from module
    std::string resultType = "void";

    if (registry.hasSignature(moduleName, functionName)) {
        // Use the registered signature's return type
        auto signature = registry.getSignature(moduleName, functionName);
        resultType = signature.returnType;
    } else {
        // Fallback: Determine type based on module and function for legacy functions
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
        } else if (moduleName == "io") {
            // File info functions that return bool
            if (functionName == "exists" || functionName == "isFile" || functionName == "isDirectory") {
                resultType = "bool";
            }
            // Functions that return int
            else if (functionName == "fileSize") {
                resultType = "int";
            }
            // Functions that return string
            else if (functionName == "readFile" || functionName == "join" ||
                     functionName == "basename" || functionName == "dirname" ||
                     functionName == "extension" || functionName == "absolute") {
                resultType = "string";
            }
            // Everything else defaults to void (writeFile, remove, mkdir, etc.)
            else {
                resultType = "void";
            }
        } else if (moduleName == "log") {
            resultType = "void";
        }
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

    // Run GC with timing
    auto start = std::chrono::high_resolution_clock::now();
    size_t collected = gc.collect(roots);
    auto end = std::chrono::high_resolution_clock::now();

    double pauseMs = std::chrono::duration<double, std::milli>(end - start).count();

    // Report to MemoryProfiler
    MemoryProfiler::instance().recordCollection(pauseMs, collected, collected);

    // Optional: print GC statistics in verbose mode
    // Uncomment for debugging:
    // if (collected > 0) {
    //     std::cerr << "[GC] Collected " << collected << " cyclic objects in "
    //               << pauseMs << "ms\n";
    // }
}

} // namespace stratos
