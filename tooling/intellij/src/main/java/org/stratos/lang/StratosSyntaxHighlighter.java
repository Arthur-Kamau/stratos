package org.stratos.lang;

import com.intellij.lexer.Lexer;
import com.intellij.openapi.editor.DefaultLanguageHighlighterColors;
import com.intellij.openapi.editor.colors.TextAttributesKey;
import com.intellij.openapi.fileTypes.SyntaxHighlighterBase;
import com.intellij.psi.tree.IElementType;
import org.antlr.intellij.adaptor.lexer.ANTLRLexerAdaptor;
import org.antlr.intellij.adaptor.lexer.PSIElementTypeFactory;
import org.antlr.intellij.adaptor.lexer.TokenIElementType;
import org.antlr.jetbrains.sample.parser.StratosLanguageLexer;
import org.antlr.jetbrains.sample.parser.StratosLanguageParser;
import org.jetbrains.annotations.NotNull;
import static com.intellij.openapi.editor.colors.TextAttributesKey.createTextAttributesKey;

public class StratosSyntaxHighlighter extends SyntaxHighlighterBase {
	private static final TextAttributesKey[] EMPTY_KEYS = new TextAttributesKey[0];
	public static final TextAttributesKey ID =
		createTextAttributesKey("STRATOS_ID", DefaultLanguageHighlighterColors.IDENTIFIER);
	public static final TextAttributesKey KEYWORD =
		createTextAttributesKey("STRATOS_KEYWORD", DefaultLanguageHighlighterColors.KEYWORD);
	public static final TextAttributesKey STRING =
		createTextAttributesKey("STRATOS_STRING", DefaultLanguageHighlighterColors.STRING);
	public static final TextAttributesKey NUMBER =
		createTextAttributesKey("STRATOS_NUMBER", DefaultLanguageHighlighterColors.NUMBER);
	public static final TextAttributesKey COMMENT =
		createTextAttributesKey("STRATOS_COMMENT", DefaultLanguageHighlighterColors.LINE_COMMENT);
	public static final TextAttributesKey BLOCK_COMMENT =
		createTextAttributesKey("STRATOS_BLOCK_COMMENT", DefaultLanguageHighlighterColors.BLOCK_COMMENT);

	static {
		PSIElementTypeFactory.defineLanguageIElementTypes(StratosLanguage.INSTANCE,
		                                                  StratosLanguageParser.tokenNames,
		                                                  StratosLanguageParser.ruleNames);
	}

	@NotNull
	@Override
	public Lexer getHighlightingLexer() {
		StratosLanguageLexer lexer = new StratosLanguageLexer(null);
		return new ANTLRLexerAdaptor(StratosLanguage.INSTANCE, lexer);
	}

	@NotNull
	@Override
	public TextAttributesKey[] getTokenHighlights(IElementType tokenType) {
		if ( !(tokenType instanceof TokenIElementType) ) return EMPTY_KEYS;
		TokenIElementType myType = (TokenIElementType)tokenType;
		int ttype = myType.getANTLRTokenType();
		TextAttributesKey attrKey;
		switch ( ttype ) {
			case StratosLanguageLexer.ID :
				attrKey = ID;
				break;
			case StratosLanguageLexer.VAL :
			case StratosLanguageLexer.VAR :
			case StratosLanguageLexer.FN :
			case StratosLanguageLexer.CLASS :
			case StratosLanguageLexer.STRUCT :
			case StratosLanguageLexer.INTERFACE :
			case StratosLanguageLexer.ENUM :
			case StratosLanguageLexer.NAMESPACE :
			case StratosLanguageLexer.USE :
			case StratosLanguageLexer.CONSTRUCTOR :
			case StratosLanguageLexer.IF :
			case StratosLanguageLexer.ELSE :
			case StratosLanguageLexer.WHILE :
			case StratosLanguageLexer.RETURN :
			case StratosLanguageLexer.WHEN :
			case StratosLanguageLexer.TRUE :
			case StratosLanguageLexer.FALSE :
			case StratosLanguageLexer.NONE :
			case StratosLanguageLexer.SOME :
				attrKey = KEYWORD;
				break;
			case StratosLanguageLexer.STRING :
				attrKey = STRING;
				break;
			case StratosLanguageLexer.INT :
			case StratosLanguageLexer.FLOAT :
				attrKey = NUMBER;
				break;
			case StratosLanguageLexer.LINE_COMMENT :
				attrKey = COMMENT;
				break;
			case StratosLanguageLexer.BLOCK_COMMENT :
				attrKey = BLOCK_COMMENT;
				break;
			default :
				return EMPTY_KEYS;
		}
		return new TextAttributesKey[] {attrKey};
	}
}