"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.ParameterTypes = exports.SyntaxKindType = void 0;
var SyntaxKindType;
(function (SyntaxKindType) {
    SyntaxKindType[SyntaxKindType["VariableDeclaration"] = 1] = "VariableDeclaration";
    SyntaxKindType[SyntaxKindType["TypeAliasDeclaration"] = 2] = "TypeAliasDeclaration";
    SyntaxKindType[SyntaxKindType["ImportDeclaration"] = 3] = "ImportDeclaration";
    SyntaxKindType[SyntaxKindType["ClassDeclaration"] = 4] = "ClassDeclaration";
    SyntaxKindType[SyntaxKindType["ImportClause"] = 5] = "ImportClause";
    SyntaxKindType[SyntaxKindType["Parameter"] = 6] = "Parameter";
    SyntaxKindType[SyntaxKindType["PropertyDeclaration"] = 7] = "PropertyDeclaration";
    SyntaxKindType[SyntaxKindType["BinaryExpression"] = 8] = "BinaryExpression";
    SyntaxKindType[SyntaxKindType["EnumDeclaration"] = 9] = "EnumDeclaration";
    SyntaxKindType[SyntaxKindType["StructDeclaration"] = 10] = "StructDeclaration";
    SyntaxKindType[SyntaxKindType["FunctionDeclaration"] = 11] = "FunctionDeclaration";
    SyntaxKindType[SyntaxKindType["PackageDeclaration"] = 12] = "PackageDeclaration";
    SyntaxKindType[SyntaxKindType["UnknownDeclaration"] = 13] = "UnknownDeclaration";
})(SyntaxKindType = exports.SyntaxKindType || (exports.SyntaxKindType = {}));
class ParameterTypes {
    constructor(name, type) {
        this.name = name;
        this.type = type;
    }
}
exports.ParameterTypes = ParameterTypes;
//# sourceMappingURL=syntax_kind.js.map