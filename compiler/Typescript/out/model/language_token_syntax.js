"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.LanguageTokenSyntaxVariableFunctionDeclarationAndAssignment = exports.LanguageTokenSyntaxVariableDeclarationAndAssignment = exports.LanguageTokenSyntaxVariableDeclaration = exports.LanguageTokenSyntaxImport = exports.LanguageTokenSyntaxPackage = exports.LanguageTokenSyntaxFunctionInvoking = exports.LanguageTokenSyntaxFunctionDeclaration = exports.LanguageTokenSyntaxClass = exports.LanguageTokenSyntax = exports.LanguageTokenSyntaxGroup = void 0;
const syntax_kind_1 = require("./syntax_kind");
class LanguageTokenSyntaxGroup {
    constructor(children, tokenGroup) {
        this.children = children;
        this.tokenGroup = tokenGroup;
    }
}
exports.LanguageTokenSyntaxGroup = LanguageTokenSyntaxGroup;
class LanguageTokenSyntax {
    constructor(type, nodes) {
        this.type = type;
        this.nodes = nodes;
    }
}
exports.LanguageTokenSyntax = LanguageTokenSyntax;
class LanguageTokenSyntaxClass extends LanguageTokenSyntax {
    constructor(name, memberFunctions, constructos, nodes) {
        super(syntax_kind_1.SyntaxKindType.ClassDeclaration, nodes);
        this.name = name;
        this.nodes = nodes;
        this.memberFunctions = memberFunctions;
        this.constructos = constructos;
    }
}
exports.LanguageTokenSyntaxClass = LanguageTokenSyntaxClass;
class LanguageTokenSyntaxFunctionDeclaration extends LanguageTokenSyntax {
    constructor(name, isPrivate, returnType, parameters, nodes) {
        super(syntax_kind_1.SyntaxKindType.FunctionDeclaration, nodes);
        this.nodes = nodes;
        this.name = name;
        this.isPrivate = isPrivate;
        this.returnType = returnType;
        this.parameters = parameters;
    }
}
exports.LanguageTokenSyntaxFunctionDeclaration = LanguageTokenSyntaxFunctionDeclaration;
class LanguageTokenSyntaxFunctionInvoking extends LanguageTokenSyntax {
}
exports.LanguageTokenSyntaxFunctionInvoking = LanguageTokenSyntaxFunctionInvoking;
class LanguageTokenSyntaxPackage extends LanguageTokenSyntax {
    constructor(name, nodes) {
        super(syntax_kind_1.SyntaxKindType.PackageDeclaration, nodes);
        this.name = name;
        this.nodes = nodes;
    }
}
exports.LanguageTokenSyntaxPackage = LanguageTokenSyntaxPackage;
class LanguageTokenSyntaxImport extends LanguageTokenSyntax {
}
exports.LanguageTokenSyntaxImport = LanguageTokenSyntaxImport;
class LanguageTokenSyntaxVariableDeclaration extends LanguageTokenSyntax {
}
exports.LanguageTokenSyntaxVariableDeclaration = LanguageTokenSyntaxVariableDeclaration;
class LanguageTokenSyntaxVariableDeclarationAndAssignment extends LanguageTokenSyntax {
}
exports.LanguageTokenSyntaxVariableDeclarationAndAssignment = LanguageTokenSyntaxVariableDeclarationAndAssignment;
class LanguageTokenSyntaxVariableFunctionDeclarationAndAssignment extends LanguageTokenSyntax {
}
exports.LanguageTokenSyntaxVariableFunctionDeclarationAndAssignment = LanguageTokenSyntaxVariableFunctionDeclarationAndAssignment;
//# sourceMappingURL=language_token_syntax.js.map