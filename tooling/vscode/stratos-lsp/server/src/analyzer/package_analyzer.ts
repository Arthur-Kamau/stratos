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

		// if (nodes == undefined || nodes.length == 0 || nodes[0].tokenGroup == null || nodes[0].tokenGroup.length == 0) {
		// 	let diagnostic: Diagnostic = {
		// 		severity: DiagnosticSeverity.Error,
		// 		range: {
		// 			start: {
		// 				line: 0,//nodes[0].tokenGroup[0].line_start!,
		// 				character: 0, // nodes[0].tokenGroup[0].char_start!
		// 			}, //Position.create(nodes[0].tokenGroup[0].line_start, ) ,//textDocument.positionAt(0),
		// 			end: {
		// 				line: 0,
		// 				character: 0
		// 			}, // textDocument.positionAt(10)
		// 		},
		// 		message: `package should be the first thing declared in a file`,
		// 		source: 'ex'
		// 	};
		// 	diagnostics.push(diagnostic);
		// } else {

		// console.log("package " + JSON.stringify(nodes[0].tokenGroup[0]))
		// console.log("name " + JSON.stringify(nodes[0].tokenGroup[1]))
		// if (nodes[0].tokenGroup[0].value != "package") {
		// 	let diagnostic: Diagnostic = {
		// 		severity: DiagnosticSeverity.Error,
		// 		range: {
		// 			start: {
		// 				line: nodes[0].tokenGroup[0].line_start!,
		// 				character: nodes[0].tokenGroup[0].char_start!
		// 			}, //Position.create(nodes[0].tokenGroup[0].line_start, ) ,//textDocument.positionAt(0),
		// 			end: {
		// 				line: nodes[0].tokenGroup[0].line_start!,
		// 				character: nodes[0].tokenGroup[0].char_start!
		// 			},
		// 		},
		// 		message: `Package should be declared`,
		// 		source: 'ex'
		// 	};
		// 	diagnostics.push(diagnostic);
		// }
		// if (folderName == "src") {
		// 	if (nodes[0].tokenGroup[1].value != "main") {
		// 		let diagnostic: Diagnostic = {
		// 			severity: DiagnosticSeverity.Error,
		// 			range: {
		// 				start: {
		// 					line: nodes[0].tokenGroup[1].line_start!,
		// 					character: nodes[0].tokenGroup[1].char_start!
		// 				}, //Position.create(nodes[0].tokenGroup[0].line_start, ) ,//textDocument.positionAt(0),
		// 				end: {
		// 					line: nodes[0].tokenGroup[1].line_start!,
		// 					character: nodes[0].tokenGroup[1].char_start!
		// 				}, // textDocument.positionAt(10)
		// 			},
		// 			message: `files in src belong to package main`,
		// 			source: 'ex'
		// 		};
		// 		diagnostics.push(diagnostic);

		// 	}
		// } else {
		// 	if (nodes[0].tokenGroup[1] == undefined) {

		// 		let diagnostic: Diagnostic = {
		// 			severity: DiagnosticSeverity.Error,
		// 			range: {
		// 				start: {
		// 					line: nodes[0].tokenGroup[0].line_start!,
		// 					character: nodes[0].tokenGroup[0].char_start!,
		// 				}, //Position.create(nodes[0].tokenGroup[0].line_start, ) ,//textDocument.positionAt(0),
		// 				end: {
		// 					line: nodes[0].tokenGroup[0].line_start! ,// nodes[0].tokenGroup[1].line_start!,
		// 					character: nodes[0].tokenGroup[0].char_end!
		// 				}, // textDocument.positionAt(10)
		// 			},
		// 			message: `the package name not specified  `,
		// 			source: 'ex'
		// 		};
		// 		diagnostics.push(diagnostic);
		// 	}else if (nodes[0].tokenGroup[1].value != folderName) {

		// 		let diagnostic: Diagnostic = {
		// 			severity: DiagnosticSeverity.Error,
		// 			range: {
		// 				start: {
		// 					line: nodes[0].tokenGroup[1].line_start!,
		// 					character: nodes[0].tokenGroup[1].char_start!
		// 				},
		// 				end: {
		// 					line: nodes[0].tokenGroup[1].line_start!,
		// 					character: nodes[0].tokenGroup[1].char_start!
		// 				},
		// 			},
		// 			message: `the package name should be ${folderName}`,
		// 			source: 'ex'
		// 		};
		// 		diagnostics.push(diagnostic);

		// 	}

		// }
		// }




		return diagnostics;
	}
}