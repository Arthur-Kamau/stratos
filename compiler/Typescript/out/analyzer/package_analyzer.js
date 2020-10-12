"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.PackageAnalyzer = void 0;
const vscode_languageserver_1 = require("vscode-languageserver");
const language_token_syntax_1 = require("../model/language_token_syntax");
const file_util_1 = require("../util/file_util");
class PackageAnalyzer {
    createPackageTokenSytax(nodes) {
        return new language_token_syntax_1.LanguageTokenSyntaxPackage(nodes[1].value, nodes);
    }
    packageRules(nodes, filePath) {
        let diagnostics = [];
        // console.log("nodes package rules " + nodes.length);
        // console.log("nodes token group length " + nodes[0].tokenGroup.length);
        // the package name should be
        let folderName = new file_util_1.FileUtils().getFileDirectoryName(filePath);
        console.log("folder name " + folderName + "node 0" + nodes[0].value);
        if (nodes[0] == undefined || nodes[1] == undefined) {
            let diagnostic = {
                severity: vscode_languageserver_1.DiagnosticSeverity.Error,
                range: {
                    start: {
                        line: nodes[0].line_start,
                        character: nodes[0].char_start,
                    },
                    end: {
                        line: nodes[0].line_start,
                        character: nodes[0].char_end
                    },
                },
                message: `package and package name not specified  `,
                source: 'ex'
            };
            diagnostics.push(diagnostic);
        }
        else if (nodes[0].value != "package") {
            let diagnostic = {
                severity: vscode_languageserver_1.DiagnosticSeverity.Error,
                range: {
                    start: {
                        line: nodes[0].line_start,
                        character: nodes[0].char_start
                    },
                    end: {
                        line: nodes[0].line_start,
                        character: nodes[0].char_start
                    },
                },
                message: `Package should be declared`,
                source: 'ex'
            };
            diagnostics.push(diagnostic);
        }
        if (folderName == "src") {
            if (nodes[1].value != "main") {
                let diagnostic = {
                    severity: vscode_languageserver_1.DiagnosticSeverity.Error,
                    range: {
                        start: {
                            line: nodes[0].line_start,
                            character: nodes[0].char_start
                        },
                        end: {
                            line: nodes[0].line_start,
                            character: nodes[0].char_start
                        },
                    },
                    message: `files in src belong to package main`,
                    source: 'ex'
                };
                diagnostics.push(diagnostic);
            }
        }
        else {
            if (nodes[1].value != folderName) {
                let diagnostic = {
                    severity: vscode_languageserver_1.DiagnosticSeverity.Error,
                    range: {
                        start: {
                            line: nodes[0].line_start,
                            character: nodes[0].char_start
                        },
                        end: {
                            line: nodes[0].line_start,
                            character: nodes[0].char_start
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
exports.PackageAnalyzer = PackageAnalyzer;
//# sourceMappingURL=package_analyzer.js.map