"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.LanguageParser = void 0;
const language_token_1 = require("../model/language_token");
const node_type_1 = require("../model/node_type");
const vscode_languageserver_1 = require("vscode-languageserver");
class LanguageParser {
    /// a statement ends in ; or start scope {  
    /// when we encounter ;  we split the tokens and push the	all the previous tokens to the token group
    /// when we encounter {  we should keep score of the state depth (scope) 
    /// when we ecnounter } we should remove a score of the depth state 
    // to ensure variables can only be accessed if appropriate depth
    createTokens(nodesPar) {
        var children = [];
        let diagnostics = [];
        var nodesTemp = [];
        var cleanNodesItems = this.cleanNode(nodesPar);
        var skipTokens = false;
        var skipTo = 0;
        for (let nodeItem = 0; nodeItem < cleanNodesItems.length; nodeItem++) {
            //	console.log("token item " + JSON.stringify(cleanNodesItems[nodeItem]));
            if (skipTokens == false) {
                if (cleanNodesItems[nodeItem].value == ";") {
                    nodesTemp.push(cleanNodesItems[nodeItem]);
                    children.push(new language_token_1.LanguageToken([], nodesTemp));
                    nodesTemp = [];
                }
                else if (cleanNodesItems[nodeItem].value == '{') {
                    var endCurly = this.findClosingCurleyBrace(cleanNodesItems, nodeItem);
                    if (endCurly == 0) {
                        let diagnostic = {
                            severity: vscode_languageserver_1.DiagnosticSeverity.Error,
                            range: {
                                start: {
                                    line: cleanNodesItems[nodeItem].line_start,
                                    character: cleanNodesItems[nodeItem].char_start,
                                },
                                end: {
                                    line: cleanNodesItems[nodeItem].line_start,
                                    character: cleanNodesItems[nodeItem].char_end
                                },
                            },
                            message: `Did not find closing tag }  `,
                            source: 'ex'
                        };
                        diagnostics.push(diagnostic);
                    }
                    else {
                        skipTokens = true;
                        skipTo = endCurly;
                        var tokenScope = cleanNodesItems.slice(nodeItem + 1, endCurly);
                        console.log("token scope to " + JSON.stringify(tokenScope));
                        var items = this.createTokens(tokenScope);
                        console.log("token " + JSON.stringify(items));
                        if (items[1].length > 0) {
                            diagnostics.push(...items[1]);
                        }
                        children.push(new language_token_1.LanguageToken(items[0], nodesTemp));
                        nodesTemp = [];
                    }
                }
                else if (cleanNodesItems[nodeItem].value == '}') {
                    console.log("=======> Return  sign \n\n");
                    // return srcTokens;
                }
                else {
                    //	console.log("add " + nodes[nodeItem].value);
                    nodesTemp.push(cleanNodesItems[nodeItem]);
                }
            }
            else {
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
    cleanNode(nodes) {
        var cleanNodeItems = [];
        for (let tokenIndex = 0; tokenIndex < nodes.length; tokenIndex++) {
            if (nodes[tokenIndex].type == node_type_1.NodeType.NewLine || nodes[tokenIndex].type == node_type_1.NodeType.SpaceNode || nodes[tokenIndex].type == node_type_1.NodeType.SpaceNode || nodes[tokenIndex].type == node_type_1.NodeType.LineComment || nodes[tokenIndex].type == node_type_1.NodeType.MultiLineComment) {
                console.log("Removing node " + nodes[tokenIndex].value + " type " + nodes[tokenIndex].type + "  Remeber [ newline =" + node_type_1.NodeType.NewLine + ", Space = " + node_type_1.NodeType.SpaceNode + " ,  line comme =" + node_type_1.NodeType.LineComment + "  , multilicoomet= " + node_type_1.NodeType.MultiLineComment + " ]");
            }
            else {
                cleanNodeItems.push(nodes[tokenIndex]);
            }
        }
        return cleanNodeItems;
    }
    findClosingCurleyBrace(nodes, openCurlyBracePos) {
        var closCurlyBracePos = openCurlyBracePos;
        var counter = 1;
        while (counter > 0) {
            var item = nodes[++closCurlyBracePos];
            if (item == undefined) {
                closCurlyBracePos = 0;
                break;
            }
            else {
                if (item.value == '{') {
                    counter++;
                }
                else if (item.value == '}') {
                    counter--;
                }
            }
        }
        return closCurlyBracePos;
    }
}
exports.LanguageParser = LanguageParser;
//# sourceMappingURL=parser.js.map