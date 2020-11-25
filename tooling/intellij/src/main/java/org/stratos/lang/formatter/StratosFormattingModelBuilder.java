package org.stratos.lang.formatter;

import com.intellij.formatting.*;
import com.intellij.lang.ASTNode;
import com.intellij.openapi.util.TextRange;
import com.intellij.psi.PsiElement;
import com.intellij.psi.PsiFile;
import com.intellij.psi.codeStyle.CodeStyleSettings;
//import org.intellij.sdk.language.psi.SimpleTypes;
import com.intellij.psi.tree.TokenSet;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.stratos.lang.StratosLanguage;

public class StratosFormattingModelBuilder implements FormattingModelBuilder {

    private static SpacingBuilder createSpaceBuilder(CodeStyleSettings settings) {
        return new SpacingBuilder(settings, StratosLanguage.INSTANCE)
                .around( TokenSet.ANY )
                .spaceIf(settings.getCommonSettings(StratosLanguage.INSTANCE.getID()).SPACE_AROUND_ASSIGNMENT_OPERATORS)
//                .before(SimpleTypes.PROPERTY)
                .before(TokenSet.WHITE_SPACE)
                .none();
    }

    @NotNull
    @Override
    public FormattingModel createModel(PsiElement element, CodeStyleSettings settings) {
        return FormattingModelProvider
                .createFormattingModelForPsiFile(element.getContainingFile(),
                        new StratosBlock(element.getNode(),
                                Wrap.createWrap(WrapType.NONE, false),
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