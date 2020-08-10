
package com.araizen.stratos.doc;

import com.intellij.lexer.DocCommentTokenTypes;
//import com.intellij.psi.StratosDocTokenType;
import com.intellij.psi.tree.IElementType;
import com.intellij.psi.tree.TokenSet;

/**
 * @author Kamau
 */
public class StratosDocTokenTypes implements DocCommentTokenTypes {
    public static final DocCommentTokenTypes INSTANCE = new StratosDocTokenTypes();
    private final TokenSet mySpaceCommentsSet = TokenSet.create(StratosDocTokenType.DOC_SPACE, StratosDocTokenType.DOC_COMMENT_DATA);

    private StratosDocTokenTypes() { }

    @Override
    public IElementType commentStart() {
        return StratosDocTokenType.DOC_COMMENT_START;
    }

    @Override
    public IElementType commentEnd() {
        return StratosDocTokenType.DOC_COMMENT_END;
    }

    @Override
    public IElementType commentData() {
        return StratosDocTokenType.DOC_COMMENT_DATA;
    }

    @Override
    public TokenSet spaceCommentsTokenSet() {
        return mySpaceCommentsSet;
    }

    @Override
    public IElementType space() {
        return StratosDocTokenType.DOC_SPACE;
    }

    @Override
    public IElementType tagValueToken() {
        return StratosDocTokenType.DOC_TAG_VALUE_TOKEN;
    }

    @Override
    public IElementType tagValueLParen() {
        return StratosDocTokenType.DOC_TAG_VALUE_LPAREN;
    }

    @Override
    public IElementType tagValueRParen() {
        return StratosDocTokenType.DOC_TAG_VALUE_RPAREN;
    }

    @Override
    public IElementType tagValueSharp() {
        return StratosDocTokenType.DOC_TAG_VALUE_SHARP_TOKEN;
    }

    @Override
    public IElementType tagValueComma() {
        return StratosDocTokenType.DOC_TAG_VALUE_COMMA;
    }

    @Override
    public IElementType tagName() {
        return StratosDocTokenType.DOC_TAG_NAME;
    }

    @Override
    public IElementType tagValueLT() {
        return StratosDocTokenType.DOC_TAG_VALUE_LT;
    }

    @Override
    public IElementType tagValueGT() {
        return StratosDocTokenType.DOC_TAG_VALUE_GT;
    }

    @Override
    public IElementType inlineTagStart() {
        return StratosDocTokenType.DOC_INLINE_TAG_START;
    }

    @Override
    public IElementType inlineTagEnd() {
        return StratosDocTokenType.DOC_INLINE_TAG_END;
    }

    @Override
    public IElementType badCharacter() {
        return StratosDocTokenType.DOC_COMMENT_BAD_CHARACTER;
    }

    @Override
    public IElementType commentLeadingAsterisks() {
        return StratosDocTokenType.DOC_COMMENT_LEADING_ASTERISKS;
    }
}
