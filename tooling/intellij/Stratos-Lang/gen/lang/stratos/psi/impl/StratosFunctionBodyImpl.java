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

public class StratosFunctionBodyImpl extends ASTWrapperPsiElement implements StratosFunctionBody {

  public StratosFunctionBodyImpl(@NotNull ASTNode node) {
    super(node);
  }

  public void accept(@NotNull StratosVisitor visitor) {
    visitor.visitFunctionBody(this);
  }

  public void accept(@NotNull PsiElementVisitor visitor) {
    if (visitor instanceof StratosVisitor) accept((StratosVisitor)visitor);
    else super.accept(visitor);
  }

  @Override
  @NotNull
  public List<StratosStatement> getStatementList() {
    return PsiTreeUtil.getChildrenOfTypeAsList(this, StratosStatement.class);
  }

}
