package org.stratos.lang;

import com.intellij.lang.ASTNode;
import com.intellij.lang.ParserDefinition;
import com.intellij.lang.PsiParser;
import com.intellij.lexer.Lexer;
import com.intellij.openapi.project.Project;
import com.intellij.psi.FileViewProvider;
import com.intellij.psi.PsiElement;
import com.intellij.psi.PsiFile;
import com.intellij.psi.tree.IElementType;
import com.intellij.psi.tree.IFileElementType;
import com.intellij.psi.tree.TokenSet;
import org.antlr.intellij.adaptor.lexer.ANTLRLexerAdaptor;
import org.antlr.intellij.adaptor.lexer.PSIElementTypeFactory;
import org.antlr.intellij.adaptor.lexer.RuleIElementType;
import org.antlr.intellij.adaptor.lexer.TokenIElementType;
import org.antlr.intellij.adaptor.parser.ANTLRParserAdaptor;
import org.antlr.intellij.adaptor.psi.ANTLRPsiNode;
import org.antlr.jetbrains.sample.parser.StratosLanguageParser;
import org.antlr.jetbrains.sample.parser.StratosLanguageLexer;
import org.stratos.lang.psi.ArgdefSubtree;
import org.stratos.lang.psi.BlockSubtree;
import org.stratos.lang.psi.CallSubtree;
import org.stratos.lang.psi.FunctionSubtree;
import org.stratos.lang.psi.SamplePSIFileRoot;
import org.stratos.lang.psi.VardefSubtree;
import org.antlr.v4.runtime.Parser;
import org.antlr.v4.runtime.tree.ParseTree;
import org.jetbrains.annotations.NotNull;

import java.util.List;

public class StratosParserDefinition implements ParserDefinition {
	public static final IFileElementType FILE =
		new IFileElementType(StratosLanguage.INSTANCE);

	public static TokenIElementType ID;

	static {
		PSIElementTypeFactory.defineLanguageIElementTypes(StratosLanguage.INSTANCE,
		                                                  StratosLanguageParser.tokenNames,
		                                                  StratosLanguageParser.ruleNames);
		List<TokenIElementType> tokenIElementTypes =
			PSIElementTypeFactory.getTokenIElementTypes(StratosLanguage.INSTANCE);
		ID = tokenIElementTypes.get(StratosLanguageLexer.ID);
	}

	public static final TokenSet COMMENTS =
		PSIElementTypeFactory.createTokenSet(
			StratosLanguage.INSTANCE,
			StratosLanguageLexer.BLOCK_COMMENT,
			StratosLanguageLexer.LINE_COMMENT);

	public static final TokenSet WHITESPACE =
		PSIElementTypeFactory.createTokenSet(
			StratosLanguage.INSTANCE,
			StratosLanguageLexer.WS);

	public static final TokenSet STRING =
		PSIElementTypeFactory.createTokenSet(
			StratosLanguage.INSTANCE,
			StratosLanguageLexer.STRING);

	@NotNull
	@Override
	public Lexer createLexer(Project project) {
		StratosLanguageLexer lexer = new StratosLanguageLexer(null);
		return new ANTLRLexerAdaptor(StratosLanguage.INSTANCE, lexer);
	}

	@NotNull
	public PsiParser createParser(final Project project) {
		final StratosLanguageParser parser = new StratosLanguageParser(null);
		return new ANTLRParserAdaptor(StratosLanguage.INSTANCE, parser) {
			@Override
			protected ParseTree parse(Parser parser, IElementType root) {
				if ( root instanceof IFileElementType ) {
					return ((StratosLanguageParser) parser).script();
				}
				return ((StratosLanguageParser) parser).primary();
			}
		};
	}

	@NotNull
	public TokenSet getWhitespaceTokens() {
		return WHITESPACE;
	}

	@NotNull
	public TokenSet getCommentTokens() {
		return COMMENTS;
	}

	@NotNull
	public TokenSet getStringLiteralElements() {
		return STRING;
	}

	public SpaceRequirements spaceExistanceTypeBetweenTokens(ASTNode left, ASTNode right) {
		return SpaceRequirements.MAY;
	}

	@Override
	public IFileElementType getFileNodeType() {
		return FILE;
	}

	@Override
	public PsiFile createFile(FileViewProvider viewProvider) {
		return new SamplePSIFileRoot(viewProvider);
	}

	@NotNull
	public PsiElement createElement(ASTNode node) {
		IElementType elType = node.getElementType();
		if ( elType instanceof TokenIElementType ) {
			return new ANTLRPsiNode(node);
		}
		if ( !(elType instanceof RuleIElementType) ) {
			return new ANTLRPsiNode(node);
		}
		RuleIElementType ruleElType = (RuleIElementType) elType;
		switch ( ruleElType.getRuleIndex() ) {
			case StratosLanguageParser.RULE_fnDecl :
				return new FunctionSubtree(node, elType);
			case StratosLanguageParser.RULE_varDecl :
				return new VardefSubtree(node, elType);
			case StratosLanguageParser.RULE_param :
				return new ArgdefSubtree(node, elType);
			case StratosLanguageParser.RULE_block :
				return new BlockSubtree(node);
            // CallExpr logic might need adjustment if using CallSubtree
			// case StratosLanguageParser.RULE_call_expr :
			// 	return new CallSubtree(node);
			default :
				return new ANTLRPsiNode(node);
		}
	}
}