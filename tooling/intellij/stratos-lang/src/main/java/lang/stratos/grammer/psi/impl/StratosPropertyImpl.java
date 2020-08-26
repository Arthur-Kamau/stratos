// This is a generated file. Not intended for manual editing.
package lang.stratos.grammer.psi.impl;

import org.jetbrains.annotations.*;
import com.intellij.lang.ASTNode;
import com.intellij.psi.PsiElementVisitor;
import com.intellij.extapi.psi.ASTWrapperPsiElement;
import lang.stratos.grammer.psi.*;
//import lang.stratos.grammer.StratosPsiImplUtil;

public class StratosPropertyImpl extends ASTWrapperPsiElement implements StratosProperty {

  public StratosPropertyImpl(@NotNull ASTNode node) {
    super(node);
  }

  public void accept(@NotNull StratosVisitor visitor) {
    visitor.visitProperty(this);
  }

  public void accept(@NotNull PsiElementVisitor visitor) {
    if (visitor instanceof StratosVisitor) accept((StratosVisitor)visitor);
    else super.accept(visitor);
  }

}
