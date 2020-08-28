// This is a generated file. Not intended for manual editing.
package lang.stratos.grammer.psi.impl;

import java.util.List;
import org.jetbrains.annotations.*;
import com.intellij.lang.ASTNode;
import com.intellij.psi.PsiElement;
import com.intellij.psi.PsiElementVisitor;
import com.intellij.psi.util.PsiTreeUtil;
import static lang.stratos.grammer.types.StratosTypes.*;
import com.intellij.extapi.psi.ASTWrapperPsiElement;
import lang.stratos.grammer.psi.*;
// import lang.stratos.grammer.StratosPsiImplUtil;

public class StratosNameSpaceDefinitionImpl extends ASTWrapperPsiElement implements StratosNameSpaceDefinition {

  public StratosNameSpaceDefinitionImpl(@NotNull ASTNode node) {
    super(node);
  }

  public void accept(@NotNull StratosVisitor visitor) {
    visitor.visitNameSpaceDefinition(this);
  }

  public void accept(@NotNull PsiElementVisitor visitor) {
    if (visitor instanceof StratosVisitor) accept((StratosVisitor)visitor);
    else super.accept(visitor);
  }

  @Override
  @Nullable
  public StratosFunctionBody getFunctionBody() {
    return findChildByClass(StratosFunctionBody.class);
  }

  @Override
  @NotNull
  public List<StratosStatement> getStatementList() {
    return PsiTreeUtil.getChildrenOfTypeAsList(this, StratosStatement.class);
  }

  @Override
  @Nullable
  public PsiElement getIdentifier() {
    return findChildByType(IDENTIFIER);
  }

}
