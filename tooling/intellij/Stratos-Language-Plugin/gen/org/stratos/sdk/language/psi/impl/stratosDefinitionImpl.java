// This is a generated file. Not intended for manual editing.
package org.stratos.sdk.language.psi.impl;

import java.util.List;
import org.jetbrains.annotations.*;
import com.intellij.lang.ASTNode;
import com.intellij.psi.PsiElement;
import com.intellij.psi.PsiElementVisitor;
import com.intellij.psi.util.PsiTreeUtil;
import static org.stratos.sdk.language.psi.StratosTypes.*;
import com.intellij.extapi.psi.ASTWrapperPsiElement;
import org.stratos.sdk.language.psi.*;

public class stratosDefinitionImpl extends ASTWrapperPsiElement implements stratosDefinition {

  public stratosDefinitionImpl(@NotNull ASTNode node) {
    super(node);
  }

  public void accept(@NotNull stratosVisitor visitor) {
    visitor.visitDefinition(this);
  }

  public void accept(@NotNull PsiElementVisitor visitor) {
    if (visitor instanceof stratosVisitor) accept((stratosVisitor)visitor);
    else super.accept(visitor);
  }

  @Override
  @NotNull
  public stratosFunctionDefinition getFunctionDefinition() {
    return findNotNullChildByClass(stratosFunctionDefinition.class);
  }

}