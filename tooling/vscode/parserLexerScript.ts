

const anExampleVariable = "Hello World"
console.log(anExampleVariable)

var keySignsArray: Array<string> = [
	'%', '/', '*', '+', '-', '(', ')', '{', '}', '[', ']'
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
	char_start?: number;
	char_end?: number;
	line_start?: number;
	line_end?: number;
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
	console.log("tyepe " + word);
	if (isKeyWord(word)) {
		return getKeyWordNodeType(word)
	} else {
		return NodeType.CharArrayNode;
	}
}

class LanguageTokens {



	getTokenList(text: string): LanguageNode[] {
		var documentNode: LanguageNode[] = []
		let lines = text.split(/\r?\n/g);
		let inMultiLineComment: boolean = false
		let multiLineCommet: string = "";
		let linemultiLineCommetStart: number = 0;
		for (let lineIndex = 0; lineIndex < lines.length; lineIndex++) {

			if (lines[lineIndex].length > 0) {
				var lineCharArray = lines[lineIndex].split('');

				var charaArray: string = "";

				for (let charIndex = 0; charIndex < lineCharArray.length; charIndex++) {
					var tempcharIndex = charIndex;

					if (lineCharArray[charIndex] == "/" && lineCharArray[charIndex + 1] == "/") {

						var n: LanguageNode = {
							line_start: lineIndex,
							line_end: lineIndex,
							char_start: charIndex,
							char_end: lines[lineIndex].length,
							type: NodeType.LineComment,
							value: lines[lineIndex].substr(charIndex, lines[lineIndex].length),
						};
						documentNode.push(n);
						break;
					} else if (lineCharArray[charIndex] == "/" && lineCharArray[tempcharIndex + 1] == "*") {

						inMultiLineComment = true;
						linemultiLineCommetStart = lineIndex;

						multiLineCommet += lineCharArray[charIndex];
					} else if (lineCharArray[charIndex] == "/" && lineCharArray[tempcharIndex - 1] == "*") {

						inMultiLineComment = false;

						multiLineCommet += lineCharArray[charIndex];

						var n: LanguageNode = {
							line_start: linemultiLineCommetStart,
							line_end: lineIndex,
							char_start: charIndex,
							char_end: charIndex + lineCharArray[charIndex].length,  // ,
							type: NodeType.MultiLineComment,
							value: multiLineCommet
						};
						documentNode.push(n);

						multiLineCommet = "";
						linemultiLineCommetStart = 0;

					}

					if (!inMultiLineComment) {
						if (isUniqueSign(lineCharArray[charIndex])) {
							var n: LanguageNode = {
								line_start: lineIndex,
								line_end: lineIndex,
								char_start: charIndex,
								char_end: charIndex + lineCharArray[charIndex].length,
								type: getkeySignsNodeType(lineCharArray[charIndex]),
								value: lineCharArray[charIndex],
							};
							documentNode.push(n);

							if (charaArray.length > 0) {
								var x: LanguageNode = {
									line_start: lineIndex,
									line_end: lineIndex,
									char_start: charIndex,
									char_end: charIndex + charaArray.length,
									type: getCharNodeType(charaArray),
									value: charaArray
								};
								documentNode.push(x);
							} else {
								console.log("character array empty " + lineCharArray[charIndex])
							}

						} else if (lineCharArray[charIndex] == " ") {


							if (charaArray.length > 0) {
								console.log("char array " + charaArray);
								var x: LanguageNode = {
									line_start: lineIndex,
									line_end: lineIndex,
									char_start: charIndex,
									char_end: charIndex + charaArray.length,
									type: getCharNodeType(charaArray),
									value: charaArray
								};
								documentNode.push(x);
							} else {
								console.log(" line " + lineIndex + " character array empty  item space " + lineCharArray[charIndex])
							}


							var x: LanguageNode = {
								line_start: lineIndex,
								line_end: lineIndex,
								char_start: charIndex,
								char_end: charIndex + lineCharArray[charIndex].length,
								type: NodeType.SpaceNode,
								value: charaArray
							};
							documentNode.push(x);

							charaArray = "";
						} else {

							charaArray += lineCharArray[charIndex]
						}

					} else {
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





