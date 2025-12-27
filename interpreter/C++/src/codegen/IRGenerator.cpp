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
    emitRaw("declare i8* @malloc(i64)\n");

    // Define common format specifiers
    emitRaw("; Format specifiers for print");
    emitRaw("@.str_specifier = private unnamed_addr constant [4 x i8] c\"%d\\0A\\00\"");

    // 2. First pass: Collect class metadata
    for (const auto& stmt : statements) {
        if (auto cls = dynamic_cast<ClassDecl*>(stmt.get())) {
            ClassInfo info;
            info.name = cls->name.lexeme;
            info.isInterface = false; // TODO: detect interfaces properly
            if (cls->superclass) {
                info.superclass = cls->superclass->name.lexeme;
            }

            int fieldIdx = 0;
            for (const auto& member : cls->methods) {
                if (auto varDecl = dynamic_cast<VarDecl*>(member.get())) {
                    FieldInfo field;
                    field.name = varDecl->name.lexeme;
                    field.type = varDecl->typeName.empty() ? "int" : varDecl->typeName;
                    field.index = fieldIdx++;
                    info.fields.push_back(field);
                } else if (auto fnDecl = dynamic_cast<FunctionDecl*>(member.get())) {
                    MethodInfo method;
                    method.name = fnDecl->name.lexeme;
                    method.paramTypes = fnDecl->paramTypes;
                    method.returnType = fnDecl->returnType;
                    method.hasBody = (fnDecl->body != nullptr);
                    info.methods.push_back(method);
                }
            }

            classes[cls->name.lexeme] = info;
        }
    }

    // 3. Generate struct type definitions for classes
    generateClassStructs();

    // 4. Generate code
    std::vector<Stmt*> topLevelStmts;

    for (const auto& stmt : statements) {
        if (auto fn = dynamic_cast<FunctionDecl*>(stmt.get())) {
            fn->accept(*this);
        } else if (auto pkg = dynamic_cast<PackageDecl*>(stmt.get())) {
            pkg->accept(*this);
        } else if (auto cls = dynamic_cast<ClassDecl*>(stmt.get())) {
            cls->accept(*this);
        } else {
            topLevelStmts.push_back(stmt.get());
        }
    }

    // 5. Generate Main for top-level code
    if (!topLevelStmts.empty()) {
        generateMainWrapper(topLevelStmts);
    }

    // 6. Emit cached string globals
    emitRaw("\n; String Literals");
    for (const auto& [text, name] : stringLiterals) {
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

    // 2. Determine Callee Name and handle different call types
    std::string funcName;
    IRValue objPtr; // For method calls
    std::string className; // For method calls
    bool isMethodCall = false;

    if (auto var = dynamic_cast<VariableExpr*>(expr.callee.get())) {
        funcName = var->name.lexeme;

        // Check if this is a constructor call (class name as function)
        if (classes.find(funcName) != classes.end()) {
            // Constructor call
            std::stringstream argSS;
            for (size_t i = 0; i < args.size(); ++i) {
                argSS << args[i].type << " " << args[i].reg;
                if (i < args.size() - 1) argSS << ", ";
            }

            std::string resultReg = nextReg();
            std::string retType = getStructType(funcName);
            emit(resultReg + " = call " + retType + " @" + funcName + "(" + argSS.str() + ")");
            lastVal = {resultReg, retType};
            return;
        }
    } else if (auto binExpr = dynamic_cast<BinaryExpr*>(expr.callee.get())) {
        // Could be method call (e.g., obj.method()) or module function call (e.g., math.sqrt())
        if (binExpr->op.type == TokenType::DOT) {
            // Check if left side is a module name (for stdlib calls like math.sqrt())
            if (auto leftVar = dynamic_cast<VariableExpr*>(binExpr->left.get())) {
                std::string leftName = leftVar->name.lexeme;

                // Check if this is a stdlib module
                if (auto rightVar = dynamic_cast<VariableExpr*>(binExpr->right.get())) {
                    std::string rightName = rightVar->name.lexeme;

                    // Check if this is a native function call
                    if (isNativeFunction(leftName, rightName)) {
                        generateNativeCall(leftName, rightName, args);
                        return;
                    }
                }
            }

            // Otherwise, treat as method call
            // Left is object, right is method name
            binExpr->left->accept(*this);
            objPtr = lastVal;

            // Extract class name from object type
            std::string objType = objPtr.type;
            if (objType.find("%struct.") == 0) {
                size_t start = 8;
                size_t end = objType.find("*");
                if (end != std::string::npos) {
                    className = objType.substr(start, end - start);
                }
            }

            if (auto methodVar = dynamic_cast<VariableExpr*>(binExpr->right.get())) {
                funcName = methodVar->name.lexeme;
                isMethodCall = true;
            }
        }
    } else {
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

    } else if (isMethodCall) {
        // Method call
        auto classInfo = getClassInfo(className);
        if (!classInfo) {
            lastVal = {"0", "i32"};
            return;
        }

        // Find method info
        MethodInfo* methodInfo = nullptr;
        for (auto& m : classInfo->methods) {
            if (m.name == funcName) {
                methodInfo = &m;
                break;
            }
        }

        if (!methodInfo) {
            lastVal = {"0", "i32"};
            return;
        }

        // Build method call arguments: this pointer + regular args
        std::stringstream argSS;
        argSS << objPtr.type << " " << objPtr.reg;

        for (size_t i = 0; i < args.size(); ++i) {
            argSS << ", " << args[i].type << " " << args[i].reg;
        }

        std::string retType = getLLVMType(methodInfo->returnType);
        std::string callReg = nextReg();
        emit(callReg + " = call " + retType + " @" + className + "_" + funcName + "(" + argSS.str() + ")");
        lastVal = {callReg, retType};

    } else {
        // Regular user function call
        std::string retType = "i32"; // TODO: Lookup from symbol table

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
    // Handle DOT operator for field access
    if (expr.op.type == TokenType::DOT) {
        // Left side should be an object (variable)
        // Right side should be a field name (variable) or method call (handled in CallExpr)
        expr.left->accept(*this);
        IRValue obj = lastVal;

        // Get the class name from the object type
        // Object type is like "%struct.Rectangle*"
        std::string objType = obj.type;
        std::string className;

        // Extract class name from "%struct.ClassName*"
        if (objType.find("%struct.") == 0) {
            size_t start = 8; // length of "%struct."
            size_t end = objType.find("*");
            if (end != std::string::npos) {
                className = objType.substr(start, end - start);
            }
        }

        // Right side is field name
        if (auto varExpr = dynamic_cast<VariableExpr*>(expr.right.get())) {
            std::string fieldName = varExpr->name.lexeme;
            int fieldIdx = getFieldIndex(className, fieldName);

            if (fieldIdx >= 0) {
                auto classInfo = getClassInfo(className);
                std::string fieldType = getLLVMType(classInfo->fields[fieldIdx].type);

                // Get pointer to field
                std::string fieldPtr = nextReg();
                emit(fieldPtr + " = getelementptr inbounds %struct." + className + ", %struct." + className + "* " + obj.reg + ", i32 0, i32 " + std::to_string(fieldIdx));

                // Load field value
                std::string fieldVal = nextReg();
                emit(fieldVal + " = load " + fieldType + ", " + fieldType + "* " + fieldPtr);

                lastVal = {fieldVal, fieldType};
                return;
            }
        }

        // If we get here, it might be a method call - let CallExpr handle it
        lastVal = obj;
        return;
    }

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
        lastVal = {"0", "i32"};
        return;
    }

    // Check if this is a direct register (like 'this' pointer) or needs loading
    // Direct registers start with % but are not pointers to load from
    // They're passed as function parameters directly
    if (expr.name.lexeme == "this") {
        // 'this' is already a direct pointer, no load needed
        lastVal = {info.ptr, info.type};
        return;
    }

    // Regular variable - need to load from stack pointer
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

void IRGenerator::visit(UnaryExpr& expr) {
    expr.right->accept(*this);
    // TODO: Implement unary operations (negation, logical NOT, etc.)
}

void IRGenerator::visit(GroupingExpr& expr) {
    expr.expression->accept(*this);
}

void IRGenerator::visit(ClassDecl& stmt) {
    // Generate constructors and methods for this class
    currentClass = stmt.name.lexeme;

    // Find and generate constructor
    bool hasExplicitConstructor = false;
    for (const auto& member : stmt.methods) {
        if (auto fnDecl = dynamic_cast<FunctionDecl*>(member.get())) {
            if (fnDecl->name.lexeme == "constructor") {
                generateConstructor(currentClass, stmt);
                hasExplicitConstructor = true;
                break;
            }
        }
    }

    // Generate default constructor if none exists
    if (!hasExplicitConstructor) {
        generateConstructor(currentClass, stmt);
    }

    // Generate methods
    for (const auto& member : stmt.methods) {
        if (auto fnDecl = dynamic_cast<FunctionDecl*>(member.get())) {
            if (fnDecl->name.lexeme != "constructor" && fnDecl->body) {
                generateMethod(currentClass, *fnDecl);
            }
        }
    }

    currentClass = "";
}

void IRGenerator::visit(PackageDecl& stmt) {
    for (const auto& s : stmt.declarations) {
        if (s) s->accept(*this);
    }
}
void IRGenerator::visit(PrintStmt& stmt) {
    stmt.expression->accept(*this);

    // Generate print call based on the expression type
    IRValue arg = lastVal;
    std::string fmt;
    if (arg.type == "double") fmt = "%f\\0A";
    else if (arg.type == "i8*") fmt = "%s\\0A";
    else fmt = "%d\\0A"; // int/bool

    std::string fmtPtr = getOrCreateStringLiteral(fmt);

    // Call printf with the appropriate format
    std::string callReg = nextReg();
    int fmtLen = (arg.type == "double" || arg.type == "i8*") ? 4 : 4; // All are 4 with \n and \0
    emit(callReg + " = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([" + std::to_string(fmtLen) + " x i8], [" + std::to_string(fmtLen) + " x i8]* " + fmtPtr + ", i32 0, i32 0), " + arg.type + " " + arg.reg + ")");
}

// ============================================================================
// OOP Helper Implementations
// ============================================================================

void IRGenerator::generateClassStructs() {
    if (classes.empty()) return;

    emitRaw("\n; Class struct type definitions");
    for (const auto& [className, classInfo] : classes) {
        if (classInfo.isInterface) continue; // Skip interfaces

        std::stringstream ss;
        ss << "%struct." << className << " = type { ";

        for (size_t i = 0; i < classInfo.fields.size(); i++) {
            ss << getLLVMType(classInfo.fields[i].type);
            if (i < classInfo.fields.size() - 1) ss << ", ";
        }

        if (classInfo.fields.empty()) {
            ss << "i8"; // Empty struct needs at least one field
        }

        ss << " }";
        emitRaw(ss.str());
    }
    emitRaw("");
}

std::string IRGenerator::getStructType(const std::string& className) {
    return "%struct." + className + "*";
}

int IRGenerator::getFieldIndex(const std::string& className, const std::string& fieldName) {
    auto it = classes.find(className);
    if (it == classes.end()) return -1;

    for (const auto& field : it->second.fields) {
        if (field.name == fieldName) {
            return field.index;
        }
    }
    return -1;
}

IRGenerator::ClassInfo* IRGenerator::getClassInfo(const std::string& className) {
    auto it = classes.find(className);
    if (it != classes.end()) {
        return &it->second;
    }
    return nullptr;
}

void IRGenerator::generateConstructor(const std::string& className, ClassDecl& classDecl) {
    auto classInfo = getClassInfo(className);
    if (!classInfo) return;

    // Find constructor declaration in class methods
    FunctionDecl* ctorDecl = nullptr;
    for (const auto& member : classDecl.methods) {
        if (auto fn = dynamic_cast<FunctionDecl*>(member.get())) {
            if (fn->name.lexeme == "constructor") {
                ctorDecl = fn;
                break;
            }
        }
    }

    // Build constructor signature
    std::stringstream paramsSS;
    std::vector<Token> params;
    std::vector<std::string> paramTypes;

    if (ctorDecl) {
        params = ctorDecl->params;
        paramTypes = ctorDecl->paramTypes;
        for (size_t i = 0; i < params.size(); i++) {
            paramsSS << getLLVMType(paramTypes[i]) << " %arg" << i;
            if (i < params.size() - 1) paramsSS << ", ";
        }
    }

    // Constructor returns a pointer to the object
    std::string retType = getStructType(className);

    emitRaw("\n; Constructor for " + className);
    emitRaw("define " + retType + " @" + className + "(" + paramsSS.str() + ") {");

    enterScope();

    // 1. Allocate memory for the object
    std::string sizeofStruct = std::to_string(classInfo->fields.size() * 8); // Simplified
    std::string mallocResult = nextReg();
    emit(mallocResult + " = call i8* @malloc(i64 " + sizeofStruct + ")");

    // 2. Bitcast to struct type
    std::string objPtr = nextReg();
    emit(objPtr + " = bitcast i8* " + mallocResult + " to %struct." + className + "*");

    // 3. Store constructor parameters to fields
    if (ctorDecl && ctorDecl->body) {
        // Define 'this' as the object pointer
        defineVar("this", objPtr, retType);

        // Define parameters
        for (size_t i = 0; i < params.size(); i++) {
            std::string type = getLLVMType(paramTypes[i]);
            std::string ptr = "%" + params[i].lexeme + ".addr";
            emit(ptr + " = alloca " + type);
            emit("store " + type + " %arg" + std::to_string(i) + ", " + type + "* " + ptr);
            defineVar(params[i].lexeme, ptr, type);
        }

        // Execute constructor body
        for (const auto& stmt : *ctorDecl->body) {
            if (stmt) stmt->accept(*this);
        }
    } else {
        // Default initialization: set all fields to default values
        for (size_t i = 0; i < classInfo->fields.size(); i++) {
            std::string fieldType = getLLVMType(classInfo->fields[i].type);
            std::string fieldPtr = nextReg();
            emit(fieldPtr + " = getelementptr inbounds %struct." + className + ", %struct." + className + "* " + objPtr + ", i32 0, i32 " + std::to_string(i));
            emit("store " + fieldType + " " + getDefaultValue(fieldType) + ", " + fieldType + "* " + fieldPtr);
        }
    }

    // 4. Return the object pointer
    emit("ret " + retType + " " + objPtr);

    exitScope();
    emitRaw("}\n");
}

void IRGenerator::generateMethod(const std::string& className, FunctionDecl& method) {
    regCount = 0;

    std::string retType = getLLVMType(method.returnType);
    std::string name = "@" + className + "_" + method.name.lexeme;

    // Method signature: first parameter is always 'this' pointer
    std::stringstream paramsSS;
    paramsSS << getStructType(className) << " %this_ptr";

    for (size_t i = 0; i < method.params.size(); ++i) {
        paramsSS << ", ";
        std::string type = getLLVMType(method.paramTypes[i]);
        paramsSS << type << " %arg" << i;
    }

    emitRaw("\n; Method " + className + "." + method.name.lexeme);
    emitRaw("define " + retType + " " + name + "(" + paramsSS.str() + ") {");
    enterScope();

    // Define 'this' in the scope - IMPORTANT: Don't treat as alloca'd variable
    // The this pointer is passed directly as a parameter, no need to load it
    VarInfo thisInfo;
    thisInfo.ptr = "%this_ptr"; // Direct register, not a stack pointer
    thisInfo.type = getStructType(className);
    scopes.back()["this"] = thisInfo;

    // Define parameters
    for (size_t i = 0; i < method.params.size(); ++i) {
        std::string paramName = method.params[i].lexeme;
        std::string type = getLLVMType(method.paramTypes[i]);
        std::string ptr = "%" + paramName + ".addr";

        emit(ptr + " = alloca " + type);
        emit("store " + type + " %arg" + std::to_string(i) + ", " + type + "* " + ptr);

        defineVar(paramName, ptr, type);
    }

    // Generate method body
    if (method.body) {
        for (const auto& s : *method.body) {
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

// ============================================================================
// Native Function Support
// ============================================================================

bool IRGenerator::isNativeFunction(const std::string& moduleName, const std::string& functionName) {
    return NativeRegistry::getInstance().isNative(moduleName, functionName);
}

void IRGenerator::generateNativeCall(const std::string& moduleName, const std::string& functionName,
                                    const std::vector<IRValue>& args) {
    // For native functions, we generate a call to a runtime wrapper
    // The wrapper will call NativeRegistry to execute the actual C++ function

    // Declare native function wrapper if not already declared
    std::string qualifiedName = moduleName + "_" + functionName;
    std::string wrapperName = "@__native_" + qualifiedName;

    // Determine return type based on function
    std::string retType = "i32"; // Default, should be looked up from function signature

    // For known stdlib functions, we can hardcode return types
    if (moduleName == "math") {
        if (functionName == "sin" || functionName == "cos" || functionName == "tan" ||
            functionName == "sqrt" || functionName == "exp" || functionName == "log" ||
            functionName == "pow" || functionName == "abs") {
            retType = "double";
        } else if (functionName == "randomInt" || functionName == "sign") {
            retType = "i32";
        } else if (functionName == "random") {
            retType = "double";
        }
    } else if (moduleName == "strings") {
        if (functionName == "toUpper" || functionName == "toLower" || functionName == "trim" ||
            functionName == "substring" || functionName == "repeat") {
            retType = "i8*"; // string
        } else if (functionName == "length" || functionName == "indexOf") {
            retType = "i32";
        } else if (functionName == "contains" || functionName == "isEmpty" ||
                   functionName == "startsWith" || functionName == "endsWith") {
            retType = "i1"; // bool
        }
    } else if (moduleName == "io") {
        if (functionName == "readFile" || functionName == "absolute" || functionName == "basename") {
            retType = "i8*"; // string
        } else if (functionName == "exists" || functionName == "isFile" || functionName == "isDirectory") {
            retType = "i1"; // bool
        } else if (functionName == "fileSize") {
            retType = "i32";
        }
    } else if (moduleName == "log") {
        retType = "void";
    } else if (moduleName == "time") {
        if (functionName == "now" || functionName == "unix") {
            retType = "i64"; // timestamp
        } else if (functionName == "sleep") {
            retType = "void";
        }
    }

    // Build argument list
    std::stringstream argSS;
    for (size_t i = 0; i < args.size(); ++i) {
        argSS << args[i].type << " " << args[i].reg;
        if (i < args.size() - 1) argSS << ", ";
    }

    // Generate the call
    if (retType == "void") {
        emit("call " + retType + " " + wrapperName + "(" + argSS.str() + ")");
        lastVal = {"", "void"};
    } else {
        std::string callReg = nextReg();
        emit(callReg + " = call " + retType + " " + wrapperName + "(" + argSS.str() + ")");
        lastVal = {callReg, retType};
    }
}

} // namespace stratos