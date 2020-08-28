package lang.stratos.grammer.types;

import com.intellij.lang.*;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.util.Pair;
import com.intellij.psi.PsiElement;
import com.intellij.psi.PsiFile;
import com.intellij.psi.TokenType;
import com.intellij.psi.text.BlockSupport;
import com.intellij.psi.tree.*;
import com.intellij.util.diff.FlyweightCapableTreeStructure;
import lang.stratos.documentation.StratosDocLexer;
import lang.stratos.documentation.StratosInHtmlLanguage;
import lang.stratos.grammer.StratosElementType;
import lang.stratos.grammer.lexer.StratosLexerAdapter;
import lang.stratos.grammer.parser.StratosParser;
import  lang.stratos.grammer.types.StratosTypes.*;
import lang.stratos.language.StratosLanguage;
import org.jetbrains.annotations.NotNull;

import javax.annotation.Nullable;

import static com.intellij.lang.parser.GeneratedParserUtilBase.*;

public class StratosTokenTypesSets {
    IFileElementType DART_FILE = new IFileElementType("STRATOSFILE", StratosLanguage.INSTANCE);

    IElementType MULTI_LINE_DOC_COMMENT = new StratosDocCommentElementType();

    IElementType LAZY_PARSEABLE_BLOCK = new StratosLazyParseableBlockElementType();

    TokenSet STRINGS = TokenSet.create(StratosTypes.RAW_SINGLE_QUOTED_STRING, StratosTypes.RAW_TRIPLE_QUOTED_STRING, StratosTypes.OPEN_QUOTE, StratosTypes.CLOSING_QUOTE, StratosTypes.REGULAR_STRING_PART);
    TokenSet WHITE_SPACES = TokenSet.create(StratosTypes.WHITE_SPACE);

    TokenSet RESERVED_WORDS = TokenSet.create(StratosTypes.ASSERT,
            StratosTypes.BREAK,
            StratosTypes.CASE,
            StratosTypes.CATCH,
            StratosTypes.CLASS,
            StratosTypes.CONST,
            StratosTypes.CONTINUE,
            StratosTypes.DEFAULT,
            StratosTypes.DO,
            StratosTypes.ELSE,
            StratosTypes.ENUM,
            StratosTypes.EXTENDS,
            StratosTypes.FALSE,
            StratosTypes.FINAL,
            StratosTypes.FINALLY,
            StratosTypes.FOR,
            StratosTypes.IF,
            StratosTypes.IN,
            StratosTypes.IS,
            StratosTypes.NEW,
            StratosTypes.NULL,
            StratosTypes.RETHROW,
            StratosTypes.RETURN,
            StratosTypes.SUPER,
            StratosTypes.SWITCH,
            StratosTypes.THIS,
            StratosTypes.THROW,
            StratosTypes.TRUE,
            StratosTypes.TRY,
            StratosTypes.VAR,
            StratosTypes.WHILE,
            StratosTypes.WITH,
            // 'void' is not listed as reserved word in spec but it may only be used as the return type of a function, so may be treated as reserved word
            StratosTypes.VOID);

    TokenSet BUILT_IN_IDENTIFIERS = TokenSet.create(StratosTypes.ABSTRACT,
            StratosTypes.AS,
            StratosTypes.COVARIANT,
            StratosTypes.DEFERRED,
            StratosTypes.EXPORT,
            StratosTypes.EXTENSION,
            StratosTypes.EXTERNAL,
            StratosTypes.FACTORY,
            StratosTypes.GET,
            StratosTypes.IMPLEMENTS,
            StratosTypes.IMPORT,
            StratosTypes.LIBRARY,
            StratosTypes.MIXIN,
            StratosTypes.OPERATOR,
            StratosTypes.PART,
            StratosTypes.SET,
            StratosTypes.STATIC,
            StratosTypes.TYPEDEF,
            // next are not listed in spec, but they seem to have the same sense as BUILT_IN_IDENTIFIERS: somewhere treated as keywords, but can be used as normal identifiers
            StratosTypes.ON,
            StratosTypes.OF,
            StratosTypes.NATIVE,
            StratosTypes.SHOW,
            StratosTypes.HIDE,
            StratosTypes.SYNC,
            StratosTypes.ASYNC,
            StratosTypes.AWAIT,
            StratosTypes.YIELD,
            StratosTypes.LATE,
            StratosTypes.REQUIRED);

    TokenSet OPERATORS = TokenSet.create(
            StratosTypes.MINUS, StratosTypes.MINUS_EQ, StratosTypes.MINUS_MINUS, StratosTypes.PLUS, StratosTypes.PLUS_PLUS, StratosTypes.PLUS_EQ, StratosTypes.DIV, StratosTypes.DIV_EQ, StratosTypes.MUL, StratosTypes.MUL_EQ, StratosTypes.INT_DIV, StratosTypes.INT_DIV_EQ, StratosTypes.REM_EQ, StratosTypes.REM, StratosTypes.BIN_NOT, StratosTypes.NOT,
            StratosTypes.EQ, StratosTypes.EQ_EQ, StratosTypes.NEQ, StratosTypes.GT, StratosTypes.GT_EQ, StratosTypes.GT_GT_EQ, StratosTypes.LT, StratosTypes.LT_EQ, StratosTypes.LT_LT, StratosTypes.LT_LT_EQ, StratosTypes.OR, StratosTypes.OR_EQ, StratosTypes.OR_OR, StratosTypes.OR_OR_EQ, StratosTypes.XOR, StratosTypes.XOR_EQ, StratosTypes.AND,
            StratosTypes.AND_EQ, StratosTypes.AND_AND, StratosTypes.AND_AND_EQ, StratosTypes.LBRACKET, StratosTypes.RBRACKET, StratosTypes.AS, StratosTypes.QUEST_QUEST, StratosTypes.QUEST_QUEST_EQ
    );

    TokenSet ASSIGNMENT_OPERATORS = TokenSet.create(
            // '=' | '*=' | '/=' | '~/=' | '%=' | '+=' | '-=' | '<<=' | '>>=' | '&=' | '&&=' | '^=' | '|=' | '||=' | '??='
            StratosTypes.EQ, StratosTypes.MUL_EQ, StratosTypes.DIV_EQ, StratosTypes.INT_DIV_EQ, StratosTypes.REM_EQ, StratosTypes.PLUS_EQ, StratosTypes.MINUS_EQ, StratosTypes.LT_LT_EQ, StratosTypes.GT_GT_EQ, StratosTypes.AND_EQ, StratosTypes.AND_AND_EQ, StratosTypes.XOR_EQ, StratosTypes.OR_EQ, StratosTypes.OR_OR_EQ,
            StratosTypes.QUEST_QUEST_EQ
    );

    TokenSet BINARY_EXPRESSIONS = TokenSet.create(
            StratosTypes.IF_NULL_EXPRESSION,
            StratosTypes.LOGIC_OR_EXPRESSION,
            StratosTypes.LOGIC_AND_EXPRESSION,
            StratosTypes.COMPARE_EXPRESSION,
            StratosTypes.SHIFT_EXPRESSION,
            StratosTypes.ADDITIVE_EXPRESSION,
            StratosTypes.MULTIPLICATIVE_EXPRESSION
    );

    TokenSet BINARY_OPERATORS = TokenSet.create(
            // '??
            StratosTypes.QUEST_QUEST,
            // '&&' '||'
            StratosTypes.AND_AND, StratosTypes.OR_OR,
            // '==' '!='
            StratosTypes.EQ_EQ, StratosTypes.NEQ,
            // '<' '<=' '>' '>='
            StratosTypes.LT, StratosTypes.LT_EQ, StratosTypes.GT, StratosTypes.GT_EQ,
            // '&' '|' '^'
            StratosTypes.AND, StratosTypes.OR, StratosTypes.XOR,
            // '<<' '>>'
            StratosTypes.LT_LT, StratosTypes.GT_GT,
            // '+' '-'
            StratosTypes.PLUS, StratosTypes.MINUS,
            // '*' '/' '%' '~/'
            StratosTypes.MUL, StratosTypes.DIV, StratosTypes.REM, StratosTypes.INT_DIV
    );

    TokenSet LOGIC_OPERATORS = TokenSet.create(
            StratosTypes.OR_OR, StratosTypes.AND_AND,
            // Strictly speaking, this isn't a logical operator, but should be formatted the same.
            StratosTypes.QUEST_QUEST
    );

    TokenSet UNARY_OPERATORS = TokenSet.create(
            // '-' '!' '~' '++' '--'
            StratosTypes.MINUS, StratosTypes.NOT, StratosTypes.BIN_NOT, StratosTypes.PLUS_PLUS, StratosTypes.MINUS_MINUS
    );

    TokenSet BITWISE_OPERATORS = TokenSet.create(StratosTypes.BITWISE_OPERATOR);
    TokenSet FUNCTION_DEFINITION = TokenSet.create(
            StratosTypes.FUNCTION_FORMAL_PARAMETER,
            StratosTypes.FUNCTION_DECLARATION_WITH_BODY,
            StratosTypes.FUNCTION_DECLARATION_WITH_BODY_OR_NATIVE,
            StratosTypes.METHOD_DECLARATION,
            StratosTypes.GETTER_DECLARATION,
            StratosTypes.SETTER_DECLARATION
    );

    TokenSet COMMENTS = TokenSet.create(StratosTypes.SINGLE_LINE_COMMENT, StratosTypes.SINGLE_LINE_DOC_COMMENT, StratosTypes.MULTI_LINE_COMMENT, MULTI_LINE_DOC_COMMENT);
    TokenSet DOC_COMMENT_CONTENTS =
            TokenSet.create(StratosTypes.MULTI_LINE_DOC_COMMENT_START, StratosTypes.MULTI_LINE_COMMENT_BODY, StratosTypes.DOC_COMMENT_LEADING_ASTERISK, StratosTypes.MULTI_LINE_COMMENT_END);

    IElementType EMBEDDED_CONTENT = new StratosEmbeddedContentElementType();

    TokenSet BLOCKS = TokenSet.create(
            StratosTypes.BLOCK,
            LAZY_PARSEABLE_BLOCK
    );

    TokenSet BLOCKS_EXT = TokenSet.create(
            StratosTypes.BLOCK,
            LAZY_PARSEABLE_BLOCK,
            StratosTypes.CLASS_MEMBERS,
            DART_FILE ,
            EMBEDDED_CONTENT
    );

    TokenSet DECLARATIONS = TokenSet.create(
            StratosTypes.CLASS_DEFINITION,
            StratosTypes.FUNCTION_DECLARATION_WITH_BODY,
            StratosTypes.FUNCTION_DECLARATION_WITH_BODY_OR_NATIVE,
            StratosTypes.GETTER_DECLARATION,
            StratosTypes.SETTER_DECLARATION,
            StratosTypes.VAR_DECLARATION_LIST,
            StratosTypes.FUNCTION_TYPE_ALIAS
    );

    class StratosDocCommentElementType extends ILazyParseableElementType {
        public StratosDocCommentElementType() {
            super("MULTI_LINE_DOC_COMMENT", StratosLanguage.INSTANCE);
        }

        @Override
        public ASTNode parseContents(@NotNull final ASTNode chameleon) {
            final PsiBuilder builder = PsiBuilderFactory.getInstance().createBuilder(chameleon.getTreeParent().getPsi().getProject(),
                    chameleon,
                    new StratosDocLexer(),
                    getLanguage(),
                    chameleon.getChars());
            doParse(builder);
            return builder.getTreeBuilt().getFirstChildNode();
        }

        private void doParse(final PsiBuilder builder) {
            final PsiBuilder.Marker root = builder.mark();

            while (!builder.eof()) {
                builder.advanceLexer();
            }

            root.done(this);
        }
    }

    class StratosLazyParseableBlockElementType extends IReparseableElementType {
        public StratosLazyParseableBlockElementType() {
            super("LAZY_PARSEABLE_BLOCK", StratosLanguage.INSTANCE);
        }

        @Override
        public boolean isParsable(@NotNull final CharSequence buffer, @NotNull final Language fileLanguage, @NotNull final Project project) {
            return PsiBuilderUtil.hasProperBraceBalance(buffer, new StratosLexerAdapter(), StratosTypes.LBRACE, StratosTypes.RBRACE);
        }

        @Nullable
        @Override
        public ASTNode createNode(@NotNull final CharSequence text) {
            return new StratosLazyParseableBlockImpl(this, text);
        }

        @Override
        protected ASTNode doParseContents(@NotNull ASTNode lazyParseableBlock, @NotNull PsiElement psi) {
            PsiBuilder builder = PsiBuilderFactory.getInstance().createBuilder(psi.getProject(), lazyParseableBlock);

//            if (isSyncOrAsync(lazyParseableBlock)) {
//                builder.putUserData(DartGeneratedParserUtilBase.INSIDE_SYNC_OR_ASYNC_FUNCTION, true);
//            }
            new StratosParser().parseLight(StratosTypes.BLOCK, builder);
            return builder.getTreeBuilt().getFirstChildNode();
        }

        private  boolean isSyncOrAsync(@NotNull final ASTNode newBlock) {
            final ASTNode oldBlock = Pair.getFirst(newBlock.getUserData(BlockSupport.TREE_TO_BE_REPARSED));
            final IElementType type = (oldBlock != null ? oldBlock : newBlock).getTreeParent().getFirstChildNode().getElementType();
            return type == StratosTypes.SYNC || type == StratosTypes.ASYNC;
        }
    }

    class StratosEmbeddedContentElementType extends ILazyParseableElementType implements ILightLazyParseableElementType {
        public StratosEmbeddedContentElementType() {
            super("STRATOS_EMBEDDED_CONTENT", StratosInHtmlLanguage.INSTANCE);
        }

        @Override
        protected ASTNode doParseContents(@NotNull ASTNode chameleon, @NotNull PsiElement psi) {
            PsiBuilder builder = PsiBuilderFactory.getInstance().createBuilder(psi.getProject(), chameleon);
            new StratosParser().parseLight(DART_FILE, builder);
            return builder.getTreeBuilt().getFirstChildNode();
        }

        @Override
        public FlyweightCapableTreeStructure<LighterASTNode> parseContents(LighterLazyParseableNode chameleon) {
//            PsiFile file = chameleon.getContainingFile();
//            assert file != null : chameleon;
//
//            final PsiBuilder psiBuilder = PsiBuilderFactory.getInstance().createBuilder(file.getProject(), chameleon);
//
//            final PsiBuilder builder = adapt_builder_(EMBEDDED_CONTENT, psiBuilder, new StratosParser(), StratosParser.EXTENDS_SETS_);
//
//            PsiBuilder.Marker marker = enter_section_(builder, 0, _COLLAPSE_, "<dart embedded content>");
//            boolean result = StratosParser.StratosFile(builder, 0);
//            exit_section_(builder, 0, marker, EMBEDDED_CONTENT, result, true, TRUE_CONDITION);
//            return builder.getLightTree();
        return null;
        }
    }
}
