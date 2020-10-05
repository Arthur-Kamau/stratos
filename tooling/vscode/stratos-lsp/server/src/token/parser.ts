
import { LanguageToken } from "../model/language_token";

export class LanguageParser {





	/// a statement ends in ; or start scope {  
	/// when we encounter ;  we split the tokens and push the	all the previous tokens to the token group
	/// when we encounter {  we should keep score of the state depth (scope) 
	/// when we ecnounter } we should remove a score of the depth state 
	// to ensure variables can only be accessed if appropriate depth

	createTokens(nodes: LanguageNode[]): LanguageToken[] {
		var srcTokens: LanguageToken[] = [];
		var tokenNodes: LanguageNode[] = [];

		var cleanNodes = this.cleanNode(nodes);
		for (let nodeItem = 0; nodeItem < cleanNodes.length; nodeItem++) {

			if (cleanNodes[nodeItem].type == NodeType.SemiColonNode) {

				srcTokens.push(new LanguageToken([], tokenNodes))
				tokenNodes = [] ;
			} else if (cleanNodes[nodeItem].type == NodeType.CurlyBracketOpenNode) {
            
			   var tokenScope = nodes.slice( nodeItem , nodes.length );
			   var resToken : LanguageToken [] = this.createTokens(tokenScope);
				srcTokens.push( new LanguageToken(resToken ,[]) )
			} else if (cleanNodes[nodeItem].type == NodeType.CurlyBracketCloseNode) {

				return srcTokens ; 
			} else {
				tokenNodes.push(cleanNodes[nodeItem])
			}

		}
		return srcTokens;

	}


	// rmove comments
	cleanNode(nodes: LanguageNode[]): LanguageNode[] {
		var cleanNodes: LanguageNode[] = []
		for (let tokenIndex = 0; tokenIndex < nodes.length; tokenIndex++) {

			if (nodes[tokenIndex].type == NodeType.NewLine || nodes[tokenIndex].type == NodeType.SpaceNode || nodes[tokenIndex].type == NodeType.LineComment || nodes[tokenIndex].type == NodeType.MultiLineComment) {
				console.log("Removing node " + nodes[tokenIndex].value);
			} else {
				cleanNodes.push(nodes[tokenIndex]);
			}
		}

		return cleanNodes;
	}


	


}