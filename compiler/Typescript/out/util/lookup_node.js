"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.getCharNodeType = exports.isKeyWord = exports.isUniqueSign = exports.getKeyWordNodeType = exports.getkeySignsNodeType = void 0;
const key_words_1 = require("../keyWords/key_words");
const node_type_1 = require("../model/node_type");
function getkeySignsNodeType(word) {
    if (word == '*') {
        return node_type_1.NodeType.MultiplyNode;
    }
    else if (word == '/') {
        return node_type_1.NodeType.DivideNode;
    }
    else if (word == '+') {
        return node_type_1.NodeType.AddNode;
    }
    else if (word == '*') {
        return node_type_1.NodeType.MultiplyNode;
    }
    else if (word == '%') {
        return node_type_1.NodeType.ModulusNode;
    }
    else if (word == ':') {
        return node_type_1.NodeType.ColonNode;
    }
    else if (word == ';') {
        return node_type_1.NodeType.SemiColonNode;
    }
    else if (word = '"') {
        return node_type_1.NodeType.QuotationNode;
    }
    else if (word = '(') {
        return node_type_1.NodeType.CurvedBracketOpenNode;
    }
    else if (word = ')') {
        return node_type_1.NodeType.CurvedBracketCloseNode;
    }
    else if (word = '{') {
        return node_type_1.NodeType.CurlyBracketOpenNode;
    }
    else if (word = '}') {
        return node_type_1.NodeType.CurlyBracketCloseNode;
    }
    else if (word = '[') {
        return node_type_1.NodeType.SquareBracketOpenNode;
    }
    else if (word = ']') {
        return node_type_1.NodeType.SquareBracketCloseNode;
    }
    else {
        return node_type_1.NodeType.UnknownSignNodeNode;
    }
}
exports.getkeySignsNodeType = getkeySignsNodeType;
function getKeyWordNodeType(word) {
    if (word == 'if') {
        return node_type_1.NodeType.IfNode;
    }
    else if (word == 'else') {
        return node_type_1.NodeType.ElseNode;
    }
    else if (word == 'package') {
        return node_type_1.NodeType.PackageNode;
    }
    else if (word == 'import') {
        return node_type_1.NodeType.ImportNode;
    }
    else if (word == 'enum') {
        return node_type_1.NodeType.EnumNode;
    }
    else if (word == 'struct') {
        return node_type_1.NodeType.StructNode;
    }
    else if (word == 'when') {
        return node_type_1.NodeType.WhenNode;
    }
    else if (word == 'let') {
        return node_type_1.NodeType.LetNode;
    }
    else if (word == 'var') {
        return node_type_1.NodeType.VarNode;
    }
    else if (word == 'val') {
        return node_type_1.NodeType.ValNode;
    }
    else if (word == 'private') {
        return node_type_1.NodeType.PrivateNode;
    }
    else {
        return node_type_1.NodeType.UnknownKeyWordNode;
    }
}
exports.getKeyWordNodeType = getKeyWordNodeType;
function isUniqueSign(word) {
    return new key_words_1.KeyWords().keySignsArray.indexOf(word) > -1;
}
exports.isUniqueSign = isUniqueSign;
function isKeyWord(word) {
    return new key_words_1.KeyWords().keyWordsArray.indexOf(word) > -1;
}
exports.isKeyWord = isKeyWord;
function getCharNodeType(word) {
    if (isKeyWord(word)) {
        return getKeyWordNodeType(word);
    }
    else {
        return node_type_1.NodeType.CharArrayNode;
    }
}
exports.getCharNodeType = getCharNodeType;
//# sourceMappingURL=lookup_node.js.map