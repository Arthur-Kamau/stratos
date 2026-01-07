#include "stratos/Formatter.h"
#include <iostream>

namespace stratos {

Formatter::Formatter() : indentLevel_(0), needsBlankLine_(false) {}

std::string Formatter::format(const std::vector<std::unique_ptr<Stmt>>& statements) {
    output_.str("");
    output_.clear();
    indentLevel_ = 0;
    needsBlankLine_ = false;

    for (size_t i = 0; i < statements.size(); i++) {
        if (needsBlankLine_ && i > 0) {
            newline();
        }
        statements[i]->accept(*this);
        needsBlankLine_ = true;
    }

    return output_.str();
}

void Formatter::indent() {
    for (int i = 0; i < indentLevel_; i++) {
        output_ << "    ";
    }
}

void Formatter::newline() {
    output_ << "\n";
}

void Formatter::space() {
    output_ << " ";
}

void Formatter::write(const std::string& text) {
    output_ << text;
}

void Formatter::formatBlock(const std::vector<std::unique_ptr<Stmt>>& statements, bool addBraces) {
    if (addBraces) {
        write("{");
        newline();
    }

    indentLevel_++;
    for (const auto& stmt : statements) {
        indent();
        stmt->accept(*this);
    }
    indentLevel_--;

    if (addBraces) {
        indent();
        write("}");
    }
}

void Formatter::formatArguments(const std::vector<std::unique_ptr<Expr>>& args) {
    for (size_t i = 0; i < args.size(); i++) {
        args[i]->accept(*this);
        if (i < args.size() - 1) {
            write(",");
            space();
        }
    }
}

// Expression visitors
void Formatter::visit(BinaryExpr& expr) {
    expr.left->accept(*this);
    space();
    write(expr.op.lexeme);
    space();
    expr.right->accept(*this);
}

void Formatter::visit(UnaryExpr& expr) {
    write(expr.op.lexeme);
    expr.right->accept(*this);
}

void Formatter::visit(LiteralExpr& expr) {
    if (expr.type == TokenType::STRING) {
        write("\"");
        write(expr.value);
        write("\"");
    } else {
        write(expr.value);
    }
}

void Formatter::visit(VariableExpr& expr) {
    write(expr.name.lexeme);
}

void Formatter::visit(CallExpr& expr) {
    expr.callee->accept(*this);
    write("(");
    formatArguments(expr.arguments);
    write(")");
}

void Formatter::visit(IndexExpr& expr) {
    expr.object->accept(*this);
    write("[");
    expr.index->accept(*this);
    write("]");
}

void Formatter::visit(GroupingExpr& expr) {
    write("(");
    expr.expression->accept(*this);
    write(")");
}

void Formatter::visit(CastExpr& expr) {
    expr.expression->accept(*this);
    space();
    write("as");
    if (expr.isSafe) {
        write("?");
    }
    space();
    write(expr.typeToken.lexeme);
}

// Statement visitors
void Formatter::visit(VarDecl& stmt) {
    write(stmt.isMutable ? "var" : "val");
    space();
    write(stmt.name.lexeme);

    if (!stmt.typeName.empty()) {
        write(":");
        space();
        write(stmt.typeName);
    }

    if (stmt.initializer) {
        space();
        write("=");
        space();
        stmt.initializer->accept(*this);
    }

    write(";");
    newline();
}

void Formatter::visit(FunctionDecl& stmt) {
    write("fn");
    space();
    write(stmt.name.lexeme);
    write("(");

    // Format parameters (they're Token objects)
    for (size_t i = 0; i < stmt.params.size(); i++) {
        write(stmt.params[i].lexeme);
        if (i < stmt.paramTypes.size() && !stmt.paramTypes[i].empty()) {
            write(":");
            space();
            write(stmt.paramTypes[i]);
        }
        if (i < stmt.params.size() - 1) {
            write(",");
            space();
        }
    }

    write(")");

    if (!stmt.returnType.empty() && stmt.returnType != "void") {
        space();
        write(stmt.returnType);
    }

    space();

    // Body is a unique_ptr<vector>, so dereference it
    if (stmt.body) {
        formatBlock(*stmt.body);
    } else {
        write("{}");
    }

    newline();
}

void Formatter::visit(ClassDecl& stmt) {
    write("class");
    space();
    write(stmt.name.lexeme);

    if (stmt.superclass) {
        space();
        write("extends");
        space();
        stmt.superclass->accept(*this);
    }

    space();
    write("{");
    newline();

    indentLevel_++;
    for (const auto& method : stmt.methods) {
        indent();
        method->accept(*this);
    }
    indentLevel_--;

    indent();
    write("}");
    newline();
}

void Formatter::visit(EnumDecl& stmt) {
    write("enum");
    space();
    write(stmt.name.lexeme);
    space();
    write("{");
    newline();

    indentLevel_++;
    for (size_t i = 0; i < stmt.values.size(); i++) {
        indent();
        write(stmt.values[i].lexeme);
        if (i < stmt.values.size() - 1) {
            write(",");
        }
        newline();
    }
    indentLevel_--;

    indent();
    write("}");
    newline();
}

void Formatter::visit(PackageDecl& stmt) {
    write("package");
    space();
    write(stmt.name.lexeme);
    write(";");
    newline();
}

void Formatter::visit(UseStmt& stmt) {
    write("use");
    space();
    write(stmt.moduleName.lexeme);
    write(";");
    newline();
}

void Formatter::visit(BlockStmt& stmt) {
    formatBlock(stmt.statements);
}

void Formatter::visit(ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
    write(";");
    newline();
}

void Formatter::visit(PrintStmt& stmt) {
    write("println(");
    stmt.expression->accept(*this);
    write(");");
    newline();
}

void Formatter::visit(IfStmt& stmt) {
    write("if");
    space();
    write("(");
    stmt.condition->accept(*this);
    write(")");
    space();
    stmt.thenBranch->accept(*this);

    if (stmt.elseBranch) {
        space();
        write("else");
        space();
        stmt.elseBranch->accept(*this);
    }
    newline();
}

void Formatter::visit(WhileStmt& stmt) {
    write("while");
    space();
    write("(");
    stmt.condition->accept(*this);
    write(")");
    space();
    stmt.body->accept(*this);
    newline();
}

void Formatter::visit(ForStmt& stmt) {
    write("for");
    space();
    write(stmt.isMutable ? "var" : "val");
    space();
    write(stmt.variable.lexeme);
    if (!stmt.varType.empty()) {
        write(":");
        space();
        write(stmt.varType);
    }
    space();
    write("in");
    space();
    stmt.iterable->accept(*this);
    space();
    stmt.body->accept(*this);
    newline();
}

void Formatter::visit(ReturnStmt& stmt) {
    write("return");
    if (stmt.value) {
        space();
        stmt.value->accept(*this);
    }
    write(";");
    newline();
}

} // namespace stratos
