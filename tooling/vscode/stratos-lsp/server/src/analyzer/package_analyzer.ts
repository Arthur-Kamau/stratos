import { Diagnostic, DiagnosticSeverity, Position } from 'vscode-languageserver';
import { LanguageNode } from '../model/language_node';
import { LanguageToken } from '../model/language_token';
import { LanguageTokenSyntax, LanguageTokenSyntaxPackage } from '../model/language_token_syntax';
import { FileUtils } from '../util/file_util';

export class PackageAnalyzer {

	createPackageTokenSytax(nodes: LanguageNode[]): LanguageTokenSyntax {

		return new LanguageTokenSyntaxPackage(nodes[1].value!, nodes);
	}
	packageRules(nodes: LanguageNode[], filePath: string): Diagnostic[] {
		let diagnostics: Diagnostic[] = [];

		// console.log("nodes package rules " + nodes.length);
		// console.log("nodes token group length " + nodes[0].tokenGroup.length);
		// the package name should be
		let folderName = new FileUtils().getFileDirectoryName(filePath);
		console.log("folder name " + folderName + "node 0" + nodes[0].value);

		if (nodes[0] == undefined || nodes[1] == undefined) {

			let diagnostic: Diagnostic = {
				severity: DiagnosticSeverity.Error,
				range: {
					start: {
						line: nodes[0].line_start!,
						character: nodes[0].char_start!,
					}, //Position.create(nodes[0].line_start, ) ,//textDocument.positionAt(0),
					end: {
						line: nodes[0].line_start!,// nodes[0].line_start!,
						character: nodes[0].char_end!
					}, // textDocument.positionAt(10)
				},
				message: `package and package name not specified  `,
				source: 'ex'
			};
			diagnostics.push(diagnostic);
		} else if (nodes[0].value != "package") {
			let diagnostic: Diagnostic = {
				severity: DiagnosticSeverity.Error,
				range: {
					start: {
						line: nodes[0].line_start!,
						character: nodes[0].char_start!
					}, //Position.create(nodes[0].line_start, ) ,//textDocument.positionAt(0),
					end: {
						line: nodes[0].line_start!,
						character: nodes[0].char_start!
					},
				},
				message: `Package should be declared`,
				source: 'ex'
			};
			diagnostics.push(diagnostic);
		}
		if (folderName == "src") {

			



			if (nodes[1].value != "main") {
				let diagnostic: Diagnostic = {
					severity: DiagnosticSeverity.Error,
					range: {
						start: {
							line: nodes[0].line_start!,
							character: nodes[0].char_start!
						}, //Position.create(nodes[0].line_start, ) ,//textDocument.positionAt(0),
						end: {
							line: nodes[0].line_start!,
							character: nodes[0].char_start!
						}, // textDocument.positionAt(10)
					},
					message: `files in src belong to package main`,
					source: 'ex'
				};
				diagnostics.push(diagnostic);

			}
		} else {
			if (nodes[1].value != folderName) {

				let diagnostic: Diagnostic = {
					severity: DiagnosticSeverity.Error,
					range: {
						start: {
							line: nodes[0].line_start!,
							character: nodes[0].char_start!
						},
						end: {
							line: nodes[0].line_start!,
							character: nodes[0].char_start!
						},
					},
					message: `the package name should be ${folderName}`,
					source: 'ex'
				};
				diagnostics.push(diagnostic);

			}

		}
		// }




		return diagnostics;
	}
}