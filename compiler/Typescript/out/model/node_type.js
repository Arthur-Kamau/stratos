"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.NodeType = void 0;
var NodeType;
(function (NodeType) {
    NodeType[NodeType["IntTypeNode"] = 1] = "IntTypeNode";
    NodeType[NodeType["DoubleTypeNode"] = 2] = "DoubleTypeNode";
    NodeType[NodeType["StringTypeNode"] = 3] = "StringTypeNode";
    NodeType[NodeType["CharTypeNode"] = 4] = "CharTypeNode";
    NodeType[NodeType["WhenNode"] = 5] = "WhenNode";
    NodeType[NodeType["IfNode"] = 6] = "IfNode";
    NodeType[NodeType["ElseNode"] = 7] = "ElseNode";
    NodeType[NodeType["PackageNode"] = 8] = "PackageNode";
    NodeType[NodeType["ImportNode"] = 9] = "ImportNode";
    NodeType[NodeType["EnumNode"] = 10] = "EnumNode";
    NodeType[NodeType["StructNode"] = 11] = "StructNode";
    NodeType[NodeType["ClassNode"] = 12] = "ClassNode";
    NodeType[NodeType["FunctionNode"] = 13] = "FunctionNode";
    NodeType[NodeType["PrivateNode"] = 14] = "PrivateNode";
    NodeType[NodeType["ValNode"] = 15] = "ValNode";
    NodeType[NodeType["VarNode"] = 16] = "VarNode";
    NodeType[NodeType["LetNode"] = 17] = "LetNode";
    NodeType[NodeType["CharArrayNode"] = 18] = "CharArrayNode";
    NodeType[NodeType["UnknownKeyWordNode"] = 19] = "UnknownKeyWordNode";
    NodeType[NodeType["UnknownSignNodeNode"] = 20] = "UnknownSignNodeNode";
    // math symobols
    NodeType[NodeType["ModulusNode"] = 21] = "ModulusNode";
    NodeType[NodeType["AddNode"] = 22] = "AddNode";
    NodeType[NodeType["MinusNode"] = 23] = "MinusNode";
    NodeType[NodeType["DivideNode"] = 24] = "DivideNode";
    NodeType[NodeType["SubtractNode"] = 25] = "SubtractNode";
    NodeType[NodeType["MultiplyNode"] = 26] = "MultiplyNode";
    // space Node
    NodeType[NodeType["SpaceNode"] = 27] = "SpaceNode";
    // unif signs
    NodeType[NodeType["SquareBracketOpenNode"] = 28] = "SquareBracketOpenNode";
    NodeType[NodeType["SquareBracketCloseNode"] = 29] = "SquareBracketCloseNode";
    NodeType[NodeType["CurvedBracketOpenNode"] = 30] = "CurvedBracketOpenNode";
    NodeType[NodeType["CurvedBracketCloseNode"] = 31] = "CurvedBracketCloseNode";
    NodeType[NodeType["CurlyBracketOpenNode"] = 32] = "CurlyBracketOpenNode";
    NodeType[NodeType["CurlyBracketCloseNode"] = 33] = "CurlyBracketCloseNode";
    NodeType[NodeType["SemiColonNode"] = 34] = "SemiColonNode";
    NodeType[NodeType["ColonNode"] = 35] = "ColonNode";
    // comment
    NodeType[NodeType["LineComment"] = 36] = "LineComment";
    NodeType[NodeType["MultiLineComment"] = 37] = "MultiLineComment";
    // stirn 
    NodeType[NodeType["QuotationNode"] = 38] = "QuotationNode";
    //new 
    NodeType[NodeType["NewLine"] = 39] = "NewLine";
})(NodeType = exports.NodeType || (exports.NodeType = {}));
//# sourceMappingURL=node_type.js.map