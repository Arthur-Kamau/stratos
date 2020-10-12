"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.ImportAnalyzer = void 0;
const vscode_languageserver_1 = require("vscode-languageserver");
const node_type_1 = require("../model/node_type");
const file_util_1 = require("../util/file_util");
class ImportAnalyzer {
    importRules(nodes, filePath) {
        var _a, _b, _c;
        let diagnostics = [];
        let folderName = new file_util_1.FileUtils().getFileDirectoryName(filePath);
        if (nodes[0] == undefined || nodes[1] == undefined) {
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
                message: `Import keyword then  Path `,
                source: 'ex'
            };
            diagnostics.push(diagnostic);
        }
        else if (((_a = nodes[1].type) === null || _a === void 0 ? void 0 : _a.valueOf()) != node_type_1.NodeType.QuotationNode.valueOf()) {
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
                message: `Import word  should be followed by String of the  Path `,
                // message: `equal ${nodes[1].type?.valueOf() == NodeType.StringTypeNode.valueOf()} vs ${JSON.stringify(nodes[1])} `,
                source: 'ex'
            };
            diagnostics.push(diagnostic);
        }
        else {
            if (nodes[0].value != "import") {
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
                    message: `Import keyword then package name`,
                    source: 'ex'
                };
                diagnostics.push(diagnostic);
            }
            var letters = /^[0-9a-zA-Z_"]+$/;
            if (nodes[1].value.match(letters)) {
                if (nodes[1].value.split("/")[0] == ".") {
                    // local packages
                    let diagnostic = {
                        severity: vscode_languageserver_1.DiagnosticSeverity.Warning,
                        range: {
                            start: {
                                line: nodes[1].line_start,
                                character: nodes[1].char_start
                            },
                            end: {
                                line: nodes[1].line_start,
                                character: nodes[1].char_start
                            },
                        },
                        message: `local package next to file`,
                        source: 'ex'
                    };
                    diagnostics.push(diagnostic);
                }
                else if (nodes[1].value.split("/")[0] == "..") {
                    // upp
                    let diagnostic = {
                        severity: vscode_languageserver_1.DiagnosticSeverity.Warning,
                        range: {
                            start: {
                                line: nodes[1].line_start,
                                character: nodes[1].char_start
                            },
                            end: {
                                line: nodes[1].line_start,
                                character: nodes[1].char_start
                            },
                        },
                        message: `package up on level`,
                        source: 'ex'
                    };
                    diagnostics.push(diagnostic);
                }
                else if (nodes[1].value.split("/")[0] == "main") {
                    // local packages
                    let diagnostic = {
                        severity: vscode_languageserver_1.DiagnosticSeverity.Warning,
                        range: {
                            start: {
                                line: nodes[1].line_start,
                                character: nodes[1].char_start
                            },
                            end: {
                                line: nodes[1].line_start,
                                character: nodes[1].char_start
                            },
                        },
                        message: `package start from main`,
                        source: 'ex'
                    };
                    diagnostics.push(diagnostic);
                }
                else {
                    if (((_b = nodes[1].value) === null || _b === void 0 ? void 0 : _b.length) == 0) {
                        let diagnostic = {
                            severity: vscode_languageserver_1.DiagnosticSeverity.Warning,
                            range: {
                                start: {
                                    line: nodes[1].line_start,
                                    character: nodes[1].char_start
                                },
                                end: {
                                    line: nodes[1].line_start,
                                    character: nodes[1].char_start
                                },
                            },
                            message: `You cannot import an empty string`,
                            source: 'ex'
                        };
                        diagnostics.push(diagnostic);
                    }
                    else {
                        let diagnostic = {
                            severity: vscode_languageserver_1.DiagnosticSeverity.Warning,
                            range: {
                                start: {
                                    line: nodes[1].line_start,
                                    character: nodes[1].char_start
                                },
                                end: {
                                    line: nodes[1].line_start,
                                    character: nodes[1].char_start
                                },
                            },
                            message: `Import of non local package is not yet supported {${(_c = nodes[1].value) === null || _c === void 0 ? void 0 : _c.length} => ${nodes[1].value}}`,
                            source: 'ex'
                        };
                        diagnostics.push(diagnostic);
                    }
                }
            }
            else {
                let diagnostic = {
                    severity: vscode_languageserver_1.DiagnosticSeverity.Error,
                    range: {
                        start: {
                            line: nodes[1].line_start,
                            character: nodes[1].char_start
                        },
                        end: {
                            line: nodes[1].line_start,
                            character: nodes[1].char_start
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
exports.ImportAnalyzer = ImportAnalyzer;
//# sourceMappingURL=import_analyzer.js.map