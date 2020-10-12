"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.LanguageSynthesis = void 0;
const package_analyzer_1 = require("../analyzer/package_analyzer");
const language_token_syntax_1 = require("../model/language_token_syntax");
const syntax_kind_1 = require("../model/syntax_kind");
const import_analyzer_1 = require("../analyzer/import_analyzer");
class LanguageSynthesis {
    langugeRules(tokenNodes, filePath) {
        let diagnostics = [];
        console.log("tokenNodes length " + tokenNodes.length);
        //rich tokens
        let richTokens = this.generateRichTokens(tokenNodes, filePath);
        console.log("found errors  langugeRules  " + richTokens[1].length);
        diagnostics.push(...richTokens[1]);
        // warning and usage serch etc
        return diagnostics;
    }
    generateRichTokens(languageTokens, filePath) {
        let richTokens = [];
        let diagnostics = [];
        for (let tokensIndex = 0; tokensIndex < languageTokens.length; tokensIndex++) {
            console.log("===================== loop " + JSON.stringify(languageTokens[tokensIndex]) + " =================");
            var childrenObject;
            var tokenGroupsItem = [];
            var children = languageTokens[tokensIndex].children;
            if (children.length > 0) {
                childrenObject = this.generateRichTokens(children, filePath);
                diagnostics.push(...childrenObject[1]);
                tokenGroupsItem.push(...childrenObject[0]);
            }
            var languageNodes = languageTokens[tokensIndex].tokenGroup;
            console.log("tokenNodes length " + JSON.stringify(languageNodes));
            var tokensGroup = this.inspectNodesForTokenType(languageNodes, filePath);
            // check childres
            var richTokensItem = new language_token_syntax_1.LanguageTokenSyntaxGroup(tokenGroupsItem, tokensGroup[0]);
            richTokens.push(richTokensItem);
            diagnostics.push(...tokensGroup[1]);
        }
        return [richTokens, diagnostics];
    }
    inspectNodesForTokenType(node, filePath) {
        //let diagnostics: Diagnostic[] = [];
        if (node.length == 0) {
            console.log("found zero  ====  ");
            return [new language_token_syntax_1.LanguageTokenSyntax(syntax_kind_1.SyntaxKindType.UnknownDeclaration, []), []];
        }
        else {
            //console.log("Inspect language nodes  " + node.length + "confirm 0" + node[0]);
            //	var letters = /^[0-9a-zA-Z]+$/;
            // check if containts package 
            if (node[0].value == "package") {
                var erros = new package_analyzer_1.PackageAnalyzer().packageRules(node, filePath);
                var syntax = new package_analyzer_1.PackageAnalyzer().createPackageTokenSytax(node);
                console.log("package found errors  ====  " + erros.length);
                return [syntax, erros];
            }
            else if (node[0].value == "import") {
                var erros = new import_analyzer_1.ImportAnalyzer().importRules(node, filePath);
                console.log("import found errors  ====  " + erros.length);
                return [new language_token_syntax_1.LanguageTokenSyntax(syntax_kind_1.SyntaxKindType.UnknownDeclaration, []), erros];
            }
            else if (this.containsKeyword(node, "function")) {
                return [new language_token_syntax_1.LanguageTokenSyntax(syntax_kind_1.SyntaxKindType.UnknownDeclaration, []), []];
            }
            else if (this.containsKeyword(node, "var")) {
                return [new language_token_syntax_1.LanguageTokenSyntax(syntax_kind_1.SyntaxKindType.UnknownDeclaration, []), []];
            }
            else if (this.containsKeyword(node, "val")) {
                return [new language_token_syntax_1.LanguageTokenSyntax(syntax_kind_1.SyntaxKindType.UnknownDeclaration, []), []];
            }
            else if (this.containsKeyword(node, "let")) {
                return [new language_token_syntax_1.LanguageTokenSyntax(syntax_kind_1.SyntaxKindType.UnknownDeclaration, []), []];
            }
            else if (node[0] == undefined) {
                return [new language_token_syntax_1.LanguageTokenSyntax(syntax_kind_1.SyntaxKindType.UnknownDeclaration, []), []];
            }
            else {
                return [new language_token_syntax_1.LanguageTokenSyntax(syntax_kind_1.SyntaxKindType.UnknownDeclaration, []), []];
            }
        }
    }
    containsKeyword(node, word) {
        var contains = false;
        for (let nodeItem = 0; nodeItem < node.length; nodeItem++) {
            if (node[nodeItem].value == word) {
                contains = true;
                break;
            }
        }
        return contains;
    }
}
exports.LanguageSynthesis = LanguageSynthesis;
//# sourceMappingURL=synthesis.js.map