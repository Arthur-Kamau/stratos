package lang.stratos.language;


//import com.intellij.ide.highlighter.EmbeddedTokenHighlighter;
//import com.intellij.lexer.Lexer;
//import com.intellij.openapi.editor.colors.TextAttributesKey;
//import com.intellij.openapi.fileTypes.SyntaxHighlighterBase;
//import com.intellij.psi.tree.IElementType;
//import com.intellij.util.containers.MultiMap;
//import lang.stratos.grammer.lexer.StratosLexerAdapter;
//import lang.stratos.grammer.types.StratosTokenTypesSets;
//import lang.stratos.grammer.types.StratosTypes;
//import org.jetbrains.annotations.NotNull;

import java.util.HashMap;
import java.util.Map;

//        import org.intellij.sdk.language.psi.StratosTypes;
//import lang.stratos.grammer.StratosLexer;
//import lang.stratos.grammer.lexer.StratosLexer;
//import lang.stratos.grammer.StratosTypes;
//import static lang.stratos.grammer.types.StratosTokenTypesSets.*;
public class StratosSyntaxHighlighter {
// extends SyntaxHighlighterBase implements EmbeddedTokenHighlighter {

//    private static final Map<IElementType, TextAttributesKey> ATTRIBUTES = new HashMap<>();
//   static   StratosTokenTypesSets t = new StratosTokenTypesSets();
//    static {
////        fillMap(ATTRIBUTES, RESERVED_WORDS, StratosSyntaxHighlighterColors.KEYWORD);
////
////        fillMap(ATTRIBUTES, ASSIGNMENT_OPERATORS, StratosSyntaxHighlighterColors.OPERATION_SIGN);
////        fillMap(ATTRIBUTES, BINARY_OPERATORS, StratosSyntaxHighlighterColors.OPERATION_SIGN);
////        fillMap(ATTRIBUTES, UNARY_OPERATORS, StratosSyntaxHighlighterColors.OPERATION_SIGN);
////        // '?' from ternary operator; ':' is handled separately in dartColorAnnotator, because there are also ':' in other syntax constructs
//        ATTRIBUTES.put(StratosTypes.QUEST, StratosSyntaxHighlighterColors.OPERATION_SIGN);
////
////        fillMap(ATTRIBUTES, STRINGS, StratosSyntaxHighlighterColors.STRING);
//
//        ATTRIBUTES.put(StratosTypes.HEX_NUMBER, StratosSyntaxHighlighterColors.NUMBER);
//        ATTRIBUTES.put(StratosTypes.NUMBER, StratosSyntaxHighlighterColors.NUMBER);
//
//
//        ATTRIBUTES.put(StratosTypes.LPAREN, StratosSyntaxHighlighterColors.PARENTHS);
//        ATTRIBUTES.put(StratosTypes.RPAREN, StratosSyntaxHighlighterColors.PARENTHS);
//
//        ATTRIBUTES.put(StratosTypes.LBRACE, StratosSyntaxHighlighterColors.BRACES);
//        ATTRIBUTES.put(StratosTypes.RBRACE, StratosSyntaxHighlighterColors.BRACES);
//        ATTRIBUTES.put(StratosTypes.SHORT_TEMPLATE_ENTRY_START, StratosSyntaxHighlighterColors.BRACES);
//        ATTRIBUTES.put(StratosTypes.LONG_TEMPLATE_ENTRY_START, StratosSyntaxHighlighterColors.BRACES);
//        ATTRIBUTES.put(StratosTypes.LONG_TEMPLATE_ENTRY_END, StratosSyntaxHighlighterColors.BRACES);
//
//        ATTRIBUTES.put(StratosTypes.LBRACKET, StratosSyntaxHighlighterColors.BRACKETS);
//        ATTRIBUTES.put(StratosTypes.RBRACKET, StratosSyntaxHighlighterColors.BRACKETS);
//
//        ATTRIBUTES.put(StratosTypes.COMMA, StratosSyntaxHighlighterColors.COMMA);
//        ATTRIBUTES.put(StratosTypes.DOT, StratosSyntaxHighlighterColors.DOT);
//        ATTRIBUTES.put(StratosTypes.DOT_DOT, StratosSyntaxHighlighterColors.DOT);
//        ATTRIBUTES.put(StratosTypes.QUEST_DOT_DOT, StratosSyntaxHighlighterColors.DOT);
//        ATTRIBUTES.put(StratosTypes.QUEST_DOT, StratosSyntaxHighlighterColors.DOT);
//        ATTRIBUTES.put(StratosTypes.SEMICOLON, StratosSyntaxHighlighterColors.SEMICOLON);
//        ATTRIBUTES.put(StratosTypes.COLON, StratosSyntaxHighlighterColors.COLON);
//        ATTRIBUTES.put(StratosTypes.EXPRESSION_BODY_DEF, StratosSyntaxHighlighterColors.FAT_ARROW);
//
//        ATTRIBUTES.put(StratosTypes.SINGLE_LINE_COMMENT, StratosSyntaxHighlighterColors.LINE_COMMENT);
//        ATTRIBUTES.put(StratosTypes.SINGLE_LINE_DOC_COMMENT, StratosSyntaxHighlighterColors.DOC_COMMENT);
//        ATTRIBUTES.put(StratosTypes.MULTI_LINE_COMMENT, StratosSyntaxHighlighterColors.BLOCK_COMMENT);
////        ATTRIBUTES.put(StratosTypes.MULTI_LINE_DOC_COMMENT, StratosSyntaxHighlighterColors.DOC_COMMENT);
//
//        ATTRIBUTES.put(StratosTypes.BAD_CHARACTER, StratosSyntaxHighlighterColors.BAD_CHARACTER);
//    }
//
//    @Override
//    @NotNull
//    public Lexer getHighlightingLexer() {
//        return new StratosLexerAdapter();
//    }
//
//    @Override
//    public TextAttributesKey[] getTokenHighlights(IElementType tokenType) {
//        return pack(ATTRIBUTES.get(tokenType));
//    }
//
//    @NotNull
//    @Override
//    public MultiMap<IElementType, TextAttributesKey> getEmbeddedTokenAttributes() {
//        MultiMap<IElementType, TextAttributesKey> map = MultiMap.create();
//        map.putAllValues(ATTRIBUTES);
//        return map;
//    }

//    public static final TextAttributesKey SEPARATOR =
//            createTextAttributesKey("SIMPLE_SEPARATOR", DefaultLanguageHighlighterColors.OPERATION_SIGN);
//    public static final TextAttributesKey KEY =
//            createTextAttributesKey("SIMPLE_KEY", DefaultLanguageHighlighterColors.KEYWORD);
//    public static final TextAttributesKey VALUE =
//            createTextAttributesKey("SIMPLE_VALUE", DefaultLanguageHighlighterColors.STRING);
//    public static final TextAttributesKey COMMENT =
//            createTextAttributesKey("SIMPLE_COMMENT", DefaultLanguageHighlighterColors.LINE_COMMENT);
//    public static final TextAttributesKey BAD_CHARACTER =
//            createTextAttributesKey("SIMPLE_BAD_CHARACTER", HighlighterColors.BAD_CHARACTER);
//
//
//    private static final TextAttributesKey[] BAD_CHAR_KEYS = new TextAttributesKey[]{BAD_CHARACTER};
//    private static final TextAttributesKey[] SEPARATOR_KEYS = new TextAttributesKey[]{SEPARATOR};
//    private static final TextAttributesKey[] KEY_KEYS = new TextAttributesKey[]{KEY};
//    private static final TextAttributesKey[] VALUE_KEYS = new TextAttributesKey[]{VALUE};
//    private static final TextAttributesKey[] COMMENT_KEYS = new TextAttributesKey[]{COMMENT};
//    private static final TextAttributesKey[] EMPTY_KEYS = new TextAttributesKey[0];
//
//    @NotNull
//    @Override
//    public Lexer getHighlightingLexer() {
//        return new StratosLexerAdapter();
//    }
//
//    @NotNull
//    @Override
//    public TextAttributesKey[] getTokenHighlights(IElementType tokenType) {
//        if (tokenType.equals(StratosTypes.SEPARATOR)) {
//            return SEPARATOR_KEYS;
//        } else if (tokenType.equals(StratosTypes.KEY)) {
//            return KEY_KEYS;
//        } else if (tokenType.equals(StratosTypes.VALUE)) {
//            return VALUE_KEYS;
//        } else if (tokenType.equals(StratosTypes.COMMENT)) {
//            return COMMENT_KEYS;
//        } else if (tokenType.equals(TokenType.BAD_CHARACTER)) {
//            return BAD_CHAR_KEYS;
//        } else {
//            return EMPTY_KEYS;
//        }
//    }
}