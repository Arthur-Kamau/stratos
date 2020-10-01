

const anExampleVariable = "Hello World"
console.log(anExampleVariable)

var keySignsArray: Array<string> = [
	'%', '/', '*', '+', '-', ' '
]
var keyWordsArray: Array<string> = [
	'let', 'var', 'val',
	'import', 'package', 'function', 'class',
	'double', 'string', 'char', 'private',
	'if', 'else', 'when',
];



enum SyntaxKind {
	VariableDeclaration,
	TypeAliasDeclaration,
	ImportDeclaration,
	ClassDeclaration,
	ImportClause,
	Parameter,
	PropertyDeclaration,
	BinaryExpression,
	EnumDeclaration,
	StructDeclaration,
	FunctionDeclaration,
	PackageDeclaration
}
enum NodeType {
	IntTypeNode,
	DoubleTypeNode,
	StringTypeNode,
	CharTypeNode,
	WhenNode,
	IfNode,
	ElseNode,
	PackageNode,
	ImportNode,
	EnumNode,
	StructNode,
	ClassNode,
	FunctionNode,
	PrivateNode,
	ValNode, VarNode, LetNode,
	CharArrayNode,
	UnknownKeyWordNode,
	UnknownSignNodeNode,

	// math symobols
	ModulusNode,
	AddNode,
	MinusNode,
	DivideNode,
	SubtractNode,
	MultiplyNode,

	// space Node
	SpaceNode,

	// unif signs
	SquareBracketOpenNode,
	SquareBracketCloseNode,
	CurvedBracketOpenNode,
	CurvedBracketCloseNode,
	CurlyBracketOpenNode,
	CurlyBracketCloseNode,

	SemiColonNode,
	ColonNode,

	// comment
	LineComment,
	MultiLineComment

}
interface LanguageNode {
	start?: number;
	end?: number;
	line?: number;
	type?: NodeType;
	value?: string;
}

function getkeySignsNodeType(word: string): NodeType {
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
	} else if (word = ' ') {
		return NodeType.SpaceNode
	} else {
		return NodeType.UnknownSignNodeNode;
	}
}
function getKeyWordNodeType(word: string): NodeType {
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

function isUniqueSign(word: string): boolean {
	return keySignsArray.indexOf(word) > -1
}

function isKeyWord(word: string): boolean {
	return keyWordsArray.indexOf(word) > -1
}

function getCharNodeType(word: string): NodeType {
	if (isKeyWord(word)) {
		return getKeyWordNodeType(word)
	} else {
		return NodeType.CharArrayNode;
	}
}

class LanguageTokens {

	getCharToken(charaArray: string, line: number, start: number, stop: number): LanguageNode | null {
		if (charaArray.length > 0) {
			var x: LanguageNode = {
				line: line,
				start: start,
				end: stop,
				type: getCharNodeType(charaArray),
				value: charaArray
			};
			return x;
		} else {
			return null;
		}

	}

	getTokenList(text: string): LanguageNode[] {
		var documentNode: LanguageNode[] = []
		let lines = text.split(/\r?\n/g);
		let inMultiLineComment: boolean = false
		let multiLineCommet: string = "";
		for (let lineIndex = 0; lineIndex < lines.length; lineIndex++) {

			if (lines[lineIndex].length > 0) {
				var lineCharArray = lines[lineIndex].split('');

				var charaArray: string = "";

				for (let charIndex = 0; charIndex < lineCharArray.length; charIndex++) {


					if (lineCharArray[charIndex] == "/" && lineCharArray[charIndex + 1] == "/") {

						var n: LanguageNode = {
							start: charIndex,
							end: lines[lineIndex].length,
							line: lineIndex,
							type: NodeType.LineComment,
							value: lines[lineIndex].substr(charIndex, lines[lineIndex].length),
						};
						documentNode.push(n);
						break;
					} else if (lineCharArray[charIndex] == "/" && lineCharArray[charIndex + 1] == "*") {
						console.log("start mulit Comment ")
						inMultiLineComment = true;

						multiLineCommet += lineCharArray[charIndex];
					} else if (lineCharArray[charIndex] == "/" && lineCharArray[charIndex - 1] == "*") {
						console.log("end mulit Comment ")
						inMultiLineComment = false;

						multiLineCommet += lineCharArray[charIndex];

						var n: LanguageNode = {
							start: charIndex,
							end: charIndex + lineCharArray[charIndex].length,
							line: lineIndex,
							type: NodeType.MultiLineComment,
							value: multiLineCommet 
						};
						documentNode.push(n);

						multiLineCommet = "";

						for (let tokenIndex = 0; tokenIndex < documentNode.length; tokenIndex++) {
							console.log(`check comment-> ${JSON.stringify(documentNode[tokenIndex])}`);
						}
					}

					if (!inMultiLineComment) {
						if (isUniqueSign(lineCharArray[charIndex])) {
							var n: LanguageNode = {
								start: charIndex,
								end: charIndex + lineCharArray[charIndex].length,
								line: lineIndex,
								type: getkeySignsNodeType(lineCharArray[charIndex]),
								value: lineCharArray[charIndex],
							};
							documentNode.push(n);

							var res = this.getCharToken(
								charaArray,
								lineIndex,
								charIndex,
								lineCharArray[charIndex].length
							);
							if (res != null) {
								documentNode.push(res);
							}
						} else {

							charaArray += lineCharArray[charIndex]
						}

					}else{
						multiLineCommet += lineCharArray[charIndex];
					}

				}
			} else {
				//console.log("Empty line number "+lineIndex);
			}
		}
		return documentNode;
	}
}


var r: LanguageNode[] = new LanguageTokens().getTokenList(`
// dummy line

/* 
multi line 
dada
 */
package main;

function start(){
 
  print("Hey there");


}
`);

// console.log(`data ${r} `);
// //lets orders 
// var sortedList :  LanguageNode[] = [];
for (let tokenIndex = 0; tokenIndex < r.length; tokenIndex++) {
	console.log(`-> ${JSON.stringify(r[tokenIndex])}`);
	// 	var currItem = r[tokenIndex] ;

	// 	if(sortedList.length == 0){
	// 		sortedList.push(currItem);
	// 	}else{
	// 		// do not mutate list while iterating
	// 		var sortedListCopy  :  LanguageNode[] =sortedList;
	// 		for (let tokenSortedIndex = 0; tokenSortedIndex < sortedListCopy.length; tokenSortedIndex++) {
	// 			if(sortedListCopy[tokenSortedIndex].line >currItem.line ){

	// 			}else if (sortedListCopy[tokenSortedIndex].line == currItem.line){

	// 			}
	// 		}
	// 		//var firstItem = sortedList[0];
	//         // if(currItem.line > firstItem){}
	// 		//myArray.splice(index, 0, item);

	// 	}
}





