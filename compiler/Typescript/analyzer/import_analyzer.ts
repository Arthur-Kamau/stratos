
import { Diagnostic, DiagnosticSeverity, Position } from 'vscode-languageserver';
import { LanguageNode } from '../model/language_node';
import { LanguageToken } from '../model/language_token';
import { LanguageTokenSyntax, LanguageTokenSyntaxPackage } from '../model/language_token_syntax';
import { NodeType } from '../model/node_type';
import { FileUtils } from '../util/file_util';

export class ImportAnalyzer {

	importRules(nodes: LanguageNode[], filePath: string): Diagnostic[] {
		let diagnostics: Diagnostic[] = [];
		let folderName = new FileUtils().getFileDirectoryName(filePath);


		if (nodes[0] == undefined || nodes[1] == undefined) {
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
				message: `Import keyword then  Path `,
				source: 'ex'
			};
			diagnostics.push(diagnostic);
		} else if (nodes[1].type?.valueOf() != NodeType.QuotationNode.valueOf()) {
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
				message: `Import word  should be followed by String of the  Path `,
				// message: `equal ${nodes[1].type?.valueOf() == NodeType.StringTypeNode.valueOf()} vs ${JSON.stringify(nodes[1])} `,
				source: 'ex'
			};
			diagnostics.push(diagnostic);

		} else {
			if (nodes[0].value != "import") {
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
					message: `Import keyword then package name`,
					source: 'ex'
				};
				diagnostics.push(diagnostic);
			}




			var letters = /^[0-9a-zA-Z_"]+$/;
			if (nodes[1].value!.match(letters)) {

				if (nodes[1].value!.split("/")[0] == ".") {
					// local packages
					let diagnostic: Diagnostic = {
						severity: DiagnosticSeverity.Warning,
						range: {
							start: {
								line: nodes[1].line_start!,
								character: nodes[1].char_start!
							}, //Position.create(nodes[0].line_start, ) ,//textDocument.positionAt(0),
							end: {
								line: nodes[1].line_start!,
								character: nodes[1].char_start!
							},
						},
						message: `local package next to file`,
						source: 'ex'
					};
					diagnostics.push(diagnostic);

				} else if (nodes[1].value!.split("/")[0] == "..") {
					// upp
					let diagnostic: Diagnostic = {
						severity: DiagnosticSeverity.Warning,
						range: {
							start: {
								line: nodes[1].line_start!,
								character: nodes[1].char_start!
							}, //Position.create(nodes[0].line_start, ) ,//textDocument.positionAt(0),
							end: {
								line: nodes[1].line_start!,
								character: nodes[1].char_start!
							},
						},
						message: `package up on level`,
						source: 'ex'
					};
					diagnostics.push(diagnostic);
				} else if (nodes[1].value!.split("/")[0] == "main") {
					// local packages
					let diagnostic: Diagnostic = {
						severity: DiagnosticSeverity.Warning,
						range: {
							start: {
								line: nodes[1].line_start!,
								character: nodes[1].char_start!
							}, //Position.create(nodes[0].line_start, ) ,//textDocument.positionAt(0),
							end: {
								line: nodes[1].line_start!,
								character: nodes[1].char_start!
							},
						},
						message: `package start from main`,
						source: 'ex'
					};
					diagnostics.push(diagnostic);



				} else {
					if (nodes[1].value?.length == 0) {
						let diagnostic: Diagnostic = {
							severity: DiagnosticSeverity.Warning,
							range: {
								start: {
									line: nodes[1].line_start!,
									character: nodes[1].char_start!
								}, //Position.create(nodes[0].line_start, ) ,//textDocument.positionAt(0),
								end: {
									line: nodes[1].line_start!,
									character: nodes[1].char_start!
								},
							},
							message: `You cannot import an empty string`,
							source: 'ex'
						};
						diagnostics.push(diagnostic);

					} else {
						let diagnostic: Diagnostic = {
							severity: DiagnosticSeverity.Warning,
							range: {
								start: {
									line: nodes[1].line_start!,
									character: nodes[1].char_start!
								}, //Position.create(nodes[0].line_start, ) ,//textDocument.positionAt(0),
								end: {
									line: nodes[1].line_start!,
									character: nodes[1].char_start!
								},
							},
							message: `Import of non local package is not yet supported {${nodes[1].value?.length} => ${nodes[1].value}}`,
							source: 'ex'
						};
						diagnostics.push(diagnostic);
					}

				}


			} else {
				let diagnostic: Diagnostic = {
					severity: DiagnosticSeverity.Error,
					range: {
						start: {
							line: nodes[1].line_start!,
							character: nodes[1].char_start!
						}, //Position.create(nodes[0].line_start, ) ,//textDocument.positionAt(0),
						end: {
							line: nodes[1].line_start!,
							character: nodes[1].char_start!
						},
					},
					message: `Import path should be alphanumeric ${nodes[1].value}`,
					source: 'ex'
				};
				diagnostics.push(diagnostic);
			}
		}



		return diagnostics;

	}


}