
import {KeyWords} from "../keyWords/key_words";

export function getkeySignsNodeType(word: string): NodeType {
	if (word == '*') {
		return NodeType.MultiplyNode;
	} else if (word == '/') {
		return NodeType.DivideNode;
	} else if (word == '+') {
		return NodeType.AddNode;
	} else if (word == '*') {
		return NodeType.MultiplyNode;
	} else if (word == '%') {
		return NodeType.ModulusNode;
	} else if (word == ':') {
		return NodeType.ColonNode;
	} else if (word == ';') {
		return NodeType.SemiColonNode;
	} else if (word = '"') {
		return NodeType.QuotationNode;
	} else if (word = '(') {
		return NodeType.CurvedBracketOpenNode;
	} else if (word = ')') {
		return NodeType.CurvedBracketCloseNode;
	} else if (word = '{') {
		return NodeType.CurlyBracketOpenNode;
	} else if (word = '}') {
		return NodeType.CurlyBracketCloseNode;
	} else if (word = '[') {
		return NodeType.SquareBracketOpenNode;
	} else if (word = ']') {
		return NodeType.SquareBracketCloseNode;
	} else {
		return NodeType.UnknownSignNodeNode;
	}

}

export function getKeyWordNodeType(word: string): NodeType {
	if (word == 'if') {
		return NodeType.IfNode;
	} else if (word == 'else') {
		return NodeType.ElseNode;
	} else if (word == 'package') {
		return NodeType.PackageNode;
	} else if (word == 'import') {
		return NodeType.ImportNode;
	} else if (word == 'enum') {
		return NodeType.EnumNode;
	} else if (word == 'struct') {
		return NodeType.StructNode;
	} else if (word == 'when') {
		return NodeType.WhenNode;
	} else if (word == 'let') {
		return NodeType.LetNode;
	} else if (word == 'var') {
		return NodeType.VarNode;
	} else if (word == 'val') {
		return NodeType.ValNode;
	} else if (word == 'private') {
		return NodeType.PrivateNode;
	} else {
		return NodeType.UnknownKeyWordNode;
	}
}


export function isUniqueSign(word: string): boolean {
	return new KeyWords().keySignsArray.indexOf(word) > -1
}

export function isKeyWord(word: string): boolean {
	return new KeyWords().keyWordsArray.indexOf(word) > -1
}

export function getCharNodeType(word: string): NodeType {
	if (isKeyWord(word)) {
		return getKeyWordNodeType(word)
	} else {
		return NodeType.CharArrayNode;
	}
}