package org.stratos.lang.formatter;

import com.intellij.formatting.*;
import com.intellij.lang.ASTNode;
import com.intellij.openapi.util.TextRange;
import com.intellij.psi.PsiElement;
import com.intellij.psi.PsiFile;
import com.intellij.psi.codeStyle.CodeStyleSettings;
import com.intellij.psi.tree.IElementType;
import com.intellij.psi.tree.TokenSet;
import org.antlr.intellij.adaptor.lexer.TokenIElementType;
import org.antlr.jetbrains.sample.parser.StratosLanguageLexer;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.stratos.lang.StratosLanguage;

public class StratosFormattingModelBuilder implements FormattingModelBuilder {
//    IElementType tt = PsiElement

    private static SpacingBuilder createSpaceBuilder(CodeStyleSettings settings) {
        return new SpacingBuilder(settings, StratosLanguage.INSTANCE)
                .before(IElementType.find((short) StratosLanguageLexer.PACKAGE)).spaceIf(true)
                .after(IElementType.find((short) StratosLanguageLexer.PACKAGE)).spaceIf(true);

//                .betweenInside(IElementType.find((short) StratosLanguageLexer.PACKAGE)).spaces(1);
//                .around( TokenSet.ANY )
//                .spaceIf(settings.getCommonSettings(StratosLanguage.INSTANCE.getID()).SPACE_AROUND_ASSIGNMENT_OPERATORS)
////                .before(SimpleTypes.PROPERTY)
////                .before(TokenIElementType.)
//                .before(IElementType.find((short) StratosLanguageLexer.PACKAGE))
//
////                .before(TokenSet.WHITE_SPACE)
//                .none();
    }

    @NotNull
    @Override
    public FormattingModel createModel(PsiElement element, CodeStyleSettings settings) {
        return FormattingModelProvider
                .createFormattingModelForPsiFile(element.getContainingFile(),
                        new StratosBlock(element.getNode(),
                                Wrap.createWrap(WrapType.NORMAL, true),
                                Alignment.createAlignment(),
                                createSpaceBuilder(settings)),
                        settings);
    }

    @Nullable
    @Override
    public TextRange getRangeAffectingIndent(PsiFile file, int offset, ASTNode elementAtOffset) {
        return null;
    }

}