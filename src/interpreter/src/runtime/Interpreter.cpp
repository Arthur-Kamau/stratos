#include "stratos/Interpreter.h"
#include "stratos/Lexer.h"
#include "stratos/Parser.h"
#include "stratos/MemoryProfiler.h"
#include "stratos/HttpServer.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <thread>

namespace stratos {

Interpreter::Interpreter() : Interpreter("") {}

Interpreter::Interpreter(const std::string& root) : projectRoot(root) {
    // Reserve capacity to prevent vector reallocation (which would invalidate currentEnv pointer)
    // This ensures currentEnv remains valid throughout execution
    environments.reserve(1000);

    // Create global environment
    auto globalEnv = std::make_unique<Environment>();
    currentEnv = globalEnv.get();
    environments.push_back(std::move(globalEnv));

    // Note: Prelude functions (print, println, etc.) are available without 'use'
    // All other modules including 'maps' require explicit 'use' statement

    // Connect MemoryProfiler with GC
    MemoryProfiler::instance().setGarbageCollector(&gc);
}

Interpreter::~Interpreter() {
    // Ensure all threads are properly cleaned up
    cleanup();
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
    // Guard against double cleanup
    if (workerPools.empty() && goroutines.empty() && environments.size() <= 1) {
        return;
    }

    // First, shut down all worker pools
    for (auto& pool : workerPools) {
        {
            std::lock_guard<std::mutex> lock(pool->poolMutex);
            pool->closed = true;
        }
        pool->jobAvailable.notify_all();
        for (auto& worker : pool->workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    workerPools.clear();

    // Join all goroutines
    joinAllGoroutines();

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
                else if (left.type == "char") leftStr = std::string(1, left.asChar());

                std::string rightStr;
                if (right.type == "string") rightStr = right.asString();
                else if (right.type == "int") rightStr = std::to_string(right.asInt());
                else if (right.type == "double") rightStr = std::to_string(right.asDouble());
                else if (right.type == "bool") rightStr = right.asBool() ? "true" : "false";
                else if (right.type == "char") rightStr = std::string(1, right.asChar());

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

        case TokenType::BITWISE_AND:
            if (left.type == "int" && right.type == "int") {
                lastValue = RuntimeValue(std::any(left.asInt() & right.asInt()), "int");
            } else {
                error("Bitwise AND requires integer operands");
            }
            break;

        case TokenType::BITWISE_OR:
            if (left.type == "int" && right.type == "int") {
                lastValue = RuntimeValue(std::any(left.asInt() | right.asInt()), "int");
            } else {
                error("Bitwise OR requires integer operands");
            }
            break;

        case TokenType::BITWISE_XOR:
            if (left.type == "int" && right.type == "int") {
                lastValue = RuntimeValue(std::any(left.asInt() ^ right.asInt()), "int");
            } else {
                error("Bitwise XOR requires integer operands");
            }
            break;

        case TokenType::LEFT_SHIFT:
            if (left.type == "int" && right.type == "int") {
                lastValue = RuntimeValue(std::any(left.asInt() << right.asInt()), "int");
            } else {
                error("Left shift requires integer operands");
            }
            break;

        case TokenType::RIGHT_SHIFT:
            if (left.type == "int" && right.type == "int") {
                lastValue = RuntimeValue(std::any(left.asInt() >> right.asInt()), "int");
            } else {
                error("Right shift requires integer operands");
            }
            break;

        case TokenType::DOT_DOT:
            if (left.type == "int" && right.type == "int") {
                int start = left.asInt();
                int end = right.asInt();
                std::vector<int> range;
                // Generate range [start, end)
                if (start <= end) {
                    // Reserve to avoid reallocations
                    range.reserve(end - start);
                    for (int i = start; i < end; ++i) {
                        range.push_back(i);
                    }
                }
                lastValue = RuntimeValue(std::any(range), "array<int>");
            } else {
                error("Range operator requires integer operands");
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

        case TokenType::BITWISE_NOT:
            if (operand.type == "int") {
                lastValue = RuntimeValue(std::any(~operand.asInt()), "int");
            } else {
                error("Bitwise NOT requires integer operand");
            }
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
                lastValue = RuntimeValue(std::any(std::stoi(expr.value, nullptr, 0)), "int");
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
                                } else if (methodName == "push" || methodName == "add") {
                                    if (!args.empty()) {
                                        vec.push_back(args[0].asString());
                                        lastValue = RuntimeValue(std::any(), "void");
                                    } else {
                                        error(methodName + "() requires a value argument");
                                    }
                                    return;
                                } else if (methodName == "forEach") {
                                    if (args.empty()) error("forEach requires a callback");
                                    auto callback = args[0];
                                    for (const auto& item : vec) {
                                        std::vector<RuntimeValue> cbArgs = {RuntimeValue(item)};
                                        executeCallback(callback, cbArgs);
                                    }
                                    lastValue = RuntimeValue(std::any(), "void");
                                    return;
                                } else if (methodName == "map") {
                                    if (args.empty()) error("map requires a callback");
                                    auto callback = args[0];
                                    
                                    // We need to determine the result type based on the first element
                                    // This is a simplification; ideally we'd know the target type
                                    if (vec.empty()) {
                                        // Empty input -> empty output (preserve type or default to any?)
                                        // Defaulting to same type for empty
                                        lastValue = RuntimeValue(std::any(std::vector<std::string>()), "array<string>");
                                        return;
                                    }
                                    
                                    std::vector<RuntimeValue> results;
                                    for (const auto& item : vec) {
                                        std::vector<RuntimeValue> cbArgs = {RuntimeValue(item)};
                                        results.push_back(executeCallback(callback, cbArgs));
                                    }
                                    
                                    // Check result type of first element
                                    if (results[0].type == "int") {
                                        std::vector<int> resVec;
                                        for (const auto& r : results) resVec.push_back(r.asInt());
                                        lastValue = RuntimeValue(std::any(resVec), "array<int>");
                                    } else if (results[0].type == "string") {
                                        std::vector<std::string> resVec;
                                        for (const auto& r : results) resVec.push_back(r.asString());
                                        lastValue = RuntimeValue(std::any(resVec), "array<string>");
                                    } else {
                                        // Fallback or error
                                        error("Unsupported map result type: " + results[0].type);
                                    }
                                    return;
                                }
                            } catch (const std::bad_any_cast&) {
                                error("Internal error: Failed to cast array value");
                            }
                        } else if (leftValue.type == "array<int>") {
                            try {
                                auto& vec = std::any_cast<std::vector<int>&>(*anyPtr);

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
                                        lastValue = RuntimeValue(0); // or null/error?
                                    }
                                    return;
                                } else if (methodName == "last") {
                                    if (!vec.empty()) {
                                        lastValue = RuntimeValue(vec.back());
                                    } else {
                                        lastValue = RuntimeValue(0);
                                    }
                                    return;
                                } else if (methodName == "push" || methodName == "add") {
                                    if (!args.empty()) {
                                        vec.push_back(args[0].asInt());
                                        lastValue = RuntimeValue(std::any(), "void");
                                    } else {
                                        error(methodName + "() requires a value argument");
                                    }
                                    return;
                                } else if (methodName == "forEach") {
                                    if (args.empty()) error("forEach requires a callback");
                                    auto callback = args[0];
                                    for (int item : vec) {
                                        std::vector<RuntimeValue> cbArgs = {RuntimeValue(item)};
                                        executeCallback(callback, cbArgs);
                                    }
                                    lastValue = RuntimeValue(std::any(), "void");
                                    return;
                                } else if (methodName == "map") {
                                    if (args.empty()) error("map requires a callback");
                                    auto callback = args[0];
                                    
                                    if (vec.empty()) {
                                        lastValue = RuntimeValue(std::any(std::vector<int>()), "array<int>");
                                        return;
                                    }
                                    
                                    std::vector<RuntimeValue> results;
                                    for (int item : vec) {
                                        std::vector<RuntimeValue> cbArgs = {RuntimeValue(item)};
                                        results.push_back(executeCallback(callback, cbArgs));
                                    }
                                    
                                    if (results[0].type == "int") {
                                        std::vector<int> resVec;
                                        for (const auto& r : results) resVec.push_back(r.asInt());
                                        lastValue = RuntimeValue(std::any(resVec), "array<int>");
                                    } else if (results[0].type == "string") {
                                        std::vector<std::string> resVec;
                                        for (const auto& r : results) resVec.push_back(r.asString());
                                        lastValue = RuntimeValue(std::any(resVec), "array<string>");
                                    } else {
                                        error("Unsupported map result type: " + results[0].type);
                                    }
                                    return;
                                } else if (methodName == "contains") {
                                    if (!args.empty()) {
                                        int searchValue = args[0].asInt();
                                        bool found = std::find(vec.begin(), vec.end(), searchValue) != vec.end();
                                        lastValue = RuntimeValue(found);
                                    } else {
                                        error("contains() requires a value argument");
                                    }
                                    return;
                                } else if (methodName == "indexOf") {
                                    if (!args.empty()) {
                                        int searchValue = args[0].asInt();
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
                                    std::vector<int> reversed(vec.rbegin(), vec.rend());
                                    lastValue = RuntimeValue(std::any(reversed), "array<int>");
                                    return;
                                }
                            } catch (const std::bad_any_cast&) {
                                error("Internal error: Failed to cast int array value");
                            }
                        }

                        // Handle std::vector<std::any> (returned by maps.values() for generic maps)
                        try {
                            auto& vec = std::any_cast<std::vector<std::any>&>(*anyPtr);

                            if (methodName == "length") {
                                lastValue = RuntimeValue(static_cast<int>(vec.size()));
                                return;
                            } else if (methodName == "isEmpty") {
                                lastValue = RuntimeValue(vec.empty());
                                return;
                            } else if (methodName == "forEach") {
                                if (args.empty()) error("forEach requires a callback");
                                auto callback = args[0];
                                for (const auto& item : vec) {
                                    std::vector<RuntimeValue> cbArgs;
                                    // Convert std::any to RuntimeValue
                                    if (item.type() == typeid(int)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<int>(item)));
                                    } else if (item.type() == typeid(double)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<double>(item)));
                                    } else if (item.type() == typeid(std::string)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<std::string>(item)));
                                    } else if (item.type() == typeid(bool)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<bool>(item)));
                                    } else if (item.type() == typeid(std::shared_ptr<ClassInstance>)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<std::shared_ptr<ClassInstance>>(item)));
                                    } else {
                                        cbArgs.push_back(RuntimeValue(item, "any"));
                                    }
                                    executeCallback(callback, cbArgs);
                                }
                                lastValue = RuntimeValue(std::any(), "void");
                                return;
                            }
                        } catch (const std::bad_any_cast&) {
                            // Not a vector<any>, continue
                        }
                    }
                    error("Unknown array method: " + methodName);
                }

                // Handle map method calls (map.get(), map.set(), map.has(), etc.)
                if (leftValue.type.starts_with("map")) {
                    if (methodName == "forEach") {
                         if (args.empty()) error("forEach requires a callback");
                         auto callback = args[0];
                         
                         if (auto* anyPtr = std::get_if<std::any>(&leftValue.value)) {
                             bool handled = false;
                             try {
                                 auto& map = std::any_cast<std::unordered_map<std::string, std::any>&>(*anyPtr);
                                 for (const auto& pair : map) {
                                     std::vector<RuntimeValue> cbArgs;
                                     cbArgs.push_back(RuntimeValue(pair.first));
                                     
                                     // Handle std::any wrapping
                                     if (pair.second.type() == typeid(int)) cbArgs.push_back(RuntimeValue(std::any_cast<int>(pair.second)));
                                     else if (pair.second.type() == typeid(double)) cbArgs.push_back(RuntimeValue(std::any_cast<double>(pair.second)));
                                     else if (pair.second.type() == typeid(std::string)) cbArgs.push_back(RuntimeValue(std::any_cast<std::string>(pair.second)));
                                     else if (pair.second.type() == typeid(bool)) cbArgs.push_back(RuntimeValue(std::any_cast<bool>(pair.second)));
                                     else cbArgs.push_back(RuntimeValue(pair.second, "any"));
                                     
                                     executeCallback(callback, cbArgs);
                                 }
                                 handled = true;
                             } catch (...) {}

                             if (!handled) {
                                 try {
                                     auto& map = std::any_cast<std::unordered_map<std::string, std::string>&>(*anyPtr);
                                     for (const auto& pair : map) {
                                         std::vector<RuntimeValue> cbArgs;
                                         cbArgs.push_back(RuntimeValue(pair.first));
                                         cbArgs.push_back(RuntimeValue(pair.second));
                                         executeCallback(callback, cbArgs);
                                     }
                                     handled = true;
                                 } catch (...) {}
                             }
                             
                             if (!handled) {
                                 try {
                                     auto& map = std::any_cast<std::unordered_map<std::string, int>&>(*anyPtr);
                                     for (const auto& pair : map) {
                                         std::vector<RuntimeValue> cbArgs;
                                         cbArgs.push_back(RuntimeValue(pair.first));
                                         cbArgs.push_back(RuntimeValue(pair.second));
                                         executeCallback(callback, cbArgs);
                                     }
                                     handled = true;
                                 } catch (...) {}
                             }
                             
                             if (handled) {
                                 lastValue = RuntimeValue(std::any(), "void");
                                 return;
                             }
                         }
                    } else if (methodName == "map") {
                        if (args.empty()) error("map requires a callback");
                         auto callback = args[0];
                         
                         if (auto* anyPtr = std::get_if<std::any>(&leftValue.value)) {
                             std::vector<RuntimeValue> results;
                             bool handled = false;
                             
                             try {
                                 auto& map = std::any_cast<std::unordered_map<std::string, std::any>&>(*anyPtr);
                                 for (const auto& pair : map) {
                                     std::vector<RuntimeValue> cbArgs;
                                     cbArgs.push_back(RuntimeValue(pair.first));
                                     // Handle std::any wrapping
                                     if (pair.second.type() == typeid(int)) cbArgs.push_back(RuntimeValue(std::any_cast<int>(pair.second)));
                                     else if (pair.second.type() == typeid(double)) cbArgs.push_back(RuntimeValue(std::any_cast<double>(pair.second)));
                                     else if (pair.second.type() == typeid(std::string)) cbArgs.push_back(RuntimeValue(std::any_cast<std::string>(pair.second)));
                                     else if (pair.second.type() == typeid(bool)) cbArgs.push_back(RuntimeValue(std::any_cast<bool>(pair.second)));
                                     else cbArgs.push_back(RuntimeValue(pair.second, "any"));
                                     
                                     results.push_back(executeCallback(callback, cbArgs));
                                 }
                                 handled = true;
                             } catch (...) {}

                             if (!handled) {
                                 try {
                                     auto& map = std::any_cast<std::unordered_map<std::string, std::string>&>(*anyPtr);
                                     for (const auto& pair : map) {
                                         std::vector<RuntimeValue> cbArgs;
                                         cbArgs.push_back(RuntimeValue(pair.first));
                                         cbArgs.push_back(RuntimeValue(pair.second));
                                         results.push_back(executeCallback(callback, cbArgs));
                                     }
                                     handled = true;
                                 } catch (...) {}
                             }
                             
                             if (!handled) {
                                 try {
                                     auto& map = std::any_cast<std::unordered_map<std::string, int>&>(*anyPtr);
                                     for (const auto& pair : map) {
                                         std::vector<RuntimeValue> cbArgs;
                                         cbArgs.push_back(RuntimeValue(pair.first));
                                         cbArgs.push_back(RuntimeValue(pair.second));
                                         results.push_back(executeCallback(callback, cbArgs));
                                     }
                                     handled = true;
                                 } catch (...) {}
                             }
                             
                             if (handled) {
                                 if (results.empty()) {
                                     lastValue = RuntimeValue(std::any(std::vector<int>()), "array<int>"); // Default empty
                                 } else if (results[0].type == "int") {
                                      std::vector<int> resVec;
                                      for (const auto& r : results) resVec.push_back(r.asInt());
                                      lastValue = RuntimeValue(std::any(resVec), "array<int>");
                                 } else if (results[0].type == "string") {
                                      std::vector<std::string> resVec;
                                      for (const auto& r : results) resVec.push_back(r.asString());
                                      lastValue = RuntimeValue(std::any(resVec), "array<string>");
                                 } else {
                                     // Default to array<any> if mixed or unsupported
                                     // But std::any stores vector<T>, not vector<any> usually for primitives.
                                     // Support generic array<any> not fully implemented perhaps?
                                      error("Unsupported map result type: " + results[0].type);
                                 }
                                 return;
                             }
                         }
                    }

                    // entries() implementation
                     else if (methodName == "entries") {
                         if (auto* anyPtr = std::get_if<std::any>(&leftValue.value)) {
                             // std::cerr << "Debug: Map check. Type: " << anyPtr->type().name() << std::endl;
                             std::vector<RuntimeValue> results;
                             bool handled = false;
                             
                             auto createPair = [&](RuntimeValue k, RuntimeValue v) -> RuntimeValue {
                                 auto instance = std::make_shared<ClassInstance>();
                                 instance->className = "Pair";
                                 // Simple field assignment, assuming Pair structure
                                 instance->fields["first"] = k;
                                 instance->fields["second"] = v;
                                 // Ideally register with GC, but ignoring for brevity in this patch
                                 // gc.registerObject(instance); 
                                 return RuntimeValue(std::any(instance), "object");
                             };

                             try {
                                 auto& map = std::any_cast<std::unordered_map<std::string, std::any>&>(*anyPtr);
                                 for (const auto& pair : map) {
                                      RuntimeValue key(pair.first);
                                      RuntimeValue val;
                                      // Unwrap commonly used types
                                      if (pair.second.type() == typeid(int)) val = RuntimeValue(std::any_cast<int>(pair.second));
                                      else if (pair.second.type() == typeid(double)) val = RuntimeValue(std::any_cast<double>(pair.second));
                                      else if (pair.second.type() == typeid(std::string)) val = RuntimeValue(std::any_cast<std::string>(pair.second));
                                      else if (pair.second.type() == typeid(bool)) val = RuntimeValue(std::any_cast<bool>(pair.second));
                                      else val = RuntimeValue(pair.second, "any");
                                      
                                      results.push_back(createPair(key, val));
                                 }
                                 handled = true;
                             } catch (...) {}

                             if (!handled) {
                                 try {
                                     auto& map = std::any_cast<std::unordered_map<std::string, std::string>&>(*anyPtr);
                                     for (const auto& pair : map) {
                                         results.push_back(createPair(RuntimeValue(pair.first), RuntimeValue(pair.second)));
                                     }
                                     handled = true;
                                 } catch (...) {}
                             }

                             if (handled) {
                                 lastValue = RuntimeValue(std::any(results), "array<any>");
                                 return;
                             }
                         }
                    }

                     else if (methodName == "put") {
                         if (args.size() < 2) error("put() requires key and value arguments");
                         auto key = args[0];
                         auto val = args[1];

                         if (auto* anyPtr = std::get_if<std::any>(&leftValue.value)) {
                             bool handled = false;
                             std::any updatedMap;

                             try {
                                 // Try map<string, any>
                                 auto map = std::any_cast<std::unordered_map<std::string, std::any>>(*anyPtr);
                                 // Convert key to string if needed
                                 std::string keyStr;
                                 if (key.type == "string") {
                                     keyStr = key.asString();
                                 } else if (key.type == "int") {
                                     keyStr = std::to_string(key.asInt());
                                 } else {
                                     keyStr = key.asString();
                                 }
                                 // Convert RuntimeValue to std::any for storage
                                 std::any anyValue;
                                 if (val.type == "int") {
                                     anyValue = val.asInt();
                                 } else if (val.type == "double") {
                                     anyValue = val.asDouble();
                                 } else if (val.type == "string") {
                                     anyValue = val.asString();
                                 } else if (val.type == "bool") {
                                     anyValue = val.asBool();
                                 } else if (val.type == "object") {
                                     anyValue = val.asObject();
                                 } else if (std::holds_alternative<std::any>(val.value)) {
                                     anyValue = std::get<std::any>(val.value);
                                 }
                                 map[keyStr] = anyValue;
                                 updatedMap = map;
                                 handled = true;
                             } catch (...) {
                                 // Try map<string, string>
                                 try {
                                     auto map = std::any_cast<std::unordered_map<std::string, std::string>>(*anyPtr);
                                     map[key.asString()] = val.asString();
                                     updatedMap = map;
                                     handled = true;
                                 } catch(...) {
                                      // Try map<string, int>
                                      try {
                                         auto map = std::any_cast<std::unordered_map<std::string, int>>(*anyPtr);
                                         map[key.asString()] = val.asInt();
                                         updatedMap = map;
                                         handled = true;
                                      } catch(...) {}
                                 }
                             }

                             if (!handled) {
                                 error("Failed to put into map: incompatible types");
                             }

                             // Update the original variable if the left side was a variable
                             if (auto* leftVarExpr = dynamic_cast<VariableExpr*>(binExpr->left.get())) {
                                 // Get a reference to the original variable and update it in place
                                 try {
                                     RuntimeValue& originalVar = currentEnv->getRef(leftVarExpr->name.lexeme);
                                     originalVar = RuntimeValue(updatedMap, leftValue.type);
                                 } catch (...) {
                                     // Fallback to assign if getRef fails
                                     currentEnv->assign(leftVarExpr->name.lexeme, RuntimeValue(updatedMap, leftValue.type));
                                 }
                             }

                             lastValue = RuntimeValue(std::any(), "void");
                             return;
                         }
                     }

                    // Map methods forward to the maps module with map as first argument
                    args.insert(args.begin(), leftValue);
                    lastValue = evaluateNativeCall("maps", methodName, args);
                    return;
                }

                // Handle List, Set, Queue, Stack method calls
                if (leftValue.type.starts_with("List<") || leftValue.type.starts_with("Set<") ||
                    leftValue.type.starts_with("Queue<") || leftValue.type.starts_with("Stack<")) {
                    if (auto* anyPtr = std::get_if<std::any>(&leftValue.value)) {
                        // All these collections are backed by std::vector<std::any>
                        try {
                            auto& vec = std::any_cast<std::vector<std::any>&>(*anyPtr);

                            if (methodName == "forEach") {
                                if (args.empty()) error("forEach requires a callback");
                                auto callback = args[0];
                                for (const auto& item : vec) {
                                    std::vector<RuntimeValue> cbArgs;
                                    // Convert std::any to RuntimeValue
                                    if (item.type() == typeid(int)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<int>(item)));
                                    } else if (item.type() == typeid(double)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<double>(item)));
                                    } else if (item.type() == typeid(std::string)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<std::string>(item)));
                                    } else if (item.type() == typeid(bool)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<bool>(item)));
                                    } else {
                                        cbArgs.push_back(RuntimeValue(item, "any"));
                                    }
                                    executeCallback(callback, cbArgs);
                                }
                                lastValue = RuntimeValue(std::any(), "void");
                                return;
                            } else if (methodName == "map") {
                                if (args.empty()) error("map requires a callback");
                                auto callback = args[0];

                                if (vec.empty()) {
                                    lastValue = RuntimeValue(std::any(std::vector<std::any>()), leftValue.type);
                                    return;
                                }

                                std::vector<RuntimeValue> results;
                                for (const auto& item : vec) {
                                    std::vector<RuntimeValue> cbArgs;
                                    if (item.type() == typeid(int)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<int>(item)));
                                    } else if (item.type() == typeid(double)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<double>(item)));
                                    } else if (item.type() == typeid(std::string)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<std::string>(item)));
                                    } else if (item.type() == typeid(bool)) {
                                        cbArgs.push_back(RuntimeValue(std::any_cast<bool>(item)));
                                    } else {
                                        cbArgs.push_back(RuntimeValue(item, "any"));
                                    }
                                    results.push_back(executeCallback(callback, cbArgs));
                                }

                                // Convert results to appropriate type
                                if (results[0].type == "int") {
                                    std::vector<int> resVec;
                                    for (const auto& r : results) resVec.push_back(r.asInt());
                                    lastValue = RuntimeValue(std::any(resVec), "array<int>");
                                } else if (results[0].type == "string") {
                                    std::vector<std::string> resVec;
                                    for (const auto& r : results) resVec.push_back(r.asString());
                                    lastValue = RuntimeValue(std::any(resVec), "array<string>");
                                } else {
                                    error("Unsupported map result type: " + results[0].type);
                                }
                                return;
                            } else if (methodName == "length") {
                                lastValue = RuntimeValue(static_cast<int>(vec.size()));
                                return;
                            }
                        } catch (const std::bad_any_cast&) {
                            error("Internal error: Failed to cast collection value");
                        }
                    }
                    error("Unknown method '" + methodName + "' for collection type: " + leftValue.type);
                }

                // Handle object method calls
                if (leftValue.type == "object") {
                    auto instance = leftValue.asObject();

                    // Handle Result type methods (io::Result or Result)
                    if (instance->className == "io::Result" || instance->className == "Result") {
                        if (methodName == "ok") {
                            // Return the isOk flag
                            lastValue = instance->fields["isOk"];
                            return;
                        } else if (methodName == "err") {
                            // Return the error object
                            lastValue = instance->fields["error"];
                            return;
                        } else if (methodName == "unwrap") {
                            // Return value if ok, otherwise throw error
                            bool isOk = instance->fields["isOk"].asBool();
                            if (isOk) {
                                lastValue = instance->fields["value"];
                            } else {
                                auto errorObj = instance->fields["error"].asObject();
                                std::string errMsg = errorObj->fields["message"].asString();
                                error("unwrap() called on Err result: " + errMsg);
                            }
                            return;
                        }
                    }

                    // Handle Optional type methods
                    if (instance->className == "Optional" || instance->className.find("Optional<") == 0) {
                        if (methodName == "isSome") {
                            lastValue = instance->fields["hasValue"];
                            return;
                        } else if (methodName == "isNone") {
                            bool hasValue = instance->fields["hasValue"].asBool();
                            lastValue = RuntimeValue(!hasValue);
                            return;
                        } else if (methodName == "unwrap") {
                            bool hasValue = instance->fields["hasValue"].asBool();
                            if (hasValue) {
                                lastValue = instance->fields["value"];
                            } else {
                                error("unwrap() called on None optional");
                            }
                            return;
                        } else if (methodName == "unwrapOr") {
                            bool hasValue = instance->fields["hasValue"].asBool();
                            if (hasValue) {
                                lastValue = instance->fields["value"];
                            } else if (!args.empty()) {
                                lastValue = args[0];
                            } else {
                                error("unwrapOr() requires a default value argument");
                            }
                            return;
                        }
                    }

                    // Handle WaitGroup type methods
                    if (instance->className == "WaitGroup") {
                        if (methodName == "add") {
                            int delta = args.empty() ? 1 : args[0].asInt();
                            int current = instance->fields["count"].asInt();
                            instance->fields["count"] = RuntimeValue(current + delta);
                            lastValue = RuntimeValue();
                            return;
                        } else if (methodName == "done") {
                            int current = instance->fields["count"].asInt();
                            instance->fields["count"] = RuntimeValue(current - 1);
                            lastValue = RuntimeValue();
                            return;
                        } else if (methodName == "wait") {
                            // Busy wait until count reaches 0
                            while (instance->fields["count"].asInt() > 0) {
                                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                            }
                            lastValue = RuntimeValue();
                            return;
                        }
                    }

                    // Handle Mutex type methods
                    if (instance->className == "Mutex") {
                        if (methodName == "lock") {
                            // Simple spinlock - wait until unlocked
                            while (instance->fields["locked"].asBool()) {
                                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                            }
                            instance->fields["locked"] = RuntimeValue(true);
                            lastValue = RuntimeValue();
                            return;
                        } else if (methodName == "unlock") {
                            instance->fields["locked"] = RuntimeValue(false);
                            lastValue = RuntimeValue();
                            return;
                        } else if (methodName == "tryLock") {
                            bool wasLocked = instance->fields["locked"].asBool();
                            if (!wasLocked) {
                                instance->fields["locked"] = RuntimeValue(true);
                            }
                            lastValue = RuntimeValue(!wasLocked);
                            return;
                        }
                    }

                    // Handle Channel type methods
                    if (instance->className == "Channel") {
                        if (methodName == "send") {
                            // Store value in channel's internal queue
                            if (args.size() > 0) {
                                // Get or create the queue
                                if (instance->fields.find("__queue") == instance->fields.end()) {
                                    std::vector<RuntimeValue> emptyQueue;
                                    instance->fields["__queue"] = RuntimeValue(std::any(emptyQueue), "array<any>");
                                }
                                auto& queueVal = instance->fields["__queue"];
                                if (auto* anyPtr = std::get_if<std::any>(&queueVal.value)) {
                                    auto queue = std::any_cast<std::vector<RuntimeValue>>(*anyPtr);
                                    queue.push_back(args[0]);
                                    instance->fields["__queue"] = RuntimeValue(std::any(queue), "array<any>");
                                }
                            }
                            lastValue = RuntimeValue(true);
                            return;
                        } else if (methodName == "receive") {
                            // Get value from channel's internal queue
                            if (instance->fields.find("__queue") != instance->fields.end()) {
                                auto& queueVal = instance->fields["__queue"];
                                if (auto* anyPtr = std::get_if<std::any>(&queueVal.value)) {
                                    auto queue = std::any_cast<std::vector<RuntimeValue>>(*anyPtr);
                                    if (!queue.empty()) {
                                        RuntimeValue value = queue.front();
                                        queue.erase(queue.begin());
                                        instance->fields["__queue"] = RuntimeValue(std::any(queue), "array<any>");
                                        // Return the value directly (not wrapped in Optional for simplicity)
                                        lastValue = value;
                                        return;
                                    }
                                }
                            }
                            // Return empty/default value if queue is empty
                            lastValue = RuntimeValue();
                            return;
                        } else if (methodName == "close") {
                            instance->fields["closed"] = RuntimeValue(true);
                            lastValue = RuntimeValue();
                            return;
                        } else if (methodName == "isClosed") {
                            if (instance->fields.find("closed") != instance->fields.end()) {
                                lastValue = instance->fields["closed"];
                            } else {
                                lastValue = RuntimeValue(false);
                            }
                            return;
                        }
                    }

                    // Handle WorkerPool type methods
                    if (instance->className == "WorkerPool") {
                        if (methodName == "submit") {
                            // Submit a job to the worker pool
                            if (!args.empty()) {
                                int poolId = instance->fields["__poolId"].asInt();
                                if (poolId >= 0 && poolId < static_cast<int>(workerPools.size())) {
                                    auto& pool = workerPools[poolId];
                                    {
                                        std::lock_guard<std::mutex> lock(pool->poolMutex);
                                        pool->jobQueue.push(args[0]);
                                    }
                                    pool->jobAvailable.notify_one();
                                }
                            }
                            lastValue = RuntimeValue();
                            return;
                        } else if (methodName == "close") {
                            // Close the worker pool
                            int poolId = instance->fields["__poolId"].asInt();
                            if (poolId >= 0 && poolId < static_cast<int>(workerPools.size())) {
                                auto& pool = workerPools[poolId];
                                {
                                    std::lock_guard<std::mutex> lock(pool->poolMutex);
                                    pool->closed = true;
                                }
                                pool->jobAvailable.notify_all();

                                // Wait for all workers to finish
                                for (auto& worker : pool->workers) {
                                    if (worker.joinable()) {
                                        worker.join();
                                    }
                                }

                                // Close the results channel
                                pool->resultsChannel->fields["closed"] = RuntimeValue(true);
                            }
                            lastValue = RuntimeValue();
                            return;
                        } else if (methodName == "results") {
                            // Return the results channel
                            int poolId = instance->fields["__poolId"].asInt();
                            if (poolId >= 0 && poolId < static_cast<int>(workerPools.size())) {
                                lastValue = RuntimeValue(workerPools[poolId]->resultsChannel);
                                return;
                            }
                        }
                    }

                    // Handle Router type methods
                    if (instance->className == "Router") {
                        auto& mgr = HttpServerManager::getInstance();
                        int routerId = instance->fields["__routerId"].asInt();

                        if (methodName == "get" || methodName == "post" || methodName == "put" ||
                            methodName == "delete" || methodName == "patch") {
                            if (args.size() >= 2) {
                                std::string pattern = args[0].asString();
                                RuntimeValue& handlerVal = args[1];

                                // Store the handler and get an ID
                                int handlerId = mgr.storeHandler(handlerVal);

                                // Determine HTTP method
                                HttpMethod httpMethod = HttpMethod::GET;
                                if (methodName == "post") httpMethod = HttpMethod::POST;
                                else if (methodName == "put") httpMethod = HttpMethod::PUT;
                                else if (methodName == "delete") httpMethod = HttpMethod::DELETE_;
                                else if (methodName == "patch") httpMethod = HttpMethod::PATCH;

                                // Add route
                                mgr.addRoute(routerId, httpMethod, pattern, handlerId);
                            }
                            lastValue = RuntimeValue(instance);  // Return self for chaining
                            return;
                        } else if (methodName == "use") {
                            // Middleware
                            if (!args.empty()) {
                                int handlerId = mgr.storeHandler(args[0]);
                                mgr.addMiddleware(routerId, handlerId);
                            }
                            lastValue = RuntimeValue(instance);
                            return;
                        } else if (methodName == "group") {
                            // Router group with prefix
                            if (!args.empty()) {
                                std::string prefix = args[0].asString();
                                int newRouterId = mgr.createRouterGroup(routerId, prefix);

                                auto newRouter = std::make_shared<ClassInstance>();
                                newRouter->className = "Router";
                                newRouter->fields["__routerId"] = RuntimeValue(newRouterId);

                                lastValue = RuntimeValue(newRouter);
                                return;
                            }
                        }
                    }

                    // Handle Server type methods
                    if (instance->className == "Server") {
                        auto& mgr = HttpServerManager::getInstance();
                        int serverId = instance->fields["__serverId"].asInt();

                        if (methodName == "listen") {
                            int port = 8080;  // Default port
                            if (!args.empty()) {
                                if (args[0].type == "int") {
                                    port = args[0].asInt();
                                } else if (args[0].type == "double") {
                                    port = static_cast<int>(args[0].asDouble());
                                }
                            }

                            std::cout << "Server listening on port " << port << std::endl;

                            // Start server (this will block in current implementation)
                            bool success = mgr.startServer(serverId, port, this);
                            lastValue = RuntimeValue(success);
                            return;
                        } else if (methodName == "stop") {
                            mgr.stopServer(serverId);
                            lastValue = RuntimeValue();
                            return;
                        }
                    }

                    // Handle Response type methods
                    if (instance->className == "Response") {
                        if (methodName == "status") {
                            if (!args.empty()) {
                                instance->fields["statusCode"] = args[0];
                            }
                            lastValue = RuntimeValue(instance);  // Return self for chaining
                            return;
                        } else if (methodName == "json") {
                            if (!args.empty()) {
                                // Serialize the argument to JSON string
                                std::string jsonStr;
                                extern std::string serializeJsonValue(const std::shared_ptr<ClassInstance>&);
                                extern std::string serializeRuntimeValue(const RuntimeValue&);

                                // Try to get an object (ClassInstance) from various sources
                                std::shared_ptr<ClassInstance> obj = nullptr;

                                if (args[0].type == "object") {
                                    obj = args[0].asObject();
                                } else if (args[0].type == "any" || args[0].type.starts_with("any")) {
                                    // The value might be stored as std::any containing ClassInstance
                                    if (auto* anyPtr = std::get_if<std::any>(&args[0].value)) {
                                        if (anyPtr->type() == typeid(std::shared_ptr<ClassInstance>)) {
                                            obj = std::any_cast<std::shared_ptr<ClassInstance>>(*anyPtr);
                                        }
                                    }
                                }

                                if (obj) {
                                    if (obj->className == "JsonValue") {
                                        jsonStr = serializeJsonValue(obj);
                                    } else {
                                        jsonStr = serializeRuntimeValue(args[0]);
                                    }
                                } else if (args[0].type == "string") {
                                    jsonStr = args[0].asString();
                                } else {
                                    jsonStr = serializeRuntimeValue(args[0]);
                                }
                                instance->fields["body"] = RuntimeValue(jsonStr);
                                // Set content type
                                if (auto* anyPtr = std::get_if<std::any>(&instance->fields["headers"].value)) {
                                    try {
                                        auto headers = std::any_cast<std::unordered_map<std::string, std::any>>(*anyPtr);
                                        headers["Content-Type"] = std::string("application/json");
                                        instance->fields["headers"] = RuntimeValue(std::any(headers), "map<string, string>");
                                    } catch (const std::bad_any_cast&) {
                                        // Headers not in expected format, create new
                                        std::unordered_map<std::string, std::any> headers;
                                        headers["Content-Type"] = std::string("application/json");
                                        instance->fields["headers"] = RuntimeValue(std::any(headers), "map<string, string>");
                                    }
                                }
                            }
                            lastValue = RuntimeValue(instance);
                            return;
                        } else if (methodName == "text") {
                            if (!args.empty()) {
                                instance->fields["body"] = args[0];
                            }
                            lastValue = RuntimeValue(instance);
                            return;
                        } else if (methodName == "html") {
                            if (!args.empty()) {
                                instance->fields["body"] = args[0];
                                if (auto* anyPtr = std::get_if<std::any>(&instance->fields["headers"].value)) {
                                    auto headers = std::any_cast<std::unordered_map<std::string, std::any>>(*anyPtr);
                                    headers["Content-Type"] = std::string("text/html");
                                    instance->fields["headers"] = RuntimeValue(std::any(headers), "map<string, string>");
                                }
                            }
                            lastValue = RuntimeValue(instance);
                            return;
                        } else if (methodName == "setHeader") {
                            if (args.size() >= 2) {
                                std::string key = args[0].asString();
                                std::string value = args[1].asString();
                                if (auto* anyPtr = std::get_if<std::any>(&instance->fields["headers"].value)) {
                                    auto headers = std::any_cast<std::unordered_map<std::string, std::any>>(*anyPtr);
                                    headers[key] = value;
                                    instance->fields["headers"] = RuntimeValue(std::any(headers), "map<string, string>");
                                }
                            }
                            lastValue = RuntimeValue(instance);
                            return;
                        }
                    }

                    // Handle Request type methods
                    if (instance->className == "Request") {
                        if (methodName == "json") {
                            // Parse the request body as JSON and return Result<JsonValue, Error>
                            std::string body = "";
                            if (instance->fields.count("body")) {
                                body = instance->fields["body"].asString();
                            }

                            // Create a Result object
                            auto result = std::make_shared<ClassInstance>();
                            result->className = "Result";

                            if (body.empty()) {
                                // Return error result
                                result->fields["isOk"] = RuntimeValue(false);
                                auto errorObj = std::make_shared<ClassInstance>();
                                errorObj->fields["message"] = RuntimeValue(std::string("Empty request body"));
                                result->fields["error"] = RuntimeValue(errorObj);
                            } else {
                                // Try to parse JSON using SimpleJsonParser
                                try {
                                    // Use the json module's parse function
                                    auto& registry = NativeRegistry::getInstance();
                                    std::vector<std::any> parseArgs = {body};
                                    auto parseResult = registry.getFunction("json", "jsonParse")(parseArgs);

                                    result->fields["isOk"] = RuntimeValue(true);
                                    result->fields["value"] = RuntimeValue(parseResult, "any");
                                    result->fields["error"] = RuntimeValue(std::make_shared<ClassInstance>());
                                } catch (const std::exception& e) {
                                    result->fields["isOk"] = RuntimeValue(false);
                                    auto errorObj = std::make_shared<ClassInstance>();
                                    errorObj->fields["message"] = RuntimeValue(std::string(e.what()));
                                    result->fields["error"] = RuntimeValue(errorObj);
                                }
                            }

                            lastValue = RuntimeValue(result);
                            return;
                        } else if (methodName == "getHeader") {
                            if (!args.empty()) {
                                std::string key = args[0].asString();
                                if (auto* anyPtr = std::get_if<std::any>(&instance->fields["headers"].value)) {
                                    auto headers = std::any_cast<std::unordered_map<std::string, std::any>>(*anyPtr);
                                    if (headers.count(key)) {
                                        lastValue = RuntimeValue(std::any_cast<std::string>(headers[key]));
                                        return;
                                    }
                                }
                            }
                            lastValue = RuntimeValue(std::string(""));
                            return;
                        } else if (methodName == "getParam" || methodName == "param") {
                            if (!args.empty()) {
                                std::string key = args[0].asString();
                                if (auto* anyPtr = std::get_if<std::any>(&instance->fields["params"].value)) {
                                    try {
                                        auto params = std::any_cast<std::unordered_map<std::string, std::any>>(*anyPtr);
                                        if (params.count(key)) {
                                            if (params[key].type() == typeid(std::string)) {
                                                lastValue = RuntimeValue(std::any_cast<std::string>(params[key]));
                                            } else {
                                                lastValue = RuntimeValue(std::string(""));
                                            }
                                            return;
                                        }
                                    } catch (...) {}
                                }
                                // Also check pathParams field
                                if (auto* anyPtr = std::get_if<std::any>(&instance->fields["pathParams"].value)) {
                                    try {
                                        auto params = std::any_cast<std::unordered_map<std::string, std::any>>(*anyPtr);
                                        if (params.count(key)) {
                                            if (params[key].type() == typeid(std::string)) {
                                                lastValue = RuntimeValue(std::any_cast<std::string>(params[key]));
                                            } else {
                                                lastValue = RuntimeValue(std::string(""));
                                            }
                                            return;
                                        }
                                    } catch (...) {}
                                }
                            }
                            lastValue = RuntimeValue(std::string(""));
                            return;
                        } else if (methodName == "getQuery") {
                            if (!args.empty()) {
                                std::string key = args[0].asString();
                                if (auto* anyPtr = std::get_if<std::any>(&instance->fields["queryParams"].value)) {
                                    auto params = std::any_cast<std::unordered_map<std::string, std::any>>(*anyPtr);
                                    if (params.count(key)) {
                                        lastValue = RuntimeValue(std::any_cast<std::string>(params[key]));
                                        return;
                                    }
                                }
                            }
                            lastValue = RuntimeValue(std::string(""));
                            return;
                        }
                    }

                    // Handle Time type methods
                    if (instance->className == "Time") {
                        if (methodName == "unix") {
                            // Return timestamp in seconds
                            if (instance->fields.count("timestamp")) {
                                int secs = instance->fields["timestamp"].asInt();
                                lastValue = RuntimeValue(secs);
                            } else {
                                lastValue = RuntimeValue(0);
                            }
                            return;
                        }
                    }

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
                                        for (size_t i = 0; i < funcDecl->parameters.size(); ++i) {
                                            const auto& param = funcDecl->parameters[i];
                                            
                                            if (param.isVariadic) {
                                                std::vector<RuntimeValue> varArgs;
                                                for (size_t j = i; j < args.size(); ++j) {
                                                    varArgs.push_back(args[j]);
                                                }
                                                currentEnv->define(param.name.lexeme, RuntimeValue(std::any(varArgs), "array<any>"));
                                                break;
                                            } else if (i < args.size()) {
                                                currentEnv->define(param.name.lexeme, args[i]);
                                            } else if (param.defaultValue) {
                                                param.defaultValue->accept(*this);
                                                currentEnv->define(param.name.lexeme, lastValue);
                                            } else {
                                                error("Missing argument for method parameter: " + param.name.lexeme);
                                            }
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

                        // Special handling for concurrent module functions that need interpreter access
                        if (moduleName == "concurrent") {
                            if (methodName == "go") {
                                // concurrent.go(closure) - spawn a goroutine
                                if (!args.empty() && args[0].type == "function") {
                                    spawnGoroutine(args[0]);
                                    lastValue = RuntimeValue();
                                    return;
                                }
                                error("concurrent.go() requires a function argument");
                                return;
                            } else if (methodName == "newWorkerPool") {
                                // concurrent.newWorkerPool(numWorkers, workerFunc)
                                if (args.size() >= 2 && args[0].type == "int" && args[1].type == "function") {
                                    lastValue = createWorkerPool(args[0].asInt(), args[1]);
                                    return;
                                }
                                error("concurrent.newWorkerPool() requires (int numWorkers, function workerFunc)");
                                return;
                            } else if (methodName == "pipeline") {
                                // concurrent.pipeline(inputChannel, [stage1, stage2, ...])
                                if (args.size() >= 2) {
                                    std::vector<RuntimeValue> stages;
                                    // Extract stages from array argument
                                    if (auto* anyPtr = std::get_if<std::any>(&args[1].value)) {
                                        try {
                                            auto stageVec = std::any_cast<std::vector<RuntimeValue>>(*anyPtr);
                                            stages = stageVec;
                                        } catch (...) {
                                            // Try as vector of closures stored differently
                                        }
                                    }
                                    if (!stages.empty()) {
                                        lastValue = createPipeline(args[0], stages);
                                        return;
                                    }
                                }
                                error("concurrent.pipeline() requires (channel input, array stages)");
                                return;
                            }
                        }

                        // FIRST: Check if it's a native function
                        auto& registry = NativeRegistry::getInstance();
                        if (registry.isNative(moduleName, methodName)) {
                            lastValue = evaluateNativeCall(moduleName, methodName, args);
                            return;
                        }

                        // SECOND: Check if it's a user-defined module function
                        if (moduleFunctions.count(moduleName) &&
                            moduleFunctions.at(moduleName).count(methodName)) {
                            FunctionDecl& funcDeclRef = moduleFunctions.at(moduleName).at(methodName).get();
                            lastValue = callModuleFunction(moduleName, methodName, args, &funcDeclRef);
                            return;
                        }

                        // Function not found
                        error("Undefined function: " + moduleName + "::" + methodName);
                        return;
                    }
                }

                // Handle static type method calls (Result.err, Result.ok, Optional.some, Optional.none)
                if (auto* leftVar = dynamic_cast<VariableExpr*>(binExpr->left.get())) {
                    std::string typeName = leftVar->name.lexeme;

                    // Result type static methods
                    if (typeName == "Result") {
                        if (methodName == "err") {
                            // Result.err(message) - create error result
                            auto result = std::make_shared<ClassInstance>();
                            result->className = "Result";
                            result->fields["value"] = RuntimeValue(std::any(), "any");
                            result->fields["isOk"] = RuntimeValue(false);

                            auto errorObj = std::make_shared<ClassInstance>();
                            errorObj->className = "Error";
                            if (!args.empty()) {
                                errorObj->fields["message"] = args[0];
                            } else {
                                errorObj->fields["message"] = RuntimeValue(std::string("Unknown error"));
                            }
                            result->fields["error"] = RuntimeValue(errorObj);

                            lastValue = RuntimeValue(result);
                            return;
                        } else if (methodName == "ok") {
                            // Result.ok(value) - create success result
                            auto result = std::make_shared<ClassInstance>();
                            result->className = "Result";
                            if (!args.empty()) {
                                result->fields["value"] = args[0];
                            } else {
                                result->fields["value"] = RuntimeValue(std::any(), "any");
                            }
                            result->fields["isOk"] = RuntimeValue(true);

                            auto errorObj = std::make_shared<ClassInstance>();
                            errorObj->className = "Error";
                            errorObj->fields["message"] = RuntimeValue(std::string(""));
                            result->fields["error"] = RuntimeValue(errorObj);

                            lastValue = RuntimeValue(result);
                            return;
                        }
                    }

                    // Optional type static methods
                    if (typeName == "Optional") {
                        if (methodName == "some") {
                            // Optional.some(value) - create Some optional
                            auto optional = std::make_shared<ClassInstance>();
                            optional->className = "Optional";
                            optional->fields["hasValue"] = RuntimeValue(true);
                            if (!args.empty()) {
                                optional->fields["value"] = args[0];
                            } else {
                                optional->fields["value"] = RuntimeValue(std::any(), "any");
                            }
                            lastValue = RuntimeValue(optional);
                            return;
                        } else if (methodName == "none") {
                            // Optional.none() - create None optional
                            auto optional = std::make_shared<ClassInstance>();
                            optional->className = "Optional";
                            optional->fields["hasValue"] = RuntimeValue(false);
                            optional->fields["value"] = RuntimeValue(std::any(), "any");
                            lastValue = RuntimeValue(optional);
                            return;
                        }
                    }
                }
            }
        }
    }

    // Check for closure/first-class function call
    bool isClosure = false;
    RuntimeValue closureValue;
    
    // Try to evaluate callee to see if it's a function variable
    try {
        // We only try this if it's NOT a dot expression (which is handled above)
        if (!dynamic_cast<BinaryExpr*>(expr.callee.get())) {
            expr.callee->accept(*this);
            closureValue = lastValue;
            if (closureValue.type == "function") {
                isClosure = true;
            }
        }
    } catch (...) {
        // Ignore error, might be a direct function call to a global/native function
    }

    if (isClosure) {
        // Evaluate arguments
        std::vector<RuntimeValue> args;
        for (const auto& arg : expr.arguments) {
            arg->accept(*this);
            args.push_back(lastValue);
        }

        lastValue = executeCallback(closureValue, args);
        return;
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

        // Check if this is a native function in the current executing module first
        if (!currentExecutingModule.empty() && registry.isNative(currentExecutingModule, functionName)) {
            lastValue = evaluateNativeCall(currentExecutingModule, functionName, args);
            return;
        }

        // If we're executing within a module, check if the function exists in that module
        if (!currentExecutingModule.empty() &&
            moduleFunctions.count(currentExecutingModule) &&
            moduleFunctions.at(currentExecutingModule).count(functionName)) {
            FunctionDecl& funcDeclRef = moduleFunctions.at(currentExecutingModule).at(functionName).get();
            lastValue = callModuleFunction(currentExecutingModule, functionName, args, &funcDeclRef);
            return;
        }

        // If we're loading a module, check if the function exists in that module
        // (for module initialization code like `var x = myFunction()`)
        if (!currentModuleName.empty() &&
            moduleFunctions.count(currentModuleName) &&
            moduleFunctions.at(currentModuleName).count(functionName)) {
            FunctionDecl& funcDeclRef = moduleFunctions.at(currentModuleName).at(functionName).get();
            lastValue = callModuleFunction(currentModuleName, functionName, args, &funcDeclRef);
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
            } else if (arrayValue.type == "array<int>") {
                try {
                    auto& vec = std::any_cast<std::vector<int>&>(*anyPtr);
                    if (idx >= 0 && idx < static_cast<int>(vec.size())) {
                        lastValue = RuntimeValue(vec[idx]);
                        return;
                    } else {
                        error("Array index out of bounds: " + std::to_string(idx));
                    }
                } catch (const std::bad_any_cast&) {
                    error("Internal error: Failed to cast int array value");
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

void Interpreter::visit(CastExpr& expr) {
    expr.expression->accept(*this); // Evaluate the expression to be cast
    RuntimeValue sourceValue = lastValue; // The value we want to cast

    TokenType targetType = expr.typeToken.type; // The type we want to cast to

    try {
        switch (targetType) {
            case TokenType::INT: {
                if (sourceValue.type == "int") {
                    lastValue = sourceValue; // Already an int
                } else if (sourceValue.type == "double") {
                    lastValue = RuntimeValue(static_cast<int>(sourceValue.asDouble())); // Truncation
                } else if (sourceValue.type == "bool") {
                    lastValue = RuntimeValue(static_cast<int>(sourceValue.asBool())); // true -> 1, false -> 0
                } else if (sourceValue.type == "string") {
                    try {
                        lastValue = RuntimeValue(std::stoi(sourceValue.asString()));
                    } catch (...) {
                        error("Cannot cast string '" + sourceValue.asString() + "' to int.");
                    }
                } else if (sourceValue.type == "char") {
                    lastValue = RuntimeValue(static_cast<int>(sourceValue.asChar()));
                }
                else {
                    error("Cannot cast " + sourceValue.type + " to int.");
                }
                break;
            }
            case TokenType::DOUBLE: {
                if (sourceValue.type == "double") {
                    lastValue = sourceValue; // Already a double
                } else if (sourceValue.type == "int") {
                    lastValue = RuntimeValue(static_cast<double>(sourceValue.asInt())); // Promotion
                } else if (sourceValue.type == "bool") {
                    lastValue = RuntimeValue(static_cast<double>(sourceValue.asBool())); // true -> 1.0, false -> 0.0
                } else if (sourceValue.type == "string") {
                    try {
                        lastValue = RuntimeValue(std::stod(sourceValue.asString()));
                    } catch (...) {
                        error("Cannot cast string '" + sourceValue.asString() + "' to double.");
                    }
                } else if (sourceValue.type == "char") {
                     lastValue = RuntimeValue(static_cast<double>(sourceValue.asChar()));
                }
                else {
                    error("Cannot cast " + sourceValue.type + " to double.");
                }
                break;
            }
            case TokenType::STRING: {
                if (sourceValue.type == "string") {
                    lastValue = sourceValue; // Already a string
                } else if (sourceValue.type == "int") {
                    lastValue = RuntimeValue(std::to_string(sourceValue.asInt()));
                } else if (sourceValue.type == "double") {
                    lastValue = RuntimeValue(std::to_string(sourceValue.asDouble()));
                } else if (sourceValue.type == "bool") {
                    lastValue = RuntimeValue(sourceValue.asBool() ? "true" : "false");
                } else if (sourceValue.type == "char") {
                    lastValue = RuntimeValue(std::string(1, sourceValue.asChar()));
                }
                else {
                    error("Cannot cast " + sourceValue.type + " to string.");
                }
                break;
            }
            case TokenType::BOOL: {
                if (sourceValue.type == "bool") {
                    lastValue = sourceValue; // Already a bool
                } else if (sourceValue.type == "int") {
                    lastValue = RuntimeValue(sourceValue.asInt() != 0);
                } else if (sourceValue.type == "double") {
                    lastValue = RuntimeValue(sourceValue.asDouble() != 0.0);
                } else if (sourceValue.type == "string") {
                    lastValue = RuntimeValue(!sourceValue.asString().empty()); // Non-empty string is true
                }
                else {
                    error("Cannot cast " + sourceValue.type + " to bool.");
                }
                break;
            }
            case TokenType::CHAR: {
                if (sourceValue.type == "char") {
                    lastValue = sourceValue; // Already a char
                } else if (sourceValue.type == "int") {
                    lastValue = RuntimeValue(static_cast<char>(sourceValue.asInt()));
                } else if (sourceValue.type == "string") {
                    if (sourceValue.asString().length() == 1) {
                        lastValue = RuntimeValue(sourceValue.asString()[0]);
                    } else {
                        error("Cannot cast string of length != 1 to char.");
                    }
                }
                else {
                    error("Cannot cast " + sourceValue.type + " to char.");
                }
                break;
            }
            // Add other types as needed
            default:
                error("Unsupported target type for cast: " + expr.typeToken.lexeme);
                break;
        }
    } catch (const std::runtime_error&) {
        if (expr.isSafe) {
            // Return None (void) for failed safe casts
            lastValue = RuntimeValue(std::any(), "void");
        } else {
            // Re-throw if not a safe cast
            throw;
        }
    }
}

void Interpreter::visit(MapLiteralExpr& expr) {
    std::unordered_map<std::string, std::any> mapData;
    for (const auto& pair : expr.entries) {
        pair.second->accept(*this);

        // Unwrap RuntimeValue to std::any
        std::any anyValue;
        if (std::holds_alternative<int>(lastValue.value)) {
            anyValue = std::get<int>(lastValue.value);
        } else if (std::holds_alternative<double>(lastValue.value)) {
            anyValue = std::get<double>(lastValue.value);
        } else if (std::holds_alternative<std::string>(lastValue.value)) {
            anyValue = std::get<std::string>(lastValue.value);
        } else if (std::holds_alternative<char>(lastValue.value)) {
            anyValue = std::get<char>(lastValue.value);
        } else if (std::holds_alternative<bool>(lastValue.value)) {
            anyValue = std::get<bool>(lastValue.value);
        } else if (std::holds_alternative<std::shared_ptr<ClassInstance>>(lastValue.value)) {
            anyValue = std::get<std::shared_ptr<ClassInstance>>(lastValue.value);
        } else if (std::holds_alternative<std::any>(lastValue.value)) {
            anyValue = std::get<std::any>(lastValue.value);
        }

        mapData[pair.first] = anyValue;
    }
    lastValue = RuntimeValue(std::any(mapData), "map<string,any>");
}

void Interpreter::visit(ArrayLiteralExpr& expr) {
    // Collect element values and determine array type
    std::string elementType = "any";

    // Evaluate first element to determine type
    if (!expr.elements.empty()) {
        expr.elements[0]->accept(*this);
        elementType = lastValue.type;
    }

    // Build appropriate typed array
    if (elementType == "string") {
        std::vector<std::string> arrayData;
        for (const auto& element : expr.elements) {
            element->accept(*this);
            if (std::holds_alternative<std::string>(lastValue.value)) {
                arrayData.push_back(std::get<std::string>(lastValue.value));
            } else {
                error("Array elements must be of the same type (expected: string, got: " + lastValue.type + ")");
                return;
            }
        }
        lastValue = RuntimeValue(std::any(arrayData), "array<string>");
    } else if (elementType == "int") {
        std::vector<int> arrayData;
        for (const auto& element : expr.elements) {
            element->accept(*this);
            if (std::holds_alternative<int>(lastValue.value)) {
                arrayData.push_back(std::get<int>(lastValue.value));
            } else {
                error("Array elements must be of the same type (expected: int, got: " + lastValue.type + ")");
                return;
            }
        }
        lastValue = RuntimeValue(std::any(arrayData), "array<int>");
    } else if (elementType == "double") {
        std::vector<double> arrayData;
        for (const auto& element : expr.elements) {
            element->accept(*this);
            if (std::holds_alternative<double>(lastValue.value)) {
                arrayData.push_back(std::get<double>(lastValue.value));
            } else {
                error("Array elements must be of the same type (expected: double, got: " + lastValue.type + ")");
                return;
            }
        }
        lastValue = RuntimeValue(std::any(arrayData), "array<double>");
    } else if (elementType == "bool") {
        std::vector<bool> arrayData;
        for (const auto& element : expr.elements) {
            element->accept(*this);
            if (std::holds_alternative<bool>(lastValue.value)) {
                arrayData.push_back(std::get<bool>(lastValue.value));
            } else {
                error("Array elements must be of the same type (expected: bool, got: " + lastValue.type + ")");
                return;
            }
        }
        lastValue = RuntimeValue(std::any(arrayData), "array<bool>");
    } else {
        // Generic array for other types
        std::vector<RuntimeValue> arrayData;
        for (const auto& element : expr.elements) {
            element->accept(*this);
            arrayData.push_back(lastValue);
        }
        lastValue = RuntimeValue(std::any(arrayData), "array<" + elementType + ">");
    }
}

void Interpreter::visit(LambdaExpr& expr) {
    auto closure = std::make_shared<Closure>();
    closure->params = expr.params;
    closure->body = expr.body.get();
    closure->env = currentEnv;
    
    lastValue = RuntimeValue(std::any(closure), "function");
}

void Interpreter::visit(StructInitExpr& expr) {
    std::string structName = expr.name.lexeme;

    // Check if struct is defined
    if (!classes.count(structName)) {
        error("Undefined struct: " + structName);
        return;
    }
    
    // Create new instance
    auto instance = std::make_shared<ClassInstance>();
    instance->className = structName;
    
    // Evaluate and assign fields
    for (const auto& field : expr.fields) {
        std::string fieldName = field.first;
        
        // Evaluate field value
        field.second->accept(*this);
        
        // Assign to instance
        instance->fields[fieldName] = lastValue;
    }
    
    // Return the instance
    lastValue = RuntimeValue(instance);
    // Return the instance
    lastValue = RuntimeValue(instance);
}

void Interpreter::visit(WhenExpr& expr) {
    RuntimeValue subjectValue;
    bool hasSubject = false;

    if (expr.condition) {
        expr.condition->accept(*this);
        subjectValue = lastValue;
        hasSubject = true;
    }

    for (const auto& kase : expr.cases) {
        // Skip else cases during normal iteration (handle at end if no match)
        if (kase.isElse) continue;

        bool matchFound = false;
        
        // Logical OR between multiple conditions in a single case (value1, value2 -> ...)
        for (const auto& condExpr : kase.conditions) {
            condExpr->accept(*this);
            RuntimeValue condValue = lastValue;
            
            if (hasSubject) {
                // Equality check
                if (subjectValue.type == condValue.type) {
                    if (subjectValue.type == "int") matchFound = (subjectValue.asInt() == condValue.asInt());
                    else if (subjectValue.type == "double") matchFound = (subjectValue.asDouble() == condValue.asDouble());
                    else if (subjectValue.type == "string") matchFound = (subjectValue.asString() == condValue.asString());
                    else if (subjectValue.type == "bool") matchFound = (subjectValue.asBool() == condValue.asBool());
                }
            } else {
                // Truthiness check
                matchFound = isTruthy(condValue);
            }

            if (matchFound) break; // Found a matching condition for this case
        }

        if (matchFound) {
            if (kase.body) {
                kase.body->accept(*this);
            } else {
                lastValue = RuntimeValue();
            }
            return; // Exit after executing the first matching case
        }
    }

    // No match found in regular cases, check for else
    for (const auto& kase : expr.cases) {
        if (kase.isElse) {
            if (kase.body) {
                kase.body->accept(*this);
            } else {
                lastValue = RuntimeValue();
            }
            return;
        }
    }
    
    // No match and no else
    lastValue = RuntimeValue();
}

void Interpreter::visit(InterpolatedStringExpr& expr) {
    std::string result;

    for (auto& part : expr.parts) {
        if (part.isExpression && part.expression) {
            // Evaluate the expression
            part.expression->accept(*this);

            // Convert the result to string
            if (lastValue.type == "string") {
                result += lastValue.asString();
            } else if (lastValue.type == "int") {
                result += std::to_string(lastValue.asInt());
            } else if (lastValue.type == "double") {
                result += std::to_string(lastValue.asDouble());
            } else if (lastValue.type == "bool") {
                result += lastValue.asBool() ? "true" : "false";
            } else if (lastValue.type == "char") {
                result += lastValue.asChar();
            } else if (lastValue.type == "object") {
                // Try to get a string representation
                auto* anyPtr = std::get_if<std::shared_ptr<ClassInstance>>(&lastValue.value);
                if (anyPtr && *anyPtr) {
                    result += "[" + (*anyPtr)->className + "]";
                } else {
                    result += "[object]";
                }
            } else {
                result += "[" + lastValue.type + "]";
            }
        } else {
            // Literal string part
            result += part.literal;
        }
    }

    lastValue = RuntimeValue(result);
}

void Interpreter::visit(AwaitExpr& expr) {
    // Basic blocking implementation for awaiting futures/promises
    // In a real async runtime, this would suspend execution
    expr.expression->accept(*this);
    
    // Check if the result is a recognizable Future/Promise object
    // For now, we just pass through the value as a placeholder
    // A more complete implementation would check for specific "Future" objects
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
    // We register them anyway so they are found during lookup, and CallExpr handles the native dispatch
    /*
    if (!stmt.body) {
        return;  // Skip registration - this is a native function declaration
    }
    */

    // If we're loading a module, register function in module namespace
    if (!currentModuleName.empty()) {
        moduleFunctions[currentModuleName].emplace(stmt.name.lexeme, std::ref(stmt));  // Safe reference
    } else {
        // Store function for later execution in global scope
        Function func;
        for (const auto& param : stmt.parameters) {
            func.parameters.push_back(&param);
        }
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

void Interpreter::visit(TypeAliasDecl& stmt) {
    // Type aliases are purely compile-time constructs
    // At runtime, they don't need any special handling
    // The alias just maps one type name to another
}

void Interpreter::visit(PackageDecl& stmt) {
    // Skip non-main packages if we're not loading a module
    // These will be loaded via UseStmt when needed
    if (currentModuleName.empty() && stmt.name.lexeme != "main") {
        return;
    }

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
    std::vector<std::string> searchPaths;

    // Add project-root-relative paths if projectRoot is set
    if (!projectRoot.empty()) {
        searchPaths.push_back(projectRoot + "/src/" + moduleName);
        searchPaths.push_back(projectRoot + "/deps/" + moduleName + "/src");
        searchPaths.push_back(projectRoot + "/deps/" + moduleName);
    }

    // Add relative paths as fallback
    searchPaths.push_back("src/" + moduleName);
    searchPaths.push_back("deps/" + moduleName + "/src");
    searchPaths.push_back("deps/" + moduleName);
    searchPaths.push_back("std/" + moduleName);
    searchPaths.push_back("std/encoding/" + moduleName);
    searchPaths.push_back("std/net/" + moduleName);
    searchPaths.push_back("../std/" + moduleName);
    searchPaths.push_back("../std/encoding/" + moduleName);
    searchPaths.push_back("../std/net/" + moduleName);
    searchPaths.push_back("../../std/" + moduleName);
    searchPaths.push_back("../../std/encoding/" + moduleName);
    searchPaths.push_back("../../std/net/" + moduleName);
    searchPaths.push_back("../../../std/" + moduleName);
    searchPaths.push_back("../../../std/encoding/" + moduleName);
    searchPaths.push_back("../../../std/net/" + moduleName);

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
            // Save and set current module name to track module context
            std::string previousModuleName = currentModuleName;
            currentModuleName = moduleName;

            for (const auto& filePath : filesToLoad) {
                std::ifstream file(filePath);
                if (!file.is_open()) continue;

                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string source = buffer.str();

                try {
                    // Lex, parse, and execute the module file
                    Lexer lexer(source, filePath);
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

            // Restore previous module name after loading
            currentModuleName = previousModuleName;
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
       // std::cerr << "[DEBUG] IfStmt: no branch executed" << std::endl;
    }
}

void Interpreter::visit(WhileStmt& stmt) {
    while (true) {
        stmt.condition->accept(*this);
        RuntimeValue condition = lastValue;

        if (!isTruthy(condition)) break;

        try {
            stmt.body->accept(*this);
        } catch (BreakException&) {
            break;
        } catch (ContinueException&) {
            continue;
        }
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
                    try {
                        stmt.body->accept(*this);
                    } catch (BreakException&) {
                        exitScope();
                        break;
                    } catch (ContinueException&) {
                        exitScope();
                        continue;
                    }

                    exitScope();
                }
            } catch (const std::bad_any_cast&) {
                error("Failed to iterate over array in for loop");
            }
        } 
        // Handle array<int> (ranges)
        else if (iterableValue.type == "array<int>") {
            try {
                auto& vec = std::any_cast<std::vector<int>&>(anyValue);

                // Iterate over each element
                for (int element : vec) {
                    // Create new scope for loop body
                    enterScope();

                    // Define loop variable with current element
                    currentEnv->define(stmt.variable.lexeme, RuntimeValue(element));

                    // Execute loop body
                    try {
                        stmt.body->accept(*this);
                    } catch (BreakException&) {
                        exitScope();
                        break;
                    } catch (ContinueException&) {
                        exitScope();
                        continue;
                    }

                    exitScope();
                }
            } catch (const std::bad_any_cast&) {
                error("Failed to iterate over int array in for loop");
            }
        }
        // Handle array<any>
        else if (iterableValue.type == "array<any>") {
            try {
                auto& vec = std::any_cast<std::vector<RuntimeValue>&>(anyValue);

                // Iterate over each element
                for (const auto& element : vec) {
                    // Create new scope for loop body
                    enterScope();

                    // Define loop variable with current element
                    // element is already RuntimeValue
                    currentEnv->define(stmt.variable.lexeme, element);

                    // Execute loop body
                    try {
                        stmt.body->accept(*this);
                    } catch (BreakException&) {
                        exitScope();
                        break;
                    } catch (ContinueException&) {
                        exitScope();
                        continue;
                    }

                    exitScope();
                }
            } catch (const std::bad_any_cast&) {
                error("Failed to iterate over any array in for loop");
            }
        }
        // Handle array of objects (e.g., array<FileInfo>, array<ClassInstance>)
        else {
            try {
                auto& vec = std::any_cast<std::vector<std::shared_ptr<ClassInstance>>&>(anyValue);

                // Iterate over each element
                for (const auto& element : vec) {
                    // Create new scope for loop body
                    enterScope();

                    // Define loop variable with current element as object
                    currentEnv->define(stmt.variable.lexeme, RuntimeValue(element));

                    // Execute loop body
                    try {
                        stmt.body->accept(*this);
                    } catch (BreakException&) {
                        exitScope();
                        break;
                    } catch (ContinueException&) {
                        exitScope();
                        continue;
                    }

                    exitScope();
                }
            } catch (const std::bad_any_cast&) {
                error("Unsupported array type in for loop: " + iterableValue.type);
            }
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
                    try {
                        stmt.body->accept(*this);
                    } catch (BreakException&) {
                        exitScope();
                        break;
                    } catch (ContinueException&) {
                        exitScope();
                        continue;
                    }

                    exitScope();
                }
            } catch (const std::bad_any_cast&) {
                error("Failed to iterate over map in for loop");
            }
        } else {
            error("Unsupported map type in for loop: " + iterableValue.type);
        }
    }
    // Handle string iteration (iterate over characters)
    else if (iterableValue.type == "string") {
        if (!std::holds_alternative<std::string>(iterableValue.value)) {
            error("Invalid string value in for loop");
        }

        const std::string& str = std::get<std::string>(iterableValue.value);

        // Iterate over each character
        for (size_t i = 0; i < str.length(); ++i) {
            // Create new scope for loop body
            enterScope();

            // Define loop variable with current character as a string
            std::string charStr(1, str[i]);
            currentEnv->define(stmt.variable.lexeme, RuntimeValue(charStr));

            // Execute loop body
            try {
                stmt.body->accept(*this);
            } catch (BreakException&) {
                exitScope();
                break;
            } catch (ContinueException&) {
                exitScope();
                continue;
            }

            exitScope();
        }
    }
    else {
        error("For loop requires an iterable (array, map, or string), got: " + iterableValue.type);
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

    // Execute deferred statements in LIFO order (Go-style defer)
    while (!deferredStatements.empty()) {
        Stmt* deferred = deferredStatements.back();
        deferredStatements.pop_back();
        if (deferred) {
            deferred->accept(*this);
        }
    }

    throw ReturnException(value);
}

void Interpreter::visit(BreakStmt& stmt) {
    throw BreakException();
}

void Interpreter::visit(ContinueStmt& stmt) {
    throw ContinueException();
}

void Interpreter::visit(DeferStmt& stmt) {
    // Store the deferred statement for later execution
    // The deferred statement will be executed when the current function returns
    if (stmt.statement) {
        deferredStatements.push_back(stmt.statement.get());
    }
}

void Interpreter::visit(DestructuringDecl& stmt) {
    // Evaluate the initializer
    stmt.initializer->accept(*this);
    RuntimeValue value = lastValue;

    // Handle array destructuring
    if (value.type.starts_with("array")) {
        auto& anyValue = std::get<std::any>(value.value);

        // Handle array<int>
        if (value.type == "array<int>") {
            try {
                auto& vec = std::any_cast<std::vector<int>&>(anyValue);
                for (size_t i = 0; i < stmt.names.size(); ++i) {
                    if (i < vec.size()) {
                        currentEnv->define(stmt.names[i].lexeme, RuntimeValue(vec[i]));
                    } else {
                        currentEnv->define(stmt.names[i].lexeme, RuntimeValue(0));
                    }
                }
            } catch (const std::bad_any_cast&) {
                error("Failed to destructure array<int>");
            }
        }
        // Handle array<string>
        else if (value.type == "array<string>") {
            try {
                auto& vec = std::any_cast<std::vector<std::string>&>(anyValue);
                for (size_t i = 0; i < stmt.names.size(); ++i) {
                    if (i < vec.size()) {
                        currentEnv->define(stmt.names[i].lexeme, RuntimeValue(vec[i]));
                    } else {
                        currentEnv->define(stmt.names[i].lexeme, RuntimeValue(std::string("")));
                    }
                }
            } catch (const std::bad_any_cast&) {
                error("Failed to destructure array<string>");
            }
        }
        // Handle array<any> (generic arrays)
        else if (value.type == "array<any>") {
            try {
                auto& vec = std::any_cast<std::vector<RuntimeValue>&>(anyValue);
                for (size_t i = 0; i < stmt.names.size(); ++i) {
                    if (i < vec.size()) {
                        currentEnv->define(stmt.names[i].lexeme, vec[i]);
                    } else {
                        currentEnv->define(stmt.names[i].lexeme, RuntimeValue());
                    }
                }
            } catch (const std::bad_any_cast&) {
                error("Failed to destructure array<any>");
            }
        }
        else {
            error("Destructuring not supported for type: " + value.type);
        }
    }
    else {
        error("Cannot destructure non-array type: " + value.type);
    }
}

void Interpreter::visit(SelectStmt& stmt) {
    // Implement Go-style select with non-blocking checks
    // Try each case in order, execute first one that's ready
    // If no case is ready, execute default (else) case if present

    bool caseExecuted = false;
    SelectCase* defaultCase = nullptr;

    // First pass: try to find a ready case
    for (auto& selectCase : stmt.cases) {
        if (selectCase.kind == SelectCase::Kind::DEFAULT) {
            defaultCase = &selectCase;
            continue;
        }

        if (selectCase.kind == SelectCase::Kind::RECEIVE) {
            // Evaluate the channel expression
            selectCase.channel->accept(*this);
            RuntimeValue channelValue = lastValue;

            if (channelValue.type == "Channel" || channelValue.type.find("Channel") != std::string::npos) {
                auto channelObj = channelValue.asObject();
                if (channelObj) {
                    // Check if channel has data (non-blocking)
                    auto queueIt = channelObj->fields.find("__queue");
                    if (queueIt != channelObj->fields.end()) {
                        if (auto* queuePtr = std::get_if<std::any>(&queueIt->second.value)) {
                            try {
                                auto& queue = std::any_cast<std::vector<RuntimeValue>&>(*queuePtr);
                                if (!queue.empty()) {
                                    // Receive value
                                    RuntimeValue receivedValue = queue.front();
                                    queue.erase(queue.begin());

                                    // Store in variable if specified
                                    if (!selectCase.variable.lexeme.empty()) {
                                        currentEnv->define(selectCase.variable.lexeme, receivedValue);
                                    }

                                    // Execute case body
                                    if (selectCase.body) {
                                        selectCase.body->accept(*this);
                                    }
                                    caseExecuted = true;
                                    break;
                                }
                            } catch (...) {}
                        }
                    }
                }
            }
        }
        else if (selectCase.kind == SelectCase::Kind::SEND) {
            // Evaluate the channel expression
            selectCase.channel->accept(*this);
            RuntimeValue channelValue = lastValue;

            if (channelValue.type == "Channel" || channelValue.type.find("Channel") != std::string::npos) {
                auto channelObj = channelValue.asObject();
                if (channelObj) {
                    // Evaluate the value to send
                    selectCase.sendValue->accept(*this);
                    RuntimeValue sendValue = lastValue;

                    // Add to channel queue
                    auto queueIt = channelObj->fields.find("__queue");
                    if (queueIt != channelObj->fields.end()) {
                        if (auto* queuePtr = std::get_if<std::any>(&queueIt->second.value)) {
                            try {
                                auto& queue = std::any_cast<std::vector<RuntimeValue>&>(*queuePtr);
                                queue.push_back(sendValue);

                                // Execute case body
                                if (selectCase.body) {
                                    selectCase.body->accept(*this);
                                }
                                caseExecuted = true;
                                break;
                            } catch (...) {}
                        }
                    }
                }
            }
        }
    }

    // If no case executed, run default case
    if (!caseExecuted && defaultCase != nullptr) {
        if (defaultCase->body) {
            defaultCase->body->accept(*this);
        }
    }
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
    // std::cout << "Calling native: " << moduleName << "::" << functionName << std::endl;
    for (const auto& arg : args) {
        // std::cout << "  Arg type: " << arg.type << " index: " << arg.value.index() << std::endl;
        std::any anyValue;
        if (std::holds_alternative<int>(arg.value)) {
            anyValue = std::get<int>(arg.value);
        } else if (std::holds_alternative<double>(arg.value)) {
            anyValue = std::get<double>(arg.value);
        } else if (std::holds_alternative<std::string>(arg.value)) {
            // std::cout << "  Arg string value: " << std::get<std::string>(arg.value) << std::endl;
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

        // Handle object types (they are ClassInstance)
        if (resultType.starts_with("Result") || resultType == "Channel" ||
            resultType == "WaitGroup" || resultType == "Mutex" ||
            resultType == "Optional" || resultType == "File" ||
            resultType == "WorkerPool" || resultType == "Router" ||
            resultType == "Server" || resultType == "Request" ||
            resultType == "Response" || resultType == "JsonValue" ||
            resultType == "Time" || resultType == "Duration") {
            resultType = "object";
        }
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
    for (size_t i = 0; i < func.parameters.size(); ++i) {
        const auto* param = func.parameters[i];
        
        if (param->isVariadic) {
            // Collect all remaining arguments into an array
            std::vector<RuntimeValue> varArgs;
            for (size_t j = i; j < args.size(); ++j) {
                varArgs.push_back(args[j]);
            }
            // Bind as array<any>
            currentEnv->define(param->name.lexeme, RuntimeValue(std::any(varArgs), "array<any>"));
            break; // Variadic is always last
        } else if (i < args.size()) {
            // Argument provided
            currentEnv->define(param->name.lexeme, args[i]);
        } else if (param->defaultValue) {
            // Use default value
            param->defaultValue->accept(*this);
            currentEnv->define(param->name.lexeme, lastValue);
        } else {
            error("Missing argument for parameter: " + param->name.lexeme);
        }
    }

    RuntimeValue result;
    try {
        if (func.body && func.body->get()) {
            for (const auto& stmt : *func.body->get()) {
                if (stmt) stmt->accept(*this);
            }
            result = RuntimeValue(std::any(), "void");
        } else {
            // Native function fallback
            // Try to find the function in NativeRegistry in common modules
            auto& registry = NativeRegistry::getInstance();
            std::vector<std::string> modules = {"collections", "math", "io", "strings", "prelude"};
            bool found = false;
            
            for (const auto& mod : modules) {
                if (registry.isNative(mod, name)) {
                    result = evaluateNativeCall(mod, name, args);
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                // If not found in common modules, just return void (or logic error?)
                 result = RuntimeValue(std::any(), "void");
            }
        }
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
    for (size_t i = 0; i < funcDecl->parameters.size(); ++i) {
        const auto& param = funcDecl->parameters[i];
        
        if (param.isVariadic) {
            // Collect all remaining arguments into an array
            std::vector<RuntimeValue> varArgs;
            for (size_t j = i; j < args.size(); ++j) {
                varArgs.push_back(args[j]);
            }
            // Bind as array<any>
            currentEnv->define(param.name.lexeme, RuntimeValue(std::any(varArgs), "array<any>"));
            break; // Variadic is always last
        } else if (i < args.size()) {
            // Argument provided
            currentEnv->define(param.name.lexeme, args[i]);
        } else if (param.defaultValue) {
            // Use default value
            param.defaultValue->accept(*this);
            currentEnv->define(param.name.lexeme, lastValue);
        } else {
            error("Missing argument for parameter: " + param.name.lexeme);
        }
    }

    // Execute function body
    RuntimeValue result;
    try {
        if (funcDecl->body && !funcDecl->body->empty()) {
            for (const auto& stmt : *funcDecl->body) {
                if (stmt) {
                    stmt->accept(*this);
                }
            }
            result = RuntimeValue(std::any(), "void");
        } else {
            // Function has no body - it's a stub/declaration that should be native
            exitScope();
            currentExecutingModule = previousModule;
            error("Function '" + moduleName + "::" + functionName + "' is declared but not implemented. "
                  "This may be a native function that requires C++ implementation.");
            return RuntimeValue(std::any(), "void");
        }
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
        for (size_t i = 0; i < constructor->parameters.size(); ++i) {
            const auto& param = constructor->parameters[i];
            
            if (param.isVariadic) {
                std::vector<RuntimeValue> varArgs;
                for (size_t j = i; j < args.size(); ++j) {
                    varArgs.push_back(args[j]);
                }
                currentEnv->define(param.name.lexeme, RuntimeValue(std::any(varArgs), "array<any>"));
                break;
            } else if (i < args.size()) {
                currentEnv->define(param.name.lexeme, args[i]);
            } else if (param.defaultValue) {
                param.defaultValue->accept(*this);
                currentEnv->define(param.name.lexeme, lastValue);
            } else {
                error("Missing argument for constructor parameter: " + param.name.lexeme);
            }
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

RuntimeValue Interpreter::executeCallback(const RuntimeValue& closureValue, const std::vector<RuntimeValue>& args) {
    if (closureValue.type != "function") {
        error("Attempt to call non-function type: " + closureValue.type);
    }

    // Extract the std::any from the variant, then cast to Closure
    auto* anyPtr = std::get_if<std::any>(&closureValue.value);
    if (!anyPtr) {
        error("Internal error: Function value is not stored as std::any");
        return RuntimeValue(); // Unreachable
    }
    
    auto closure = std::any_cast<std::shared_ptr<Closure>>(*anyPtr);
    
    // Save current environment
    Environment* previousEnv = currentEnv;
    
    // Switch to closure's captured environment
    currentEnv = closure->env;
    
    // Enter new scope for parameters
    enterScope();
    
    // Bind parameters
    for (size_t i = 0; i < closure->params.size() && i < args.size(); ++i) {
        currentEnv->define(closure->params[i].lexeme, args[i]);
    }
    
    // Execute body
    RuntimeValue result(std::any(), "void");
    try {
        if (closure->body) {
            closure->body->accept(*this);
        }
    } catch (ReturnException& ret) {
        result = ret.value;
    }
    
    // Exit parameter scope
    exitScope();

    // Restore original environment
    currentEnv = previousEnv;

    return result;
}

void Interpreter::executeHttpHandler(const RuntimeValue& handler, HttpRequestInternal& req, HttpResponseInternal& res) {
    // Create Request object
    auto reqObj = std::make_shared<ClassInstance>();
    reqObj->className = "Request";
    reqObj->fields["method"] = RuntimeValue(methodToString(req.method));
    reqObj->fields["url"] = RuntimeValue(req.url);
    reqObj->fields["path"] = RuntimeValue(req.path);
    reqObj->fields["query"] = RuntimeValue(req.query);
    reqObj->fields["body"] = RuntimeValue(req.body);
    reqObj->fields["remoteAddr"] = RuntimeValue(req.remoteAddr);

    // Convert headers to a map
    std::unordered_map<std::string, std::any> headerMap;
    for (const auto& [key, value] : req.headers) {
        headerMap[key] = value;
    }
    reqObj->fields["headers"] = RuntimeValue(std::any(headerMap), "map<string, string>");

    // Convert query params to a map
    std::unordered_map<std::string, std::any> queryMap;
    for (const auto& [key, value] : req.queryParams) {
        queryMap[key] = value;
    }
    reqObj->fields["queryParams"] = RuntimeValue(std::any(queryMap), "map<string, string>");

    // Convert path params to a map
    std::unordered_map<std::string, std::any> pathMap;
    for (const auto& [key, value] : req.pathParams) {
        pathMap[key] = value;
    }
    reqObj->fields["pathParams"] = RuntimeValue(std::any(pathMap), "map<string, string>");
    reqObj->fields["params"] = RuntimeValue(std::any(pathMap), "map<string, string>");

    // Create Response object
    auto resObj = std::make_shared<ClassInstance>();
    resObj->className = "Response";
    resObj->fields["statusCode"] = RuntimeValue(200);
    resObj->fields["body"] = RuntimeValue(std::string(""));
    std::unordered_map<std::string, std::any> resHeaders;
    resHeaders["Content-Type"] = std::string("text/plain");
    resObj->fields["headers"] = RuntimeValue(std::any(resHeaders), "map<string, string>");

    // Call the handler
    std::vector<RuntimeValue> args;
    args.push_back(RuntimeValue(reqObj));
    args.push_back(RuntimeValue(resObj));

    try {
        executeCallback(handler, args);
    } catch (const std::exception& e) {
        res.statusCode = 500;
        res.body = "{\"error\": \"" + std::string(e.what()) + "\"}";
        res.headers["Content-Type"] = "application/json";
        return;
    }

    // Extract response data
    if (resObj->fields.count("statusCode")) {
        auto& statusVal = resObj->fields["statusCode"];
        if (statusVal.type == "int") {
            res.statusCode = statusVal.asInt();
        }
    }

    if (resObj->fields.count("body")) {
        auto& bodyVal = resObj->fields["body"];
        if (bodyVal.type == "string") {
            res.body = bodyVal.asString();
        }
    }

    if (resObj->fields.count("headers")) {
        auto& headersVal = resObj->fields["headers"];
        if (headersVal.type.starts_with("map")) {
            try {
                auto* anyPtr = std::get_if<std::any>(&headersVal.value);
                if (anyPtr) {
                    auto headers = std::any_cast<std::unordered_map<std::string, std::any>>(*anyPtr);
                    for (const auto& [key, value] : headers) {
                        if (value.type() == typeid(std::string)) {
                            res.headers[key] = std::any_cast<std::string>(value);
                        }
                    }
                }
            } catch (...) {}
        }
    }
}

bool Interpreter::executeHttpMiddleware(const RuntimeValue& middleware, HttpRequestInternal& req, HttpResponseInternal& res) {
    // Create Request object (same as handler)
    auto reqObj = std::make_shared<ClassInstance>();
    reqObj->className = "Request";
    reqObj->fields["method"] = RuntimeValue(methodToString(req.method));
    reqObj->fields["url"] = RuntimeValue(req.url);
    reqObj->fields["path"] = RuntimeValue(req.path);
    reqObj->fields["query"] = RuntimeValue(req.query);
    reqObj->fields["body"] = RuntimeValue(req.body);
    reqObj->fields["remoteAddr"] = RuntimeValue(req.remoteAddr);

    // Convert headers to a map
    std::unordered_map<std::string, std::any> headerMap;
    for (const auto& [key, value] : req.headers) {
        headerMap[key] = value;
    }
    reqObj->fields["headers"] = RuntimeValue(std::any(headerMap), "map<string, string>");

    // Convert query params to a map
    std::unordered_map<std::string, std::any> queryMap;
    for (const auto& [key, value] : req.queryParams) {
        queryMap[key] = value;
    }
    reqObj->fields["queryParams"] = RuntimeValue(std::any(queryMap), "map<string, string>");

    // Convert path params to a map
    std::unordered_map<std::string, std::any> pathMap;
    for (const auto& [key, value] : req.pathParams) {
        pathMap[key] = value;
    }
    reqObj->fields["pathParams"] = RuntimeValue(std::any(pathMap), "map<string, string>");
    reqObj->fields["params"] = RuntimeValue(std::any(pathMap), "map<string, string>");

    // Create Response object
    auto resObj = std::make_shared<ClassInstance>();
    resObj->className = "Response";
    resObj->fields["statusCode"] = RuntimeValue(res.statusCode);  // Preserve existing status
    resObj->fields["body"] = RuntimeValue(res.body);
    std::unordered_map<std::string, std::any> resHeaders;
    for (const auto& [key, value] : res.headers) {
        resHeaders[key] = value;
    }
    if (resHeaders.empty()) {
        resHeaders["Content-Type"] = std::string("text/plain");
    }
    resObj->fields["headers"] = RuntimeValue(std::any(resHeaders), "map<string, string>");

    // Create a "next called" flag
    bool nextCalled = false;

    // Create the "next" function that sets the flag
    auto nextClosure = std::make_shared<Closure>();
    nextClosure->parameters = {};
    nextClosure->body = nullptr;
    nextClosure->env = currentEnv;
    nextClosure->isAsync = false;

    // We need to track whether next() was called
    // Create a special flag field on the response object
    reqObj->fields["__nextCalled"] = RuntimeValue(false);

    // Call the middleware with (req, res, next)
    // The next function is a no-op callback - middleware behavior signals continuation
    std::vector<RuntimeValue> args;
    args.push_back(RuntimeValue(reqObj));
    args.push_back(RuntimeValue(resObj));

    // Create a simple next function that sets a flag
    // For now, middleware returns to indicate continuation
    // A more sophisticated implementation would use a callback

    try {
        RuntimeValue result = executeCallback(middleware, args);

        // Check if middleware explicitly returned false to stop chain
        if (result.type == "bool" && !result.asBool()) {
            nextCalled = false;
        } else {
            // By default, continue the chain unless response was sent with error
            nextCalled = true;
        }
    } catch (const std::exception& e) {
        res.statusCode = 500;
        res.body = "{\"error\": \"Middleware error: " + std::string(e.what()) + "\"}";
        res.headers["Content-Type"] = "application/json";
        return false;
    }

    // Extract any modifications made to the response
    if (resObj->fields.count("statusCode")) {
        auto& statusVal = resObj->fields["statusCode"];
        if (statusVal.type == "int") {
            res.statusCode = statusVal.asInt();
        }
    }

    if (resObj->fields.count("body")) {
        auto& bodyVal = resObj->fields["body"];
        if (bodyVal.type == "string") {
            res.body = bodyVal.asString();
        }
    }

    if (resObj->fields.count("headers")) {
        auto& headersVal = resObj->fields["headers"];
        if (headersVal.type.starts_with("map")) {
            try {
                auto* anyPtr = std::get_if<std::any>(&headersVal.value);
                if (anyPtr) {
                    auto headers = std::any_cast<std::unordered_map<std::string, std::any>>(*anyPtr);
                    for (const auto& [key, value] : headers) {
                        if (value.type() == typeid(std::string)) {
                            res.headers[key] = std::any_cast<std::string>(value);
                        }
                    }
                }
            } catch (...) {}
        }
    }

    // Also update request modifications back (e.g., middleware might add auth info)
    if (reqObj->fields.count("headers")) {
        auto& headersVal = reqObj->fields["headers"];
        if (headersVal.type.starts_with("map")) {
            try {
                auto* anyPtr = std::get_if<std::any>(&headersVal.value);
                if (anyPtr) {
                    auto headers = std::any_cast<std::unordered_map<std::string, std::any>>(*anyPtr);
                    req.headers.clear();
                    for (const auto& [key, value] : headers) {
                        if (value.type() == typeid(std::string)) {
                            req.headers[key] = std::any_cast<std::string>(value);
                        }
                    }
                }
            } catch (...) {}
        }
    }

    return nextCalled;
}

void Interpreter::spawnGoroutine(const RuntimeValue& closureValue) {
    if (closureValue.type != "function") {
        error("concurrent.go() requires a function argument");
        return;
    }

    // Extract the closure
    auto* anyPtr = std::get_if<std::any>(&closureValue.value);
    if (!anyPtr) {
        error("Internal error: Function value is not stored as std::any");
        return;
    }

    auto closure = std::any_cast<std::shared_ptr<Closure>>(*anyPtr);

    // Spawn a new thread that executes the closure
    // We need to capture a copy of the closure and use a mutex to protect interpreter state
    goroutines.emplace_back([this, closure]() {
        // Acquire the interpreter mutex to execute the closure
        std::lock_guard<std::mutex> lock(interpreterMutex);

        if (shuttingDown) return;

        // Save current environment
        Environment* previousEnv = currentEnv;

        // Switch to closure's captured environment
        currentEnv = closure->env;

        // Enter new scope for parameters (closures passed to go() typically have no params)
        enterScope();

        // Execute body
        try {
            if (closure->body) {
                closure->body->accept(*this);
            }
        } catch (ReturnException& ret) {
            // Ignore return value from goroutine
        } catch (std::exception& e) {
            std::cerr << "Error in goroutine: " << e.what() << std::endl;
        }

        // Exit parameter scope
        exitScope();

        // Restore original environment
        currentEnv = previousEnv;
    });
}

void Interpreter::joinAllGoroutines() {
    shuttingDown = true;
    for (auto& t : goroutines) {
        if (t.joinable()) {
            t.join();
        }
    }
    goroutines.clear();
    shuttingDown = false;
}

RuntimeValue Interpreter::createWorkerPool(int numWorkers, const RuntimeValue& workerFunc) {
    // Create a worker pool object
    auto pool = std::make_shared<WorkerPool>();

    // Create the results channel
    auto resultsChannel = std::make_shared<ClassInstance>();
    resultsChannel->className = "Channel";
    resultsChannel->fields["closed"] = RuntimeValue(false);
    std::vector<RuntimeValue> emptyQueue;
    resultsChannel->fields["__queue"] = RuntimeValue(std::any(emptyQueue), "array<any>");
    pool->resultsChannel = resultsChannel;

    // Extract the worker function closure
    auto* anyPtr = std::get_if<std::any>(&workerFunc.value);
    if (!anyPtr) {
        error("Worker function is not a valid closure");
        return RuntimeValue();
    }
    auto workerClosure = std::any_cast<std::shared_ptr<Closure>>(*anyPtr);

    // Spawn worker threads
    for (int i = 0; i < numWorkers; i++) {
        pool->workers.emplace_back([this, pool, workerClosure]() {
            while (true) {
                RuntimeValue job;
                {
                    std::unique_lock<std::mutex> lock(pool->poolMutex);
                    pool->jobAvailable.wait(lock, [&pool]() {
                        return !pool->jobQueue.empty() || pool->closed;
                    });

                    if (pool->closed && pool->jobQueue.empty()) {
                        return;
                    }

                    job = pool->jobQueue.front();
                    pool->jobQueue.pop();
                    pool->activeWorkers++;
                }

                // Execute the worker function with the job
                RuntimeValue result;
                {
                    std::lock_guard<std::mutex> lock(interpreterMutex);
                    std::vector<RuntimeValue> args = {job};
                    result = executeCallback(RuntimeValue(std::any(workerClosure), "function"), args);
                }

                // Put result in results channel
                {
                    std::lock_guard<std::mutex> lock(pool->poolMutex);
                    if (auto* queuePtr = std::get_if<std::any>(&pool->resultsChannel->fields["__queue"].value)) {
                        auto queue = std::any_cast<std::vector<RuntimeValue>>(*queuePtr);
                        queue.push_back(result);
                        pool->resultsChannel->fields["__queue"] = RuntimeValue(std::any(queue), "array<any>");
                    }
                    pool->activeWorkers--;
                    pool->jobDone.notify_all();
                }
            }
        });
    }

    // Store the pool
    workerPools.push_back(pool);

    // Create and return a WorkerPool object
    auto poolObj = std::make_shared<ClassInstance>();
    poolObj->className = "WorkerPool";
    poolObj->fields["__poolId"] = RuntimeValue(static_cast<int>(workerPools.size() - 1));
    poolObj->fields["numWorkers"] = RuntimeValue(numWorkers);
    poolObj->fields["results"] = RuntimeValue(resultsChannel);

    return RuntimeValue(poolObj);
}

RuntimeValue Interpreter::createPipeline(const RuntimeValue& inputChannel, const std::vector<RuntimeValue>& stages) {
    if (stages.empty()) {
        error("Pipeline requires at least one stage");
        return RuntimeValue();
    }

    // Create intermediate channels for each stage
    std::vector<std::shared_ptr<ClassInstance>> channels;

    // First channel is the input
    auto inputObj = inputChannel.asObject();
    channels.push_back(inputObj);

    // Create intermediate and output channels
    for (size_t i = 0; i < stages.size(); i++) {
        auto channel = std::make_shared<ClassInstance>();
        channel->className = "Channel";
        channel->fields["closed"] = RuntimeValue(false);
        std::vector<RuntimeValue> emptyQueue;
        channel->fields["__queue"] = RuntimeValue(std::any(emptyQueue), "array<any>");
        channels.push_back(channel);
    }

    // Spawn a goroutine for each stage
    for (size_t i = 0; i < stages.size(); i++) {
        auto* anyPtr = std::get_if<std::any>(&stages[i].value);
        if (!anyPtr) continue;

        auto stageClosure = std::any_cast<std::shared_ptr<Closure>>(*anyPtr);
        auto inChannel = channels[i];
        auto outChannel = channels[i + 1];

        goroutines.emplace_back([this, stageClosure, inChannel, outChannel]() {
            while (true) {
                // Receive from input channel
                RuntimeValue value;
                bool hasValue = false;
                {
                    std::lock_guard<std::mutex> lock(interpreterMutex);
                    if (inChannel->fields.find("__queue") != inChannel->fields.end()) {
                        if (auto* queuePtr = std::get_if<std::any>(&inChannel->fields["__queue"].value)) {
                            auto queue = std::any_cast<std::vector<RuntimeValue>>(*queuePtr);
                            if (!queue.empty()) {
                                value = queue.front();
                                queue.erase(queue.begin());
                                inChannel->fields["__queue"] = RuntimeValue(std::any(queue), "array<any>");
                                hasValue = true;
                            }
                        }
                    }

                    // Check if channel is closed and empty
                    if (!hasValue && inChannel->fields["closed"].asBool()) {
                        // Close output channel and exit
                        outChannel->fields["closed"] = RuntimeValue(true);
                        return;
                    }
                }

                if (!hasValue) {
                    // Wait a bit and try again
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }

                // Process value through stage function
                RuntimeValue result;
                {
                    std::lock_guard<std::mutex> lock(interpreterMutex);
                    std::vector<RuntimeValue> args = {value};
                    result = executeCallback(RuntimeValue(std::any(stageClosure), "function"), args);
                }

                // Send result to output channel
                {
                    std::lock_guard<std::mutex> lock(interpreterMutex);
                    if (auto* queuePtr = std::get_if<std::any>(&outChannel->fields["__queue"].value)) {
                        auto queue = std::any_cast<std::vector<RuntimeValue>>(*queuePtr);
                        queue.push_back(result);
                        outChannel->fields["__queue"] = RuntimeValue(std::any(queue), "array<any>");
                    }
                }
            }
        });
    }

    // Return the output channel (last in the chain)
    return RuntimeValue(channels.back());
}

} // namespace stratos
