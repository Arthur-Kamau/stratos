package com.araizen.stratos.doc;


import com.intellij.psi.tree.IElementType;
        import com.intellij.psi.tree.TokenSet;



public interface StratosDocTokenType {
    IElementType DOC_COMMENT_START = new IStratosDocElementType("DOC_COMMENT_START");
    IElementType DOC_COMMENT_END = new IStratosDocElementType("DOC_COMMENT_END");
    IElementType DOC_COMMENT_DATA = new IStratosDocElementType("DOC_COMMENT_DATA");
    IElementType DOC_SPACE = new IStratosDocElementType("DOC_SPACE");
    IElementType DOC_COMMENT_LEADING_ASTERISKS = new IStratosDocElementType("DOC_COMMENT_LEADING_ASTERISKS");
    IElementType DOC_TAG_NAME = new IStratosDocElementType("DOC_TAG_NAME");
    IElementType DOC_INLINE_TAG_START = new IStratosDocElementType("DOC_INLINE_TAG_START");
    IElementType DOC_INLINE_TAG_END = new IStratosDocElementType("DOC_INLINE_TAG_END");

    IElementType DOC_TAG_VALUE_TOKEN = new IStratosDocElementType("DOC_TAG_VALUE_TOKEN");
    IElementType DOC_TAG_VALUE_DOT = new IStratosDocElementType("DOC_TAG_VALUE_DOT");
    IElementType DOC_TAG_VALUE_COMMA = new IStratosDocElementType("DOC_TAG_VALUE_COMMA");
    IElementType DOC_TAG_VALUE_LPAREN = new IStratosDocElementType("DOC_TAG_VALUE_LPAREN");
    IElementType DOC_TAG_VALUE_RPAREN = new IStratosDocElementType("DOC_TAG_VALUE_RPAREN");
    IElementType DOC_TAG_VALUE_LT = new IStratosDocElementType("DOC_TAG_VALUE_LT");
    IElementType DOC_TAG_VALUE_GT = new IStratosDocElementType("DOC_TAG_VALUE_GT");
    IElementType DOC_TAG_VALUE_SHARP_TOKEN = new IStratosDocElementType("DOC_TAG_VALUE_SHARP_TOKEN");

    IElementType DOC_COMMENT_BAD_CHARACTER = new IStratosDocElementType("DOC_COMMENT_BAD_CHARACTER");

    TokenSet ALL_JAVADOC_TOKENS = TokenSet.create(
            DOC_COMMENT_START, DOC_COMMENT_END, DOC_COMMENT_DATA, DOC_SPACE, DOC_COMMENT_LEADING_ASTERISKS, DOC_TAG_NAME,
            DOC_INLINE_TAG_START, DOC_INLINE_TAG_END, DOC_TAG_VALUE_TOKEN, DOC_TAG_VALUE_DOT, DOC_TAG_VALUE_COMMA,
            DOC_TAG_VALUE_LPAREN, DOC_TAG_VALUE_RPAREN, DOC_TAG_VALUE_SHARP_TOKEN
    );
}