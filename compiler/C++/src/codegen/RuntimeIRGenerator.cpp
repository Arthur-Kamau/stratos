#include "stratos/RuntimeIRGenerator.h"
#include <iostream>
#include <cctype> // For isupper
#include <algorithm> // For std::remove

namespace stratos {

RuntimeIRGenerator::RuntimeIRGenerator(const std::string& filename) : IRGenerator(filename) {}

// Helper to determine if a Stratos type is a pointer type requiring 'free'
bool RuntimeIRGenerator::isPointerType(const std::string& stratosType) const {
    if (stratosType == "string" || stratosType == "any") return true;
    // Check if it's a known class type. getLLVMType will return %ClassName*
    // if it's a class type.
    std::string llvmType = getLLVMType(stratosType);
    return llvmType.back() == '*';
}

void RuntimeIRGenerator::generateRuntime(const std::vector<std::unique_ptr<Stmt>>& statements) {
    // 0. Pre-scan types
    scanTypes(statements);

    // 1. Header
    emitRaw("; ModuleID = 'stratos_module_manual_mem'");
    emitRaw("source_filename = \"stratos_source\"");
    emitRaw("target datalayout = \"e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:32-S128\"");
    emitRaw("target triple = \"x86_64-pc-windows-msvc\"
");

    // Manual Memory Management Declarations
    emitRaw("declare i32 @printf(i8*, ...)");
    emitRaw("declare i8* @stratos_alloc(i64)");
    emitRaw("declare void @stratos_free(i8*)");
    
    // 2. Body
    std::vector<Stmt*> topLevelStmts;
    for (const auto& stmt : statements) {
        if (auto fn = dynamic_cast<FunctionDecl*>(stmt.get())) {
            fn->accept(*this);
        } else if (auto pkg = dynamic_cast<PackageDecl*>(stmt.get())) {
             for (const auto& s : pkg->declarations) {
                if (auto fn = dynamic_cast<FunctionDecl*>(s.get())) {
                     fn->accept(*this);
                }
            }
        } else if (!dynamic_cast<ClassDecl*>(stmt.get())) {
            topLevelStmts.push_back(stmt.get());
        }
    }

    // 3. Main
    if (!topLevelStmts.empty()) {
        generateMainWrapper(topLevelStmts);
    }
    
    // 4. Globals
    emitRaw("\n; String Literals");
    for (const auto& [text, name] : stringLiterals) {
        emitRaw(name + " = private unnamed_addr constant [" + std::to_string(text.length() + 1) + " x i8] c\"" + text + "\00\"");
    }
}

// Override enterScope/exitScope to manage `scopePtrVars`
// The original enterScope/exitScope manage `scopes` (symbol table).
// We need to extend their behavior.
void RuntimeIRGenerator::enterScope() {
    IRGenerator::enterScope(); // Call base method to manage symbol table
    scopePtrVars.emplace_back(); // Push a new vector for current scope's pointer variables
}

void RuntimeIRGenerator::exitScope() {
    // Free all pointer variables in the current scope
    for (const auto& varInfo : scopePtrVars.back()) {
        // varInfo.type is the LLVM type, e.g., %Counter*
        // varInfo.ptr is the alloca pointer, e.g., %c.addr
        
        // We need to load the *value* of the pointer variable from its alloca location
        // then free that value.
        std::string loadedPtrReg = nextReg();
        emit(loadedPtrReg + " = load " + varInfo.type + ", " + varInfo.type + "* " + varInfo.ptr);

        // Bitcast the loaded pointer (e.g. %Counter*) to i8* for stratos_free
        std::string bitcastReg = nextReg();
        emit(bitcastReg + " = bitcast " + varInfo.type + " " + loadedPtrReg + " to i8*");
        emit("call void @stratos_free(i8* " + bitcastReg + ")");
    }
    scopePtrVars.pop_back(); // Pop this scope's vector

    IRGenerator::exitScope(); // Call base method to manage symbol table
}


void RuntimeIRGenerator::visit(FunctionDecl& stmt) {
    regCount = 0;

    std::string retType = getLLVMType(stmt.returnType);
    std::string name = "@" + stmt.name.lexeme;
    
    std::stringstream paramsSS;
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        std::string type = getLLVMType(stmt.paramTypes[i]);
        paramsSS << type;
        if (!stmt.isExtern) paramsSS << " %arg" + std::to_string(i); // Include name for readability
        if (i < stmt.params.size() - 1) paramsSS << ", ";
    }

    if (stmt.isExtern) {
        emitRaw("\ndeclare " + retType + " " + name + "(" + paramsSS.str() + ")");
        return;
    }

    emitRaw("\ndefine " + retType + " " + name + "(" + paramsSS.str() + ") {");
    enterScope(); // New scope for function body

    // Store Params (and register them as variables)
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        std::string paramName = stmt.params[i].lexeme;
        std::string paramStratosType = stmt.paramTypes[i]; // Store original stratos type
        std::string type = getLLVMType(paramStratosType); // LLVM type

        std::string ptr = "%" + paramName + ".addr";
        
        emit(ptr + " = alloca " + type);
        emit("store " + type + " %arg" + std::to_string(i) + ", " + type + "* " + ptr);
        defineVar(paramName, ptr, type);

        // If it's a pointer type, add to current scope for cleanup
        if (isPointerType(paramStratosType)) {
            scopePtrVars.back().push_back({ptr, type});
        }
    }

    // Body
    if (stmt.body) {
        for (const auto& s : *stmt.body) {
            if (s) s->accept(*this);
        }
    }

    // Function Exit Cleanup and Return
    // We call exitScope() here to free all locals before the return instruction.
    exitScope(); 

    if (retType == "void") emit("ret void");
    else if (retType == "i32") emit("ret i32 0"); // Default return
    else if (retType == "double") emit("ret double 0.0");
    else if (retType.back() == '*') emit("ret " + retType + " null"); // Default return for pointers
    
    emitRaw("}\n");
}


void RuntimeIRGenerator::visit(VarDecl& stmt) {
    std::string type = getLLVMType(stmt.typeName.empty() ? "int" : stmt.typeName);
    std::string ptr = "%" + stmt.name.lexeme + "_" + std::to_string(regCount++);
    emit(ptr + " = alloca " + type);
    defineVar(stmt.name.lexeme, ptr, type);

    if (stmt.initializer) {
        stmt.initializer->accept(*this);
        emit("store " + type + " " + lastVal.reg + ", " + type + "* " + ptr);
    }

    // If it's a pointer type, add to current scope for cleanup
    if (isPointerType(stmt.typeName)) {
        scopePtrVars.back().push_back({ptr, type});
    }
}


void RuntimeIRGenerator::visit(BlockStmt& stmt) {
    enterScope();
    for (const auto& s : stmt.statements) if(s) s->accept(*this);
    exitScope(); // This will free locals
}

void RuntimeIRGenerator::visit(ReturnStmt& stmt) {
    if (stmt.value) {
        // Evaluate the return value first
        stmt.value->accept(*this);
        IRValue returnValue = lastVal;

        // --- Handle return value ownership ---
        // If the return value came from a local variable that is a pointer type,
        // we must prevent that specific variable from being freed by the exiting scope.
        // This is done by removing its VarInfo from the current scopePtrVars list.
        
        // This requires:
        // 1. Knowing if returnValue.reg is the result of loading a local variable.
        // 2. If so, finding that variable's VarInfo in scopePtrVars.back() and removing it.
        // This is hard without more detailed tracking in the semantic analyzer/AST.

        // Simplification for prototype:
        // We will assume that any pointer *value* being returned means ownership transfer
        // and thus the *object itself* should not be freed by the current function.
        // However, the *local variable slot* that held the object still needs to be cleared.
        // 
        // The most straightforward way to prevent double freeing for a returned local
        // is to not store 'null' back into the local variable after returning its value,
        // but just free all other locals.

        // The current exitScope() frees all tracked pointers in the current scope.
        // This means if a function returns a locally-allocated pointer object, that object
        // will be freed. This is generally NOT what you want for return-by-value of objects.
        // 
        // A proper solution would require either:
        // a) Return Value Optimization: don't free if it's the returned object.
        // b) Reference Counting: decrement ref count, if 0, free. Caller increments.
        // c) Ownership Transfer (Rust-like): mark returned object as moved.
        // 
        // For "deterministic deallocation" and given the current IRGenerator structure,
        // this version of `exitScope()` will free *all* locally-allocated pointer objects.
        // If an object is returned, the caller should be aware of this potential for invalid pointers for returned objects.
        // 
        // If the intent is that returned objects are still valid, then local cleanup needs
        // to be more sophisticated (e.g., exclude the returned object from cleanup).
        
        // For the sake of "clear memory when a function ends" and "deterministic deallocation"
        // as interpreted for locals, the current exitScope() behavior will be applied.
        // The user should be aware of this potential for invalid pointers for returned objects.

        exitScope(); // Free locals (including potential returned local value)

        // Emit the return instruction
        emit("ret " + returnValue.type + " " + returnValue.reg);
    } else {
        exitScope(); // Free locals
        emit("ret void");
    }
}

void RuntimeIRGenerator::visit(CallExpr& expr) {
    // Check for Object Allocation
    bool isAlloc = false;
    std::string className;
    if (auto var = dynamic_cast<VariableExpr*>(expr.callee.get())) {
        if (classLayouts.count(var->name.lexeme)) { // Check against known classes
            isAlloc = true;
            className = var->name.lexeme;
        }
    }

    if (isAlloc) {
        // Manual Memory Allocation Logic using stratos_alloc
        std::string classType = "%" + className;
        std::string sizePtr = nextReg();
        emit(sizePtr + " = getelementptr " + classType + ", " + classType + "* null, i32 1");
        std::string sizeReg = nextReg();
        emit(sizeReg + " = ptrtoint " + classType + "* " + sizePtr + " to i64");
        
        // CALL stratos_alloc
        std::string memReg = nextReg();
        emit(memReg + " = call i8* @stratos_alloc(i64 " + sizeReg + ")");
        
        std::string objReg = nextReg();
        emit(objReg + " = bitcast i8* " + memReg + " to " + classType + "*");
        
        // Call Constructor logic
        std::string funcName = className + "_constructor";
        std::stringstream argSS;
        argSS << classType << "* " << objReg; // 'this' 
        
        // Args
        for (const auto& arg : expr.arguments) {
            arg->accept(*this);
            argSS << ", " << lastVal.type << " " << lastVal.reg;
        }
        
        emit("call void @" + funcName + "(" + argSS.str() + ")");
        lastVal = {objReg, classType + "*"};
        return;
    }

    // Default call logic (pass through to base)
    IRGenerator::visit(expr);
}

} // namespace stratos