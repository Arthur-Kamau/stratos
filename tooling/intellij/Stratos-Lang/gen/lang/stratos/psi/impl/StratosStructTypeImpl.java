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

public class StratosStructTypeImpl extends ASTWrapperPsiElement implements StratosStructType {

  public StratosStructTypeImpl(@NotNull ASTNode node) {
    super(node);
  }

  public void accept(@NotNull StratosVisitor visitor) {
    visitor.visitStructType(this);
  }

  public void accept(@NotNull PsiElementVisitor visitor) {
    if (visitor instanceof StratosVisitor) accept((StratosVisitor)visitor);
    else super.accept(visitor);
  }

  @Override
  @NotNull
  public List<StratosBasicVariableDefinition> getBasicVariableDefinitionList() {
    return PsiTreeUtil.getChildrenOfTypeAsList(this, StratosBasicVariableDefinition.class);
  }

  @Override
  @NotNull
  public StratosTypeWrapper getTypeWrapper() {
    return findNotNullChildByClass(StratosTypeWrapper.class);
  }

  @Override
  @NotNull
  public PsiElement getIdentifier() {
    return findNotNullChildByType(IDENTIFIER);
  }

}
