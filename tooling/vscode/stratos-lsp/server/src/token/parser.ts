
import { LanguageNode } from '../model/language_node';
import { LanguageToken } from "../model/language_token";
import {NodeType} from "../model/node_type";

export class LanguageParser {





	/// a statement ends in ; or start scope {  
	/// when we encounter ;  we split the tokens and push the	all the previous tokens to the token group
	/// when we encounter {  we should keep score of the state depth (scope) 
	/// when we ecnounter } we should remove a score of the depth state 
	// to ensure variables can only be accessed if appropriate depth

	createTokens(nodesPar: LanguageNode[]): LanguageToken[] {
		var children: LanguageToken[] = [];
		var nodesTemp: LanguageNode[] = [];

		var cleanNodesItems = this.cleanNode(nodesPar);
		var skipTokens = false;
		var skipTo = 0;

		for (let nodeItem = 0; nodeItem < cleanNodesItems.length; nodeItem++) {
		//	console.log("token item " + JSON.stringify(cleanNodesItems[nodeItem]));
			if (skipTokens == false) {
				if (cleanNodesItems[nodeItem].value == ";") {

					children.push(new LanguageToken([], nodesTemp))
					nodesTemp = [];
				} else if (cleanNodesItems[nodeItem].value == '{') {

					var endCurly = this.findClosingCurleyBrace(cleanNodesItems, nodeItem);

					skipTokens = true;
					skipTo = endCurly;

					// console.log("Print to " + JSON.stringify(nodes[endCurly]));

					var tokenScope = cleanNodesItems.slice(nodeItem + 1, endCurly);
					console.log("token scope to " + JSON.stringify(tokenScope));

					var items = this.createTokens(tokenScope);
					console.log("token " + JSON.stringify(items));

					//tokenNodes.push(items )//new LanguageToken(, []))

					children.push(new LanguageToken(items, nodesTemp))
				} else if (cleanNodesItems[nodeItem].value == '}') {
					//console.log("=======> Return " + JSON.stringify(srcTokens) + "\n\n");
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
		console.log("Length "+children.length);
		return children;

	}


	// rmove comments
	cleanNode(nodes: LanguageNode[]): LanguageNode[] {
		var cleanNodeItems: LanguageNode[] = []
		for (let tokenIndex = 0; tokenIndex < nodes.length; tokenIndex++) {

			if (nodes[tokenIndex].type == NodeType.NewLine || nodes[tokenIndex].type == NodeType.SpaceNode ||  nodes[tokenIndex].type ==NodeType.SpaceNode  || nodes[tokenIndex].type == NodeType.LineComment || nodes[tokenIndex].type == NodeType.MultiLineComment) {
				console.log("Removing node " + nodes[tokenIndex].value +" type "+ nodes[tokenIndex].type + "  Remeber [ newline =" +NodeType.NewLine+ ", Space = "+NodeType.SpaceNode  +" ,  line comme ="+NodeType.LineComment+"  , multilicoomet= "+NodeType.MultiLineComment+" ]");
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
			if (item.value == '{') {
				counter++
			} else if (item.value == '}') {
				counter--;
			}
		}


		return closCurlyBracePos;


	}


	


}