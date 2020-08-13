package com.araizen.stratos.highlighter;

import com.araizen.stratos.documentation.StratosDocElementType;
import com.araizen.stratos.documentation.StratosDocTokenType;
import com.araizen.stratos.language.LanguageLevel;
import com.araizen.stratos.parser.StratosElementType;
import com.araizen.stratos.psi.StratosTokenType;
import com.araizen.stratos.psi.TokenType;
import com.intellij.lexer.Lexer;
import com.intellij.openapi.editor.HighlighterColors;
import com.intellij.openapi.editor.colors.TextAttributesKey;
import com.intellij.openapi.fileTypes.SyntaxHighlighterBase;
import com.intellij.psi.StringEscapesTokenTypes;
import com.intellij.psi.tree.IElementType;
import com.intellij.psi.xml.XmlTokenType;
import org.jetbrains.annotations.NotNull;

import java.util.HashMap;
import java.util.Map;


public class StratosFileHighlighter extends SyntaxHighlighterBase {
    private static final Map<IElementType, TextAttributesKey> ourMap1;
    private static final Map<IElementType, TextAttributesKey> ourMap2;

    static {
        ourMap1 = new HashMap<>();
        ourMap2 = new HashMap<>();

        fillMap(ourMap1, StratosElementType.KEYWORD_BIT_SET, StratosHighlightingColors.KEYWORD);
        fillMap(ourMap1, StratosElementType.LITERAL_BIT_SET, StratosHighlightingColors.KEYWORD);
        fillMap(ourMap1, StratosElementType.OPERATION_BIT_SET, StratosHighlightingColors.OPERATION_SIGN);

        for (IElementType type : StratosDocTokenType.ALL_JAVADOC_TOKENS.getTypes()) {
            ourMap1.put(type, StratosHighlightingColors.DOC_COMMENT);
        }

        ourMap1.put(XmlTokenType.XML_DATA_CHARACTERS, StratosHighlightingColors.DOC_COMMENT);
        ourMap1.put(XmlTokenType.XML_REAL_WHITE_SPACE, StratosHighlightingColors.DOC_COMMENT);
        ourMap1.put(XmlTokenType.TAG_WHITE_SPACE, StratosHighlightingColors.DOC_COMMENT);

        ourMap1.put(StratosTokenType.INTEGER_LITERAL, StratosHighlightingColors.NUMBER);
        ourMap1.put(StratosTokenType.LONG_LITERAL, StratosHighlightingColors.NUMBER);
        ourMap1.put(StratosTokenType.FLOAT_LITERAL, StratosHighlightingColors.NUMBER);
        ourMap1.put(StratosTokenType.DOUBLE_LITERAL, StratosHighlightingColors.NUMBER);
        ourMap1.put(StratosTokenType.STRING_LITERAL, StratosHighlightingColors.STRING);
        ourMap1.put(StratosTokenType.TEXT_BLOCK_LITERAL, StratosHighlightingColors.STRING);
        ourMap1.put(StringEscapesTokenTypes.VALID_STRING_ESCAPE_TOKEN, StratosHighlightingColors.VALID_STRING_ESCAPE);
        ourMap1.put(StringEscapesTokenTypes.INVALID_CHARACTER_ESCAPE_TOKEN, StratosHighlightingColors.INVALID_STRING_ESCAPE);
        ourMap1.put(StringEscapesTokenTypes.INVALID_UNICODE_ESCAPE_TOKEN, StratosHighlightingColors.INVALID_STRING_ESCAPE);
        ourMap1.put(StratosTokenType.CHARACTER_LITERAL, StratosHighlightingColors.STRING);

        ourMap1.put(StratosTokenType.LPARENTH, StratosHighlightingColors.PARENTHESES);
        ourMap1.put(StratosTokenType.RPARENTH, StratosHighlightingColors.PARENTHESES);

        ourMap1.put(StratosTokenType.LBRACE, StratosHighlightingColors.BRACES);
        ourMap1.put(StratosTokenType.RBRACE, StratosHighlightingColors.BRACES);

        ourMap1.put(StratosTokenType.LBRACKET, StratosHighlightingColors.BRACKETS);
        ourMap1.put(StratosTokenType.RBRACKET, StratosHighlightingColors.BRACKETS);

        ourMap1.put(StratosTokenType.COMMA, StratosHighlightingColors.COMMA);
        ourMap1.put(StratosTokenType.DOT, StratosHighlightingColors.DOT);
        ourMap1.put(StratosTokenType.SEMICOLON, StratosHighlightingColors.JAVA_SEMICOLON);

        ourMap1.put(StratosTokenType.C_STYLE_COMMENT, StratosHighlightingColors.JAVA_BLOCK_COMMENT);
        ourMap1.put(StratosDocElementType.DOC_COMMENT, StratosHighlightingColors.DOC_COMMENT);
        ourMap1.put(StratosTokenType.END_OF_LINE_COMMENT, StratosHighlightingColors.LINE_COMMENT);
        ourMap1.put(TokenType.BAD_CHARACTER, HighlighterColors.BAD_CHARACTER);

        ourMap1.put(StratosDocTokenType.DOC_TAG_NAME, StratosHighlightingColors.DOC_COMMENT);
        ourMap2.put(StratosDocTokenType.DOC_TAG_NAME, StratosHighlightingColors.DOC_COMMENT_TAG);

        IElementType[] javaDocMarkup = {
                XmlTokenType.XML_START_TAG_START, XmlTokenType.XML_END_TAG_START, XmlTokenType.XML_TAG_END, XmlTokenType.XML_EMPTY_ELEMENT_END,
                XmlTokenType.TAG_WHITE_SPACE, XmlTokenType.XML_TAG_NAME, XmlTokenType.XML_NAME, XmlTokenType.XML_ATTRIBUTE_VALUE_TOKEN,
                XmlTokenType.XML_ATTRIBUTE_VALUE_START_DELIMITER, XmlTokenType.XML_ATTRIBUTE_VALUE_END_DELIMITER, XmlTokenType.XML_CHAR_ENTITY_REF,
                XmlTokenType.XML_ENTITY_REF_TOKEN, XmlTokenType.XML_EQ
        };
        for (IElementType idx : javaDocMarkup) {
            ourMap1.put(idx, StratosHighlightingColors.DOC_COMMENT);
            ourMap2.put(idx, StratosHighlightingColors.DOC_COMMENT_MARKUP);
        }
    }

    protected final LanguageLevel myLanguageLevel;

    public StratosFileHighlighter() {
        this(LanguageLevel.HIGHEST);
    }

    public StratosFileHighlighter(@NotNull LanguageLevel languageLevel) {
        myLanguageLevel = languageLevel;
    }

    @Override
    @NotNull
    public Lexer getHighlightingLexer() {
        return new StratosHighlightingLexer(myLanguageLevel);
    }

    @Override
    public TextAttributesKey @NotNull [] getTokenHighlights(IElementType tokenType) {
        return pack(ourMap1.get(tokenType), ourMap2.get(tokenType));
    }
}