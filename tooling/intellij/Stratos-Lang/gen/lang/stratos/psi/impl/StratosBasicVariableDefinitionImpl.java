// This is a generated file. Not intended for manual editing.
package lang.stratos.psi.impl;

import java.util.List;
import org.jetbrains.annotations.*;
import com.intellij.lang.ASTNode;
import com.intellij.psi.PsiElement;
import com.intellij.psi.PsiElementVisitor;
import com.intellij.psi.util.PsiTreeUtil;
import static lang.stratos.psi.StratosTypes.*;
import com.intellij.extapi.psi.ASTWrapperPsiElement;
import lang.stratos.psi.*;

public class StratosBasicVariableDefinitionImpl extends ASTWrapperPsiElement implements StratosBasicVariableDefinition {

  public StratosBasicVariableDefinitionImpl(@NotNull ASTNode node) {
    super(node);
  }

  public void accept(@NotNull StratosVisitor visitor) {
    visitor.visitBasicVariableDefinition(this);
  }

  public void accept(@NotNull PsiElementVisitor visitor) {
    if (visitor instanceof StratosVisitor) accept((StratosVisitor)visitor);
    else super.accept(visitor);
  }

  @Override
  @NotNull
  public StratosType getType() {
    return findNotNullChildByClass(StratosType.class);
  }

}
