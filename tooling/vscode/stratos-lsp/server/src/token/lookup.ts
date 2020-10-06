import { Diagnostic, DiagnosticSeverity, Position } from 'vscode-languageserver';
import { LanguageToken } from '../model/language_token';
import { PackageAnalyzer } from '../analyzer/package_analyzer';
import { LanguageTokenSyntax } from '../model/language_token_syntax';

export class LanguageLookUp {

	langugeRules(tokenNodes: LanguageToken[], filePath: string): Diagnostic[] {
		let diagnostics: Diagnostic[] = [];


		//package check 
		// let packageRulesData = new PackageAnalyzer().packageRules(tokenNodes, filePath);
		// diagnostics.push(...packageRulesData);


		let richTokens : LanguageTokenSyntax[] = this.generateRichTokens(tokenNodes);







		return diagnostics;
	}

	generateRichTokens (languageTokens: LanguageToken[]):LanguageTokenSyntax[] {
		let richTokens: LanguageTokenSyntax[] = [];

		for (let tokensIndex = 0; tokensIndex < languageTokens.length; tokensIndex++) {
			// console.log(`node -> ${JSON.stringify(r[tokenIndex])}`);
		  var languageNodes = languageTokens[tokensIndex].tokenGroup;

		}
		

		return richTokens;
	}

}