package lang.stratos.parser;


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
//import com.jetbrains.lang.dart.lexer.DartLexer;
//import com.jetbrains.lang.dart.psi.DartFile;
//import com.jetbrains.lang.dart.psi.impl.DartDocCommentImpl;
//import com.jetbrains.lang.dart.psi.impl.DartEmbeddedContentImpl;
import lang.stratos.language.StratosFile;
import lang.stratos.lexer.StratosLexer;
import lang.stratos.psi.StratosTypes;
import org.jetbrains.annotations.NotNull;

public class StratosParserDefinition implements ParserDefinition {

    @NotNull
    @Override
    public Lexer createLexer(Project project) {
        return new StratosLexer();
    }

    @Override
    public PsiParser createParser(Project project) {
        return new StratosParser();
    }

    @Override
    public IFileElementType getFileNodeType() {
        return  StratosTypes.STRATOS_FILE; // DartTokenTypesSets.DART_FILE;
    }

    @NotNull
    @Override
    public TokenSet getWhitespaceTokens() {
        return StratosTypes.WHITE_SPACES;
    }

    @NotNull
    @Override
    public TokenSet getCommentTokens() {
        return StratosTypes.COMMENTS;
    }

    @NotNull
    @Override
    public TokenSet getStringLiteralElements() {
        return TokenSet.create(
                StratosTypes.RAW_SINGLE_QUOTED_STRING,
                StratosTypes.RAW_TRIPLE_QUOTED_STRING,
                StratosTypes.OPEN_QUOTE,
                StratosTypes.CLOSING_QUOTE,
                StratosTypes.REGULAR_STRING_PART
        );
    }

    @NotNull
    @Override
    public PsiElement createElement(ASTNode node) {
        final IElementType type = node.getElementType();


        return StratosTypes.Factory.createElement(node);
    }

    @Override
    public PsiFile createFile(FileViewProvider viewProvider) {
        return new StratosFile(viewProvider);
    }

    @Override
    public SpaceRequirements spaceExistenceTypeBetweenTokens(ASTNode left, ASTNode right) {
        return SpaceRequirements.MAY;
    }
}
