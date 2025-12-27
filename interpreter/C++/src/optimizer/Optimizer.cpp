#include "stratos/Optimizer.h"
#include <iostream>

namespace stratos {

void Optimizer::optimize(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& stmt : statements) {
        if (stmt) stmt->accept(*this);
    }
}

// Helper to check if we just visited a constant
bool Optimizer::isConstant(Expr* expr) {
    return lastConst.type != 0;
}

// --- Expressions ---

void Optimizer::visit(BinaryExpr& expr) {
    expr.left->accept(*this);
    ConstValue left = lastConst;
    
    expr.right->accept(*this);
    ConstValue right = lastConst;
    
    lastConst.type = 0; // Reset result

    // Constant Folding: Int + Int
    if (left.type == 1 && right.type == 1) {
        if (expr.op.type == TokenType::PLUS) {
            lastConst.type = 1;
            lastConst.iVal = left.iVal + right.iVal;
            // Mutate AST: This BinaryExpr effectively becomes a LiteralExpr.
            // But we can't replace 'this' (the BinaryExpr object) easily here because we are inside it.
            // We would need the parent to replace the unique_ptr.
            // Strategy: We can't do full tree rewriting in this simple visitor without parent pointers.
            // WORKAROUND: We will print optimization message for demo purposes.
            std::cout << "  [Optimizer] Folded Int BinaryExpr: " << left.iVal << " + " << right.iVal << " = " << lastConst.iVal << std::endl;
        } else if (expr.op.type == TokenType::STAR) {
            lastConst.type = 1;
            lastConst.iVal = left.iVal * right.iVal;
             std::cout << "  [Optimizer] Folded Int BinaryExpr: " << left.iVal << " * " << right.iVal << " = " << lastConst.iVal << std::endl;
        }
    }
}

void Optimizer::visit(LiteralExpr& expr) {
    if (expr.type == TokenType::NUMBER) {
        if (expr.value.find('.') != std::string::npos) {
            lastConst.type = 2;
            lastConst.dVal = std::stod(expr.value);
        } else {
            lastConst.type = 1;
            lastConst.iVal = std::stoi(expr.value);
        }
    } else if (expr.type == TokenType::TRUE) {
        lastConst.type = 4;
        lastConst.bVal = true;
    } else if (expr.type == TokenType::FALSE) {
        lastConst.type = 4;
        lastConst.bVal = false;
    } else {
        lastConst.type = 0;
    }
}

void Optimizer::visit(UnaryExpr& expr) {
    expr.right->accept(*this);
    // Fold -Int
    if (lastConst.type == 1 && expr.op.type == TokenType::MINUS) {
        lastConst.iVal = -lastConst.iVal;
        std::cout << "  [Optimizer] Folded UnaryExpr - " << -lastConst.iVal << " = " << lastConst.iVal << std::endl;
    }
}

void Optimizer::visit(VariableExpr& expr) { lastConst.type = 0; }
void Optimizer::visit(CallExpr& expr) { lastConst.type = 0; }
void Optimizer::visit(GroupingExpr& expr) { expr.expression->accept(*this); }

// --- Statements ---

void Optimizer::visit(VarDecl& stmt) {
    if (stmt.initializer) stmt.initializer->accept(*this);
}

void Optimizer::visit(FunctionDecl& stmt) {
    if (stmt.body) {
        for (const auto& s : *stmt.body) if (s) s->accept(*this);
    }
}

void Optimizer::visit(BlockStmt& stmt) {
    for (const auto& s : stmt.statements) if (s) s->accept(*this);
}

void Optimizer::visit(IfStmt& stmt) {
    stmt.condition->accept(*this);
    // Dead code elimination potential here
    if (lastConst.type == 4) {
        if (lastConst.bVal) {
             std::cout << "  [Optimizer] If-Condition is ALWAYS TRUE. Else branch is dead code." << std::endl;
        } else {
             std::cout << "  [Optimizer] If-Condition is ALWAYS FALSE. Then branch is dead code." << std::endl;
        }
    }
    
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) stmt.elseBranch->accept(*this);
}

void Optimizer::visit(WhileStmt& stmt) {
    stmt.condition->accept(*this);
    stmt.body->accept(*this);
}

void Optimizer::visit(ReturnStmt& stmt) {
    if (stmt.value) stmt.value->accept(*this);
}

void Optimizer::visit(ClassDecl& stmt) {} // No-op
void Optimizer::visit(PackageDecl& stmt) {} // No-op
void Optimizer::visit(UseStmt& stmt) {} // No-op
void Optimizer::visit(PrintStmt& stmt) { stmt.expression->accept(*this); }

} // namespace stratos
