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

public class StratosDefinitionImpl extends ASTWrapperPsiElement implements StratosDefinition {

  public StratosDefinitionImpl(@NotNull ASTNode node) {
    super(node);
  }

  public void accept(@NotNull StratosVisitor visitor) {
    visitor.visitDefinition(this);
  }

  public void accept(@NotNull PsiElementVisitor visitor) {
    if (visitor instanceof StratosVisitor) accept((StratosVisitor)visitor);
    else super.accept(visitor);
  }

  @Override
  @NotNull
  public List<StratosEnumType> getEnumTypeList() {
    return PsiTreeUtil.getChildrenOfTypeAsList(this, StratosEnumType.class);
  }

  @Override
  @NotNull
  public List<StratosFunctionDefinition> getFunctionDefinitionList() {
    return PsiTreeUtil.getChildrenOfTypeAsList(this, StratosFunctionDefinition.class);
  }

  @Override
  @Nullable
  public StratosPackageDefinition getPackageDefinition() {
    return findChildByClass(StratosPackageDefinition.class);
  }

  @Override
  @NotNull
  public List<StratosStructType> getStructTypeList() {
    return PsiTreeUtil.getChildrenOfTypeAsList(this, StratosStructType.class);
  }

}
