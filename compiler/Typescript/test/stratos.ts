export default class StratosLanguage {
    check(fileData: string) {
        console.log("File data "+ fileData);
        var antlr4 = require('antlr4');
        var MyGrammarLexer = require('./generated/StratosLexer.js').StratosLexer;
        var MyGrammarParser = require('./generated/StratosParser.js').StratosParser;
        var MyGrammarListener = require('./generated/StratosParserListener.js').StratosParserListener;

        var input = "your text to parse here"
        var chars = new antlr4.InputStream(input);
        var lexer = new MyGrammarLexer(chars);
        var tokens = new antlr4.CommonTokenStream(lexer);
        var parser = new MyGrammarParser(tokens);
        parser.buildParseTrees = true;
        var tree = parser.MyStartRule();

        console.log("tree "+JSON.stringify(tree));
    }
}