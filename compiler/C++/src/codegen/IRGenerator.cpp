#include "stratos/IRGenerator.h"
#include <iostream>

namespace stratos {

IRGenerator::IRGenerator(const std::string& filename) : out(filename) {
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << filename << std::endl;
    }
}

void IRGenerator::generate(const std::vector<std::unique_ptr<Stmt>>& statements) {
    // Boilerplate LLVM IR header
    emitRaw("; ModuleID = 'stratos_module'");
    emitRaw("source_filename = \"stratos_source\"");
    emitRaw("target datalayout = \"e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"");
    emitRaw("target triple = \"x86_64-pc-windows-msvc\"\n");

    // Declare printf for our print statement
    emitRaw("declare i32 @printf(i8*, ...)\n");
    emitRaw("@.str_specifier = private unnamed_addr constant [4 x i8] c\"%d\\0A\\00\"\n"); // For integers

    // Generate code for all top-level statements
    // NOTE: In LLVM, top-level code usually goes into 'main'. 
    // Stratos allows top-level statements, so we'll wrap them in main if they aren't functions.
    
    // We'll scan for functions first, generate them, then put the rest in main.
    // For simplicity now, we assume everything is inside a main function wrapper unless it's a decl. 
    
    emitRaw("define i32 @main() {");
    enterScope();
    
    for (const auto& stmt : statements) {
        if (stmt) stmt->accept(*this);
    }

    emit("ret i32 0");
    exitScope();
    emitRaw("}\n");
}

void IRGenerator::emit(const std::string& code) {
    out << "  " << code << "\n";
}

void IRGenerator::emitRaw(const std::string& code) {
    out << code << "\n";
}

std::string IRGenerator::nextReg() {
    return "%t" + std::to_string(regCount++);
}

std::string IRGenerator::nextLabel() {
    return "L" + std::to_string(labelCount++);
}

void IRGenerator::enterScope() {
    scopes.push_back({});
}

void IRGenerator::exitScope() {
    scopes.pop_back();
}

void IRGenerator::defineVar(const std::string& name, const std::string& ptr) {
    if (!scopes.empty()) {
        scopes.back()[name] = ptr;
    }
}

std::string IRGenerator::getVarPtr(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->count(name)) return it->at(name);
    }
    return ""; // Should be unreachable if semantic analysis passed
}

// --- Expressions ---

void IRGenerator::visit(LiteralExpr& expr) {
    if (expr.type == TokenType::NUMBER) {
        // Just return the number string, LLVM handles literals directly
        // But our design stores result in `lastReg`.
        // Since we can't "store" a raw number in a reg name variable,
        // we might need to handle this differently.
        // Let's assume `lastReg` holds the Value OR Register.
        lastReg = expr.value; 
    } else if (expr.type == TokenType::TRUE) {
        lastReg = "1";
    } else if (expr.type == TokenType::FALSE) {
        lastReg = "0";
    }
}

void IRGenerator::visit(VariableExpr& expr) {
    std::string ptr = getVarPtr(expr.name.lexeme);
    std::string reg = nextReg();
    // Load the value from the stack pointer
    emit(reg + " = load i32, i32* " + ptr);
    lastReg = reg;
}

void IRGenerator::visit(BinaryExpr& expr) {
    expr.left->accept(*this);
    std::string leftVal = lastReg;
    
    expr.right->accept(*this);
    std::string rightVal = lastReg;

    std::string res = nextReg();
    std::string opCode;

    switch (expr.op.type) {
        case TokenType::PLUS: opCode = "add nsw i32"; break;
        case TokenType::MINUS: opCode = "sub nsw i32"; break;
        case TokenType::STAR: opCode = "mul nsw i32"; break;
        case TokenType::SLASH: opCode = "sdiv i32"; break;
        // Comparisons
        case TokenType::GREATER: opCode = "icmp sgt i32"; break;
        case TokenType::LESS: opCode = "icmp slt i32"; break;
        default: opCode = "add i32"; break; // Fallback
    }

    emit(res + " = " + opCode + " " + leftVal + ", " + rightVal);
    lastReg = res;
}

void IRGenerator::visit(UnaryExpr& expr) {
    // TODO
}

void IRGenerator::visit(GroupingExpr& expr) {
    expr.expression->accept(*this);
}

void IRGenerator::visit(CallExpr& expr) {
    // Simplified: Assuming generic function call
    // In reality, we need to know the return type.
    // For now, assuming i32 return.
    
    // Evaluate args
    std::string args = "";
    for (size_t i = 0; i < expr.arguments.size(); ++i) {
        expr.arguments[i]->accept(*this);
        args += "i32 " + lastReg;
        if (i < expr.arguments.size() - 1) args += ", ";
    }

    // Getting callee name (assuming simple identifier)
    // We need to cast ASTNode to VariableExpr to get name
    // This is a bit hacky for C++ without dynamic_cast/rtti enable
    // But safely assuming semantic analysis passed.
    // ...
}

// --- Statements ---

void IRGenerator::visit(VarDecl& stmt) {
    // 1. Allocate stack space
    std::string ptr = "%" + stmt.name.lexeme + "_" + std::to_string(regCount++); 
    // Avoid name collisions by appending ID
    // Actually, LLVM identifiers usually start with %.
    
    emit(ptr + " = alloca i32"); // Assume int for everything for now
    defineVar(stmt.name.lexeme, ptr);

    // 2. Store initializer
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
        emit("store i32 " + lastReg + ", i32* " + ptr);
    }
}

void IRGenerator::visit(PrintStmt& stmt) {
    stmt.expression->accept(*this);
    // Call printf
    // i32 (i8*, ...)
    std::string callReg = nextReg();
    emit(callReg + " = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i32 " + lastReg + ")");
}

void IRGenerator::visit(BlockStmt& stmt) {
    enterScope();
    for (const auto& s : stmt.statements) {
        if (s) s->accept(*this);
    }
    exitScope();
}

void IRGenerator::visit(IfStmt& stmt) {
    // Labels
    std::string thenLabel = nextLabel();
    std::string elseLabel = nextLabel();
    std::string endLabel = nextLabel();

    // Condition
    stmt.condition->accept(*this);
    // Use lastReg (assumed i1 result from comparison, or i32 needing cast)
    // For now assuming binary expr returned i1 if it was a comparison
    // If it's a raw number (e.g. if(1)), we need to compare ne 0
    
    // Branch
    emit("br i1 " + lastReg + ", label %" + thenLabel + ", label %" + elseLabel);

    // Then
    emitRaw("\n" + thenLabel + ":");
    stmt.thenBranch->accept(*this);
    emit("br label %" + endLabel);

    // Else
    emitRaw("\n" + elseLabel + ":");
    if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }
    emit("br label %" + endLabel);

    // End
    emitRaw("\n" + endLabel + ":");
}

void IRGenerator::visit(WhileStmt& stmt) {
    std::string condLabel = nextLabel();
    std::string bodyLabel = nextLabel();
    std::string endLabel = nextLabel();

    emit("br label %" + condLabel);
    
    // Condition
    emitRaw("\n" + condLabel + ":");
    stmt.condition->accept(*this);
    emit("br i1 " + lastReg + ", label %" + bodyLabel + ", label %" + endLabel);

    // Body
    emitRaw("\n" + bodyLabel + ":");
    stmt.body->accept(*this);
    emit("br label %" + condLabel);

    emitRaw("\n" + endLabel + ":");
}

// No-ops / TODOs for now
void IRGenerator::visit(FunctionDecl& stmt) {}
void IRGenerator::visit(ClassDecl& stmt) {}
void IRGenerator::visit(NamespaceDecl& stmt) {}
void IRGenerator::visit(ReturnStmt& stmt) {}

} // namespace stratos
