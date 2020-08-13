
package com.araizen.stratos.documentation;

import com.araizen.stratos.language.LanguageLevel;
import com.intellij.lexer.DocCommentTokenTypes;
import com.intellij.lexer.LexerBase;
import com.intellij.lexer.MergingLexerAdapter;
import com.intellij.psi.tree.IElementType;
import com.intellij.util.text.CharArrayUtil;
import org.jetbrains.annotations.NotNull;

import java.io.IOException;

//public class StratosDocLexer{
public class StratosDocLexer extends MergingLexerAdapter {
    public StratosDocLexer(@NotNull LanguageLevel level) {
        this(StratosDocTokenTypes.INSTANCE, level.isAtLeast(LanguageLevel.CP_0_1));
    }

    private StratosDocLexer(DocCommentTokenTypes tokenTypes, boolean isJdk15Enabled) {
        super(new AsteriskStripperLexer(new _StratosDocLexer(isJdk15Enabled, tokenTypes), tokenTypes), tokenTypes.spaceCommentsTokenSet());
    }

    private static class AsteriskStripperLexer extends LexerBase {
        private final _StratosDocLexer myFlex;
        private final DocCommentTokenTypes myTokenTypes;
        private CharSequence myBuffer;
        private int myBufferIndex;
        private int myBufferEndOffset;
        private int myTokenEndOffset;
        private int myState;
        private IElementType myTokenType;
        private boolean myAfterLineBreak;
        private boolean myInLeadingSpace;

        AsteriskStripperLexer(final _StratosDocLexer flex, final DocCommentTokenTypes tokenTypes) {
            myFlex = flex;
            myTokenTypes = tokenTypes;
        }

        @Override
        public final void start(@NotNull CharSequence buffer, int startOffset, int endOffset, int initialState) {
            myBuffer = buffer;
            myBufferIndex = startOffset;
            myBufferEndOffset = endOffset;
            myTokenType = null;
            myTokenEndOffset = startOffset;
            myFlex.reset(myBuffer, startOffset, endOffset, initialState);
        }

        @Override
        public int getState() {
            return myState;
        }

        @NotNull
        @Override
        public CharSequence getBufferSequence() {
            return myBuffer;
        }

        @Override
        public int getBufferEnd() {
            return myBufferEndOffset;
        }

        @Override
        public final IElementType getTokenType() {
            locateToken();
            return myTokenType;
        }

        @Override
        public final int getTokenStart() {
            locateToken();
            return myBufferIndex;
        }

        @Override
        public final int getTokenEnd() {
            locateToken();
            return myTokenEndOffset;
        }

        @Override
        public final void advance() {
            locateToken();
            myTokenType = null;
        }

        protected final void locateToken() {
            if (myTokenType != null) return;
            _locateToken();

            if (myTokenType == myTokenTypes.space()) {
                myAfterLineBreak = CharArrayUtil.containLineBreaks(myBuffer, getTokenStart(), getTokenEnd());
            }
        }

        private void _locateToken() {
            if (myTokenEndOffset == myBufferEndOffset) {
                myTokenType = null;
                myBufferIndex = myBufferEndOffset;
                return;
            }

            myBufferIndex = myTokenEndOffset;

            if (myAfterLineBreak) {
                myAfterLineBreak = false;
                while (myTokenEndOffset < myBufferEndOffset && myBuffer.charAt(myTokenEndOffset) == '*' &&
                        (myTokenEndOffset + 1 >= myBufferEndOffset || myBuffer.charAt(myTokenEndOffset + 1) != '/')) {
                    myTokenEndOffset++;
                }

                myInLeadingSpace = true;
                if (myBufferIndex < myTokenEndOffset) {
                    myTokenType = myTokenTypes.commentLeadingAsterisks();
                    return;
                }
            }

            if (myInLeadingSpace) {
                myInLeadingSpace = false;
                boolean lf = false;
                while (myTokenEndOffset < myBufferEndOffset && Character.isWhitespace(myBuffer.charAt(myTokenEndOffset))) {
                    if (myBuffer.charAt(myTokenEndOffset) == '\n') lf = true;
                    myTokenEndOffset++;
                }

                final int state = myFlex.yystate();
                if (state == _StratosDocLexer.COMMENT_DATA ||
                        myTokenEndOffset < myBufferEndOffset && (myBuffer.charAt(myTokenEndOffset) == '@' ||
                                myBuffer.charAt(myTokenEndOffset) == '{' ||
                                myBuffer.charAt(myTokenEndOffset) == '\"' ||
                                myBuffer.charAt(myTokenEndOffset) == '<')) {
                    myFlex.yybegin(_StratosDocLexer.COMMENT_DATA_START);
                }

                if (myBufferIndex < myTokenEndOffset) {
                    myTokenType = lf ||
                            state == _StratosDocLexer.PARAM_TAG_SPACE || state == _StratosDocLexer.TAG_DOC_SPACE ||
                            state == _StratosDocLexer.INLINE_TAG_NAME || state == _StratosDocLexer.DOC_TAG_VALUE_IN_PAREN
                            ? myTokenTypes.space() : myTokenTypes.commentData();

                    return;
                }
            }

            flexLocateToken();
        }

        private void flexLocateToken() {
            try {
                myState = myFlex.yystate();
                myFlex.goTo(myBufferIndex);
                myTokenType = myFlex.advance();
                myTokenEndOffset = myFlex.getTokenEnd();
            }
            catch (IOException e) {
                // Can't be
            }
        }
    }
}