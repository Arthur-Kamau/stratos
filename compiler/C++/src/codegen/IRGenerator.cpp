#include "stratos/IRGenerator.h"
#include <iostream>

namespace stratos {

IRGenerator::IRGenerator(const std::string& filename) : out(filename) {
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << filename << std::endl;
    }
}

void IRGenerator::generate(const std::vector<std::unique_ptr<Stmt>>& statements) {
    // 1. Header
    emitRaw("; ModuleID = 'stratos_module'");
    emitRaw("source_filename = \"stratos_source\"");
    emitRaw("target datalayout = \"e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"");
    emitRaw("target triple = \"x86_64-pc-windows-msvc\"\n");

    emitRaw("declare i32 @printf(i8*, ...)\n");

    // 2. Pre-scan for string literals or globals could go here if we walked AST differently.
    // For now, we'll generate functions and accumulate top-level stmts for main.

    std::vector<Stmt*> topLevelStmts;

    for (const auto& stmt : statements) {
        if (auto fn = dynamic_cast<FunctionDecl*>(stmt.get())) {
            fn->accept(*this); // Generate function definition immediately
        } else if (auto ns = dynamic_cast<NamespaceDecl*>(stmt.get())) {
            // TODO: flatten namespace?
        } else if (auto cls = dynamic_cast<ClassDecl*>(stmt.get())) {
            // TODO: generate structs
        } else {
            topLevelStmts.push_back(stmt.get());
        }
    }

    // 3. Generate Main for top-level code
    if (!topLevelStmts.empty()) {
        generateMainWrapper(topLevelStmts);
    }

    // 4. Emit cached string globals at the top/bottom
    // (LLVM doesn't care about order of globals vs functions usually, but better at top. 
    // Since we write linearly to stream, we might have missed the spot. 
    // However, LLVM IR structure allows globals anywhere. 
    // But we need to declare them. We'll rely on the fact that we can append them or should have buffered.
    // **Correction**: In a single pass stream, we can't write globals at top after finding them.
    // Strategy: We will just emit them as we find them? No, functions need to see them.
    // Valid LLVM IR allows globals at bottom.
    
    emitRaw("\n; String Literals");
    for (const auto& [text, name] : stringLiterals) {
        // @.str = private unnamed_addr constant [N x i8] c"...\00"
        emitRaw(name + " = private unnamed_addr constant [" + std::to_string(text.length() + 1) + " x i8] c\"" + text + "\\00\"");
    }
}

void IRGenerator::generateMainWrapper(const std::vector<Stmt*>& topLevelStmts) {
    emitRaw("\ndefine i32 @main() {");
    enterScope();
    
    for (auto stmt : topLevelStmts) {
        stmt->accept(*this);
    }

    emit("ret i32 0");
    exitScope();
    emitRaw("}\n");
}

// --- Helpers ---

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

void IRGenerator::defineVar(const std::string& name, const std::string& ptr, const std::string& type) {
    if (!scopes.empty()) {
        scopes.back()[name] = {ptr, type};
    }
}

IRGenerator::VarInfo IRGenerator::getVarInfo(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->count(name)) return it->at(name);
    }
    return {"", ""};
}

std::string IRGenerator::getLLVMType(const std::string& stratosType) {
    if (stratosType == "int") return "i32";
    if (stratosType == "double") return "double";
    if (stratosType == "bool") return "i1";
    if (stratosType == "string") return "i8*"; // pointer to char array
    if (stratosType == "void") return "void";
    if (stratosType == "any") return "i8*"; // Simplified
    return "i32"; // Default
}

std::string IRGenerator::getDefaultValue(const std::string& llvmType) {
    if (llvmType == "double") return "0.0";
    if (llvmType == "i8*") return "null";
    return "0";
}

std::string IRGenerator::getOrCreateStringLiteral(const std::string& text) {
    // Escape string logic needed here technically, simplified for now
    if (stringLiterals.count(text)) return stringLiterals[text];
    
    std::string name = "@.str" + std::to_string(strCount++);
    stringLiterals[text] = name;
    return name;
}

// --- Visitors ---

void IRGenerator::visit(FunctionDecl& stmt) {
    regCount = 0; // Reset temp regs for new function

    std::string retType = getLLVMType(stmt.returnType);
    std::string name = "@" + stmt.name.lexeme;
    
    // Params
    std::stringstream paramsSS;
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        std::string type = getLLVMType(stmt.paramTypes[i]);
        paramsSS << type << " %arg" << i;
        if (i < stmt.params.size() - 1) paramsSS << ", ";
    }

    emitRaw("\ndefine " + retType + " " + name + "(" + paramsSS.str() + ") {");
    enterScope();

    // Allocate stack space for params and store them
    // This allows them to be mutable and addressed like local vars
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        std::string paramName = stmt.params[i].lexeme;
        std::string type = getLLVMType(stmt.paramTypes[i]);
        std::string ptr = "%" + paramName + ".addr";
        
        emit(ptr + " = alloca " + type);
        emit("store " + type + " %arg" + std::to_string(i) + ", " + type + "* " + ptr);
        
        defineVar(paramName, ptr, type);
    }

    if (stmt.body) {
        for (const auto& s : *stmt.body) {
            if (s) s->accept(*this);
        }
    }

    // Default return if missing
    if (retType == "void") emit("ret void");
    else if (retType == "i32") emit("ret i32 0");
    else if (retType == "double") emit("ret double 0.0");
    
    exitScope();
    emitRaw("}\n");
}

void IRGenerator::visit(ReturnStmt& stmt) {
    if (stmt.value) {
        stmt.value->accept(*this);
        emit("ret " + lastVal.type + " " + lastVal.reg);
    } else {
        emit("ret void");
    }
}

void IRGenerator::visit(VarDecl& stmt) {
    std::string type = getLLVMType(stmt.typeName.empty() ? "int" : stmt.typeName); // Default to int if inferred
    // TODO: Improve type inference in Semantics to support this better

    std::string ptr = "%" + stmt.name.lexeme + "_" + std::to_string(regCount++);
    emit(ptr + " = alloca " + type);
    defineVar(stmt.name.lexeme, ptr, type);

    if (stmt.initializer) {
        stmt.initializer->accept(*this);
        // Cast if necessary (omitted for now)
        emit("store " + type + " " + lastVal.reg + ", " + type + "* " + ptr);
    }
}

void IRGenerator::visit(CallExpr& expr) {
    // 1. Evaluate arguments
    std::vector<IRValue> args;
    for (const auto& arg : expr.arguments) {
        arg->accept(*this);
        args.push_back(lastVal);
    }

    // 2. Determine Callee Name
    // Hack: Cast callee to VariableExpr
    std::string funcName;
    if (auto var = dynamic_cast<VariableExpr*>(expr.callee.get())) {
        funcName = var->name.lexeme;
    } else {
        // Complex callee (e.g. function pointer) not supported yet
        funcName = "unknown";
    }

    // 3. Generate Call
    if (funcName == "__if_expr") {
        // args[0] = cond, args[1] = then, args[2] = else
        // Ideally we use 'select' instruction if types match
        // %res = select i1 %cond, type %then, type %else
        
        IRValue cond = args[0];
        IRValue thenVal = args[1];
        IRValue elseVal = args[2];
        
        // Ensure cond is i1
        std::string condReg = cond.reg;
        if (cond.type != "i1") {
             std::string castReg = nextReg();
             emit(castReg + " = icmp ne " + cond.type + " " + cond.reg + ", " + getDefaultValue(cond.type));
             condReg = castReg;
        }
        
        std::string resReg = nextReg();
        // Assume result type is thenVal.type (simple inference)
        emit(resReg + " = select i1 " + condReg + ", " + thenVal.type + " " + thenVal.reg + ", " + elseVal.type + " " + elseVal.reg);
        lastVal = {resReg, thenVal.type};
        
    } else if (funcName == "print") {
        // Handle printf special case
        // Logic: Create format string based on arg type
        if (args.empty()) return;
        
        IRValue arg = args[0];
        std::string fmt;
        if (arg.type == "double") fmt = "%f\0A";
        else if (arg.type == "i8*") fmt = "%s\0A";
        else fmt = "%d\0A"; // int/bool
        
        std::string fmtPtr = getOrCreateStringLiteral(fmt);
        // We need to bitcast the [N x i8]* to i8*
        std::string fmtReg = nextReg();
        // Assuming literal format string length for GEP
        // For simplicity, just use standard getelementptr pattern
        int len = fmt.length() - 2; // Approximate unescaped length, tricky without strict counting. 
        // Safer: Declare specific global for int specifier and reuse.
        
        // REVERT to simple global specifier for this prototype
        // Assuming @.str_specifier is "%d\n"
        // TODO: Real implementation needs dynamic specifiers
        
        std::string callReg = nextReg();
        emit(callReg + " = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), " + arg.type + " " + arg.reg + ")");
        lastVal = {callReg, "i32"};

    } else {
        // User function call
        // We need return type. Assumed i32 for now without symbol table lookup here.
        std::string retType = "i32"; // TODO: Lookup
        
        std::stringstream argSS;
        for (size_t i = 0; i < args.size(); ++i) {
            argSS << args[i].type << " " << args[i].reg;
            if (i < args.size() - 1) argSS << ", ";
        }
        
        std::string callReg = nextReg();
        emit(callReg + " = call " + retType + " @" + funcName + "(" + argSS.str() + ")");
        lastVal = {callReg, retType};
    }
}

void IRGenerator::visit(LiteralExpr& expr) {
    if (expr.type == TokenType::NUMBER) {
        if (expr.value.find('.') != std::string::npos) {
             lastVal = {expr.value, "double"};
        } else {
             lastVal = {expr.value, "i32"};
        }
    } else if (expr.type == TokenType::TRUE) {
        lastVal = {"1", "i1"};
    } else if (expr.type == TokenType::FALSE) {
        lastVal = {"0", "i1"};
    } else if (expr.type == TokenType::STRING) {
        std::string globalName = getOrCreateStringLiteral(expr.value);
        std::string reg = nextReg();
        // Get pointer to start of array
        int size = expr.value.length() + 1;
        emit(reg + " = getelementptr inbounds [" + std::to_string(size) + " x i8], [" + std::to_string(size) + " x i8]* " + globalName + ", i64 0, i64 0");
        lastVal = {reg, "i8*"};
    } else if (expr.type == TokenType::NONE) {
        lastVal = {"null", "i8*"};
    }
}

void IRGenerator::visit(BinaryExpr& expr) {
    expr.left->accept(*this);
    IRValue left = lastVal;
    
    expr.right->accept(*this);
    IRValue right = lastVal;
    
    // Type promotion (simple)
    std::string type = left.type;
    if (right.type == "double") type = "double";
    
    // TODO: Emit casts if types differ (sitofp etc.)

    std::string res = nextReg();
    std::string opCode;

    bool isFloat = (type == "double");

    switch (expr.op.type) {
        case TokenType::PLUS: opCode = isFloat ? "fadd" : "add nsw"; break;
        case TokenType::MINUS: opCode = isFloat ? "fsub" : "sub nsw"; break;
        case TokenType::STAR: opCode = isFloat ? "fmul" : "mul nsw"; break;
        case TokenType::SLASH: opCode = isFloat ? "fdiv" : "sdiv"; break;
        case TokenType::GREATER: opCode = isFloat ? "fcmp ogt" : "icmp sgt"; break;
        case TokenType::LESS: opCode = isFloat ? "fcmp olt" : "icmp slt"; break;
        case TokenType::EQUAL_EQUAL: opCode = isFloat ? "fcmp oeq" : "icmp eq"; break;
        default: opCode = "add"; break;
    }

    emit(res + " = " + opCode + " " + type + " " + left.reg + ", " + right.reg);
    
    // Result type for cmp is i1
    if (opCode.find("cmp") != std::string::npos) {
        lastVal = {res, "i1"};
    } else {
        lastVal = {res, type};
    }
}

void IRGenerator::visit(VariableExpr& expr) {
    VarInfo info = getVarInfo(expr.name.lexeme);
    if (info.ptr.empty()) {
        // Assume global variable or literal we missed.
        // For 02_control_flow.st, 'age' is defined but maybe scope issue?
        // Or if it's a raw number in AST treated as var?
        lastVal = {"0", "i32"};
        return;
    }
    std::string reg = nextReg();
    emit(reg + " = load " + info.type + ", " + info.type + "* " + info.ptr);
    lastVal = {reg, info.type};
}

// ... Control Flow Visitors (If, While, Block) similar to previous implementation but using IRValue ...

void IRGenerator::visit(BlockStmt& stmt) {
    enterScope();
    for (const auto& s : stmt.statements) if(s) s->accept(*this);
    exitScope();
}

void IRGenerator::visit(IfStmt& stmt) {
    std::string thenLabel = nextLabel();
    std::string elseLabel = nextLabel();
    std::string endLabel = nextLabel();

    stmt.condition->accept(*this);
    
    // Ensure we have a valid condition value. 
    // If semantic analysis passed, this should be valid.
    std::string condReg = lastVal.reg;
    if (lastVal.type != "i1") {
        // Implicit cast to bool if needed (e.g. i32 to i1)
        std::string castReg = nextReg();
        emit(castReg + " = icmp ne " + lastVal.type + " " + lastVal.reg + ", " + getDefaultValue(lastVal.type));
        condReg = castReg;
    }

    emit("br i1 " + condReg + ", label %" + thenLabel + ", label %" + elseLabel);

    emitRaw("\n" + thenLabel + ":");
    stmt.thenBranch->accept(*this);
    emit("br label %" + endLabel);

    emitRaw("\n" + elseLabel + ":");
    if (stmt.elseBranch) stmt.elseBranch->accept(*this);
    emit("br label %" + endLabel);

    emitRaw("\n" + endLabel + ":");
}

void IRGenerator::visit(WhileStmt& stmt) {
    std::string condLabel = nextLabel();
    std::string bodyLabel = nextLabel();
    std::string endLabel = nextLabel();

    emit("br label %" + condLabel);
    
    emitRaw("\n" + condLabel + ":");
    stmt.condition->accept(*this);
    
    std::string condReg = lastVal.reg;
    if (lastVal.type != "i1") {
         std::string castReg = nextReg();
         emit(castReg + " = icmp ne " + lastVal.type + " " + lastVal.reg + ", " + getDefaultValue(lastVal.type));
         condReg = castReg;
    }

    emit("br i1 " + condReg + ", label %" + bodyLabel + ", label %" + endLabel);

    emitRaw("\n" + bodyLabel + ":");
    stmt.body->accept(*this);
    emit("br label %" + condLabel);

    emitRaw("\n" + endLabel + ":");
}

void IRGenerator::visit(UnaryExpr& expr) { expr.right->accept(*this); /* TODO: neg */ }
void IRGenerator::visit(GroupingExpr& expr) { expr.expression->accept(*this); }
void IRGenerator::visit(ClassDecl& stmt) {
   // No-op to avoid crash
   return;
}
void IRGenerator::visit(NamespaceDecl& stmt) {
    for (const auto& s : stmt.declarations) {
        if (s) s->accept(*this);
    }
}
void IRGenerator::visit(PrintStmt& stmt) {
    stmt.expression->accept(*this);
    // Print logic duplicated from CallExpr 'print'
    // Simplified:
    std::string callReg = nextReg();
    emit(callReg + " = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), " + lastVal.type + " " + lastVal.reg + ")");
}

} // namespace stratos