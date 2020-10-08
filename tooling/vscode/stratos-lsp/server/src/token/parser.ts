
import { LanguageNode } from '../model/language_node';
import { LanguageToken } from "../model/language_token";
import { NodeType } from "../model/node_type";
import { Diagnostic, DiagnosticSeverity, Position } from 'vscode-languageserver';
export class LanguageParser {





	/// a statement ends in ; or start scope {  
	/// when we encounter ;  we split the tokens and push the	all the previous tokens to the token group
	/// when we encounter {  we should keep score of the state depth (scope) 
	/// when we ecnounter } we should remove a score of the depth state 
	// to ensure variables can only be accessed if appropriate depth

	createTokens(nodesPar: LanguageNode[]): [LanguageToken[], Diagnostic[]] {
		var children: LanguageToken[] = [];
		let diagnostics: Diagnostic[] = [];
		var nodesTemp: LanguageNode[] = [];

		var cleanNodesItems = this.cleanNode(nodesPar);
		var skipTokens = false;
		var skipTo = 0;

		for (let nodeItem = 0; nodeItem < cleanNodesItems.length; nodeItem++) {
			//	console.log("token item " + JSON.stringify(cleanNodesItems[nodeItem]));
			if (skipTokens == false) {
				if (cleanNodesItems[nodeItem].value == ";") {

					nodesTemp.push(cleanNodesItems[nodeItem]);
					children.push(new LanguageToken([], nodesTemp));

					nodesTemp = [];
				} else if (cleanNodesItems[nodeItem].value == '{') {

					var endCurly = this.findClosingCurleyBrace(cleanNodesItems, nodeItem);

					if (endCurly == 0) {
						let diagnostic: Diagnostic = {
							severity: DiagnosticSeverity.Error,
							range: {
								start: {
									line: cleanNodesItems[nodeItem].line_start!,
									character: cleanNodesItems[nodeItem].char_start!,
								}, //Position.create(nodes[0].line_start, ) ,//textDocument.positionAt(0),
								end: {
									line: cleanNodesItems[nodeItem].line_start!,// nodes[0].line_start!,
									character: cleanNodesItems[nodeItem].char_end!
								}, // textDocument.positionAt(10)
							},
							message: `Did not find closing tag }  `,
							source: 'ex'
						};
						diagnostics.push(diagnostic);
					} else {
						skipTokens = true;
						skipTo = endCurly;

						var tokenScope = cleanNodesItems.slice(nodeItem + 1, endCurly);
						console.log("token scope to " + JSON.stringify(tokenScope));

						var items = this.createTokens(tokenScope);
						console.log("token " + JSON.stringify(items));


						if (items[1].length > 0) {
							diagnostics.push(...items[1]);
						}
						children.push(new LanguageToken(items[0], nodesTemp));
						nodesTemp = [];
					}

				} else if (cleanNodesItems[nodeItem].value == '}') {
					console.log("=======> Return  sign \n\n");
					// return srcTokens;
				} else {
					//	console.log("add " + nodes[nodeItem].value);
					nodesTemp.push(cleanNodesItems[nodeItem])
				}
			} else {
				//console.log("Skip to  print character number " + nodeItem + " should skip to " + skipTo);
				if (nodeItem == skipTo) {
					skipTokens = false;
					skipTo = 0;

				}
			}


		}
		console.log("Length " + children.length);
		return [children, diagnostics];

	}


	// rmove comments
	cleanNode(nodes: LanguageNode[]): LanguageNode[] {
		var cleanNodeItems: LanguageNode[] = []
		for (let tokenIndex = 0; tokenIndex < nodes.length; tokenIndex++) {

			if (nodes[tokenIndex].type == NodeType.NewLine || nodes[tokenIndex].type == NodeType.SpaceNode || nodes[tokenIndex].type == NodeType.SpaceNode || nodes[tokenIndex].type == NodeType.LineComment || nodes[tokenIndex].type == NodeType.MultiLineComment) {
				console.log("Removing node " + nodes[tokenIndex].value + " type " + nodes[tokenIndex].type + "  Remeber [ newline =" + NodeType.NewLine + ", Space = " + NodeType.SpaceNode + " ,  line comme =" + NodeType.LineComment + "  , multilicoomet= " + NodeType.MultiLineComment + " ]");
			} else {
				cleanNodeItems.push(nodes[tokenIndex]);
			}
		}

		return cleanNodeItems;
	}


	findClosingCurleyBrace(nodes: LanguageNode[], openCurlyBracePos: number) {
		var closCurlyBracePos = openCurlyBracePos;
		var counter: number = 1;

		while (counter > 0) {
			var item = nodes[++closCurlyBracePos];

			if (item == undefined) {
				closCurlyBracePos = 0;
				break;
			} else {
				if (item.value == '{') {
					counter++
				} else if (item.value == '}') {
					counter--;
				}
			}
		}


		return closCurlyBracePos;


	}





}