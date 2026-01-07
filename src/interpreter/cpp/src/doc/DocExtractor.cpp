#include "stratos/DocExtractor.h"
#include <iostream>

namespace stratos {

DocExtractor::DocExtractor(const std::string& sourceFile)
    : sourceFile_(sourceFile), currentPackage_(nullptr), currentClass_(nullptr) {
    documentation_ = std::make_unique<Documentation>();
}

std::unique_ptr<Documentation> DocExtractor::extract(
    const std::vector<std::unique_ptr<Stmt>>& statements) {

    ensureDefaultPackage();

    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->accept(*this);
        }
    }

    return std::move(documentation_);
}

void DocExtractor::ensureDefaultPackage() {
    if (documentation_->packages.empty()) {
        auto pkg = std::make_unique<DocPackage>();
        pkg->name = "main";
        pkg->sourceFile = sourceFile_;
        currentPackage_ = pkg.get();
        documentation_->packages.push_back(std::move(pkg));
    }
}

void DocExtractor::visit(PackageDecl& stmt) {
    // Remove default "main" package if it exists and is empty
    // This prevents generating an empty "main" package for files that have explicit package declarations
    if (!documentation_->packages.empty()) {
        DocPackage* first = documentation_->packages.front().get();
        if (first->name == "main" && 
            first->functions.empty() && 
            first->classes.empty() && 
            first->variables.empty() &&
            first->dependencies.empty()) {
            
            documentation_->packages.clear();
        }
    }

    auto pkg = std::make_unique<DocPackage>();
    pkg->name = stmt.name.lexeme;
    pkg->sourceFile = sourceFile_;
    pkg->sourceLine = stmt.name.line;

    if (stmt.documentation) {
        pkg->doc = *stmt.documentation;
    }

    currentPackage_ = pkg.get();
    documentation_->packages.push_back(std::move(pkg));

    // Process package contents
    for (const auto& decl : stmt.declarations) {
        decl->accept(*this);
    }
}

void DocExtractor::visit(FunctionDecl& stmt) {
    auto func = std::make_unique<DocFunction>();
    func->name = stmt.name.lexeme;
    func->returnType = stmt.returnType;
    func->sourceFile = sourceFile_;
    func->sourceLine = stmt.name.line;
    func->isNative = (stmt.body == nullptr);

    // Extract parameters
    for (size_t i = 0; i < stmt.params.size(); i++) {
        func->paramNames.push_back(stmt.params[i].lexeme);
        if (i < stmt.paramTypes.size()) {
            func->paramTypes.push_back(stmt.paramTypes[i]);
        }
    }

    // Attach documentation
    if (stmt.documentation) {
        func->doc = *stmt.documentation;
    }

    // Add to current context
    if (currentClass_) {
        currentClass_->methods.push_back(std::move(func));
    } else if (currentPackage_) {
        currentPackage_->functions.push_back(std::move(func));
    }
}

void DocExtractor::visit(ClassDecl& stmt) {
    auto cls = std::make_unique<DocClass>();
    cls->name = stmt.name.lexeme;
    cls->sourceFile = sourceFile_;
    cls->sourceLine = stmt.name.line;

    if (stmt.superclass) {
        cls->superclass = stmt.superclass->name.lexeme;
    }

    if (stmt.documentation) {
        cls->doc = *stmt.documentation;
    }

    // Process methods
    DocClass* prevClass = currentClass_;
    currentClass_ = cls.get();

    for (const auto& method : stmt.methods) {
        method->accept(*this);
    }

    currentClass_ = prevClass;

    if (currentPackage_) {
        currentPackage_->classes.push_back(std::move(cls));
    }
}

void DocExtractor::visit(EnumDecl& stmt) {
    // TODO: Extract enum documentation
    // For now, just register that the enum exists
}

void DocExtractor::visit(VarDecl& stmt) {
    auto var = std::make_unique<DocVariable>();
    var->name = stmt.name.lexeme;
    var->type = stmt.typeName;
    var->isMutable = stmt.isMutable;
    var->sourceFile = sourceFile_;
    var->sourceLine = stmt.name.line;

    if (stmt.documentation) {
        var->doc = *stmt.documentation;
    }

    if (currentClass_) {
        currentClass_->properties.push_back(std::move(var));
    } else if (currentPackage_) {
        currentPackage_->variables.push_back(std::move(var));
    }
}

void DocExtractor::visit(UseStmt& stmt) {
    if (currentPackage_) {
        currentPackage_->dependencies.push_back(stmt.moduleName.lexeme);
    }
}

} // namespace stratos
