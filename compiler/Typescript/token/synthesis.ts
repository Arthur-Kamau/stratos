import { Diagnostic, DiagnosticSeverity, Position } from 'vscode-languageserver';
import { LanguageToken } from '../model/language_token';
import { PackageAnalyzer } from '../analyzer/package_analyzer';
import { LanguageTokenSyntax, LanguageTokenSyntaxGroup } from '../model/language_token_syntax';
import { LanguageNode } from '../model/language_node';
import { SyntaxKindType } from '../model/syntax_kind';
import { ImportAnalyzer } from '../analyzer/import_analyzer';

export class LanguageSynthesis {

	langugeRules(tokenNodes: LanguageToken[], filePath: string): Diagnostic[] {
		let diagnostics: Diagnostic[] = [];

		console.log("tokenNodes length " + tokenNodes.length);

		//rich tokens
		let richTokens: [LanguageTokenSyntaxGroup[], Diagnostic[]] = this.generateRichTokens(tokenNodes, filePath);
		console.log("found errors  langugeRules  " + richTokens[1].length);
		diagnostics.push(...richTokens[1]);

		// warning and usage serch etc





		return diagnostics;
	}

	generateRichTokens(languageTokens: LanguageToken[], filePath: string): [LanguageTokenSyntaxGroup[], Diagnostic[]] {
		let richTokens: LanguageTokenSyntaxGroup[] = [];
		let diagnostics: Diagnostic[] = [];
		for (let tokensIndex = 0; tokensIndex < languageTokens.length; tokensIndex++) {
			console.log("===================== loop " + JSON.stringify(languageTokens[tokensIndex]) + " =================");

			var childrenObject: [LanguageTokenSyntaxGroup[], Diagnostic[]];

			var tokenGroupsItem: LanguageTokenSyntaxGroup[] = [];

			var children = languageTokens[tokensIndex].children;
			if (children.length > 0) {
				childrenObject = this.generateRichTokens(children, filePath);
				diagnostics.push(...childrenObject[1]);
				tokenGroupsItem.push(...childrenObject[0])
			}
			var languageNodes: LanguageNode[] = languageTokens[tokensIndex].tokenGroup;
			console.log("tokenNodes length " + JSON.stringify(languageNodes));

			var tokensGroup = this.inspectNodesForTokenType(languageNodes, filePath);
			// check childres
			var richTokensItem: LanguageTokenSyntaxGroup = new LanguageTokenSyntaxGroup(tokenGroupsItem,
				tokensGroup[0]);
			richTokens.push(richTokensItem);
			diagnostics.push(...tokensGroup[1])

		}


		return [richTokens, diagnostics];
	}


	inspectNodesForTokenType(node: LanguageNode[], filePath: string): [LanguageTokenSyntax, Diagnostic[]] {
		//let diagnostics: Diagnostic[] = [];
		if (node.length == 0) {
			console.log("found zero  ====  ");
			return [new LanguageTokenSyntax(SyntaxKindType.UnknownDeclaration, []), []];
		} else {

			//console.log("Inspect language nodes  " + node.length + "confirm 0" + node[0]);
			//	var letters = /^[0-9a-zA-Z]+$/;
			// check if containts package 
			if (node[0].value == "package") {

				var erros: Diagnostic[] = new PackageAnalyzer().packageRules(node, filePath);
				var syntax: LanguageTokenSyntax = new PackageAnalyzer().createPackageTokenSytax(node);

				console.log("package found errors  ====  " + erros.length);
				return [syntax, erros];
			} else if (node[0].value == "import") {

				var erros: Diagnostic[] = new ImportAnalyzer().importRules(node, filePath);
				console.log("import found errors  ====  " + erros.length);
				return [new LanguageTokenSyntax(SyntaxKindType.UnknownDeclaration, []), erros];
			} else if (this.containsKeyword(node, "function")) {
				return [new LanguageTokenSyntax(SyntaxKindType.UnknownDeclaration, []), []];
			} else if (this.containsKeyword(node, "var")) {
				return [new LanguageTokenSyntax(SyntaxKindType.UnknownDeclaration, []), []];
			} else if (this.containsKeyword(node, "val")) {
				return [new LanguageTokenSyntax(SyntaxKindType.UnknownDeclaration, []), []];
			} else if (this.containsKeyword(node, "let")) {
				return [new LanguageTokenSyntax(SyntaxKindType.UnknownDeclaration, []), []];
			} else if (node[0] == undefined) {
				return [new LanguageTokenSyntax(SyntaxKindType.UnknownDeclaration, []), []];
			} else {
				return [new LanguageTokenSyntax(SyntaxKindType.UnknownDeclaration, []), []];
			}

		}

	}

	containsKeyword(node: LanguageNode[], word: string): boolean {
		var contains: boolean = false;
		for (let nodeItem = 0; nodeItem < node.length; nodeItem++) {
			if (node[nodeItem].value == word) {
				contains = true;
				break;
			}
		}
		return contains;
	}

	//
	// var statement = ["import", "package" , "function" , "var", "val" , "let" ];


	//   console.log("find "+node.find(e => e.value === "import")); 
	//   var res = node.find(e => e.value === element);

	// statement.forEach(element => {


	// 	if(res==undefined)
	// });

	// }

}