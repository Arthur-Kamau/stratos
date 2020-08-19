package com.araizen.stratos.highlighter;


        import com.araizen.stratos.documentation.StratosDocElementType;
        import com.araizen.stratos.documentation.StratosDocTokenType;
        import com.araizen.stratos.parser.StratosParserDefinition;
        import com.araizen.stratos.psi.StratosTokenType;
        import com.intellij.lexer.HtmlHighlightingLexer;
        import com.intellij.lexer.LayeredLexer;
        import  com.araizen.stratos.language.LanguageLevel;

        import com.intellij.lexer.StringLiteralLexer;

        import com.intellij.psi.tree.IElementType;
        import org.jetbrains.annotations.NotNull;

public class StratosHighlightingLexer extends LayeredLexer {
    public StratosHighlightingLexer(@NotNull LanguageLevel languageLevel) {
        super(StratosParserDefinition.createLexer(languageLevel));

        registerSelfStoppingLayer(new StringLiteralLexer('\"', StratosTokenType.STRING_LITERAL, false, "s"),
                new IElementType[]{StratosTokenType.STRING_LITERAL}, IElementType.EMPTY_ARRAY);

        registerSelfStoppingLayer(new StringLiteralLexer('\'', StratosTokenType.STRING_LITERAL),
                new IElementType[]{StratosTokenType.CHARACTER_LITERAL}, IElementType.EMPTY_ARRAY);

        registerSelfStoppingLayer(new StringLiteralLexer(StringLiteralLexer.NO_QUOTE_CHAR, StratosTokenType.TEXT_BLOCK_LITERAL, true, "s"),
                new IElementType[]{StratosTokenType.TEXT_BLOCK_LITERAL}, IElementType.EMPTY_ARRAY);

        LayeredLexer docLexer = new LayeredLexer(StratosParserDefinition.createDocLexer(languageLevel));
        HtmlHighlightingLexer htmlLexer = new HtmlHighlightingLexer(null);
        htmlLexer.setHasNoEmbeddments(true);
        docLexer.registerLayer(htmlLexer, StratosDocTokenType.DOC_COMMENT_DATA);
        registerSelfStoppingLayer(docLexer, new IElementType[]{StratosDocElementType.DOC_COMMENT}, IElementType.EMPTY_ARRAY);
    }
}