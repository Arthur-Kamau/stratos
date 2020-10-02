

const anExampleVariable = "Hello World"
console.log(anExampleVariable)

var keySignsArray: Array<string> = [
	'%', '/', '*', '+', '-', '(', ')', '{', '}', '[', ']', "\"", ":", ";"
]
var keyWordsArray: Array<string> = [
	'let', 'var', 'val',
	'import', 'package', 'function', 'class',
	'double', 'string', 'char', 'private',
	'if', 'else', 'when',
];
class LanguageToken {

	children: LanguageToken[];
	tokenGroup: LanguageNode[];
	constructor(children: LanguageToken[], tokenGroup: LanguageNode[]) {
		this.children = children;
		this.tokenGroup = tokenGroup;

	}
}


enum SyntaxKind {
	VariableDeclaration = 1,
	TypeAliasDeclaration = 2,
	ImportDeclaration = 3,
	ClassDeclaration = 4,
	ImportClause = 5,
	Parameter = 6,
	PropertyDeclaration = 7,
	BinaryExpression = 8,
	EnumDeclaration = 9,
	StructDeclaration = 10,
	FunctionDeclaration = 11,
	PackageDeclaration = 12
}
enum NodeType {
	IntTypeNode = 1,
	DoubleTypeNode = 2,
	StringTypeNode = 3,
	CharTypeNode = 4,
	WhenNode = 5,
	IfNode = 6,
	ElseNode = 7,
	PackageNode = 8,
	ImportNode = 9,
	EnumNode = 10,
	StructNode = 11,
	ClassNode = 12,
	FunctionNode = 13,
	PrivateNode = 14,
	ValNode = 15,
	VarNode = 16,
	LetNode = 17,
	CharArrayNode = 18,
	UnknownKeyWordNode = 19,
	UnknownSignNodeNode = 20,

	// math symobols
	ModulusNode = 21,
	AddNode = 22,
	MinusNode = 23,
	DivideNode = 24,
	SubtractNode = 25,
	MultiplyNode = 26,

	// space Node
	SpaceNode = 27,

	// unif signs
	SquareBracketOpenNode = 28,
	SquareBracketCloseNode = 29,
	CurvedBracketOpenNode = 30,
	CurvedBracketCloseNode = 31,
	CurlyBracketOpenNode = 32,
	CurlyBracketCloseNode = 33,

	SemiColonNode = 34,
	ColonNode = 35,

	// comment
	LineComment = 36,
	MultiLineComment = 37,

	// stirn 
	QuotationNode = 38,

	//new 
	NewLine = 39

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

					if (lineCharArray[charIndex] == "/" && lineCharArray[tempcharIndex - 1] == "/") {
// fall through if 
					} else if (lineCharArray[charIndex] == "/" && lineCharArray[tempcharIndex + 1] == "/") {

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
					} else if (lineCharArray[charIndex] == "/" && lineCharArray[charIndex + 1] == "*") {

						inMultiLineComment = true;
						linemultiLineCommetStart = lineIndex;

						// multiLineCommet += lineCharArray[charIndex];
					} else if (lineCharArray[charIndex] == "/" && lineCharArray[charIndex - 1] == "*") {

					

						multiLineCommet += lineCharArray[charIndex];
						console.log("comment " + multiLineCommet);
						console.log("data  " + lineCharArray[charIndex])
						console.log("char str  " + charaArray)
						var n: LanguageNode = {
							line_start: linemultiLineCommetStart,
							line_end: lineIndex,
							char_start: charIndex,
							char_end: charIndex + lineCharArray[charIndex].length,  // ,
							type: NodeType.MultiLineComment,
							value: multiLineCommet
						};
						documentNode.push(n);

						inMultiLineComment = false;
						multiLineCommet = "";
						linemultiLineCommetStart = 0;

					}

					if (!inMultiLineComment) {
						if (isUniqueSign(lineCharArray[charIndex])) {


							if (charaArray.length > 0) {

								var x: LanguageNode = {
									line_start: lineIndex,
									line_end: lineIndex,
									char_start: charIndex != 0 ? charIndex - charaArray.length : 0,
									char_end: charIndex,
									type: getCharNodeType(charaArray),
									value: charaArray
								};
								documentNode.push(x);
							} else {
								//	console.log("character array empty " + lineCharArray[charIndex] + " line " + lineIndex)
							}

							var n: LanguageNode = {
								line_start: lineIndex,
								line_end: lineIndex,
								char_start: charIndex,
								char_end: charIndex + lineCharArray[charIndex].length,
								type: getkeySignsNodeType(lineCharArray[charIndex]),
								value: lineCharArray[charIndex],
							};
							documentNode.push(n);

							charaArray = "";

						} else if (lineCharArray[charIndex] == " " || lineCharArray[charIndex] == "\t") {


							if (charaArray.length > 0) {

								var x: LanguageNode = {
									line_start: lineIndex,
									line_end: lineIndex,
									char_start: charIndex - charaArray.length,
									char_end: charaArray.length,
									type: getCharNodeType(charaArray),
									value: charaArray
								};
								documentNode.push(x);

								charaArray = "";
							} else {
								//console.log(" line " + lineIndex + " character array empty  item space " + lineCharArray[charIndex])
							}


							var x: LanguageNode = {
								line_start: lineIndex,
								line_end: lineIndex,
								char_start: charIndex,
								char_end: charIndex + lineCharArray[charIndex].length,
								type: NodeType.SpaceNode,
								value: " "
							};
							documentNode.push(x);


						} else {

							charaArray += lineCharArray[charIndex]
						}

					} else {
						console.log("check /" + lineCharArray[charIndex]);
						multiLineCommet += lineCharArray[charIndex];
					}

				}


			} else {
				//console.log("Empty line number "+lineIndex);
			}

			var n: LanguageNode = {
				line_start: lineIndex,
				line_end: lineIndex,
				char_start: lines[lineIndex].length - 1,
				char_end: lines[lineIndex].length,
				type: NodeType.NewLine,
				value: "\n",
			};
			documentNode.push(n);

		}
		return documentNode;
	}


}





class MainScopeRulesRules {


	// rmove comments
	cleanNode(nodes: LanguageNode[]): LanguageNode[] {
		var cleanNodes: LanguageNode[] = []
		for (let tokenIndex = 0; tokenIndex < nodes.length; tokenIndex++) {

			if (nodes[tokenIndex].value == "\n") {// || nodes[tokenIndex].value == " " || nodes[tokenIndex].type == NodeType.LineComment || nodes[tokenIndex].type == NodeType.MultiLineComment) {
				// console.log("Removing node " + nodes[tokenIndex].value);
			} else {
				cleanNodes.push(nodes[tokenIndex]);
			}
		}

		return cleanNodes;
	}


	findClosingCurleyBrace(nodes: LanguageNode[], openCurlyBracePos: number) {
		var closCurlyBracePos = openCurlyBracePos;
		var counter: number = 1;

		while (counter > 0) {
			var item = nodes[++closCurlyBracePos];
			if (item.value == '{') {
				counter++
			} else if (item.value == '}') {
				counter--;
			}
		}


		return closCurlyBracePos;


	}

	createTokens(nodes: LanguageNode[]): LanguageToken[] {
		var children: LanguageToken[] = [];
		var nodesTemp: LanguageNode[] = [];

		var cleanNodes = this.cleanNode(nodes);
		var skipTokens = false;
		var skipTo = 0;

		for (let nodeItem = 0; nodeItem < cleanNodes.length; nodeItem++) {
			//console.log("token item " + JSON.stringify(nodes[nodeItem]));
			if (skipTokens == false) {
				if (nodes[nodeItem].value == ";") {

					children.push(new LanguageToken([], nodesTemp))
					nodesTemp = [];
				} else if (nodes[nodeItem].value == '{') {

					var endCurly = this.findClosingCurleyBrace(nodes, nodeItem);

					skipTokens = true;
					skipTo = endCurly;

					// console.log("Print to " + JSON.stringify(nodes[endCurly]));

					var tokenScope = nodes.slice(nodeItem + 1, endCurly);
					console.log("token scope to " + JSON.stringify(tokenScope));

					var items = this.createTokens(tokenScope);
					console.log("token " + JSON.stringify(items));

					//tokenNodes.push(items )//new LanguageToken(, []))

					children.push(new LanguageToken(items, nodesTemp))
				} else if (nodes[nodeItem].value == '}') {
					//console.log("=======> Return " + JSON.stringify(srcTokens) + "\n\n");
					// return srcTokens;
				} else {
					console.log("add " + nodes[nodeItem].value);
					nodesTemp.push(nodes[nodeItem])
				}
			} else {
				console.log("Skip to  print character number " + nodeItem + " should skip to " + skipTo);
				if (nodeItem == skipTo) {
					skipTokens = false;
					skipTo = 0;

				}
			}


		}
		// console.log("Length "+tokenNodes.length);
		return children;

	}
}

// package main;
// import data; 
// function start(){
//   print("Hey there");
// }

var r: LanguageNode[] = new LanguageTokens().getTokenList(`
/* data man **/

package main; 
function start(){ print ("kamau"); }`);




for (let tokenIndex = 0; tokenIndex < r.length; tokenIndex++) {
	console.log(`node -> ${JSON.stringify(r[tokenIndex])}`);

}


// var tokenItem = new MainScopeRulesRules();
// // var resi = tokenItem.cleanNode(r);
// var toke = tokenItem.createTokens(r);

// console.log("Error " + toke.length);

// for (let tokenIndex = 0; tokenIndex < toke.length; tokenIndex++) {
// 	console.log(`token  -> ${JSON.stringify(toke[tokenIndex])}`);

// }


//check the first token if its package





