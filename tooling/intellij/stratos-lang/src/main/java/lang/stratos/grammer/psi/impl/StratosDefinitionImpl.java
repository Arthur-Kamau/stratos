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
  @Nullable
  public StratosClassDefinition getClassDefinition() {
    return findChildByClass(StratosClassDefinition.class);
  }

  @Override
  @Nullable
  public StratosFunctionDefinition getFunctionDefinition() {
    return findChildByClass(StratosFunctionDefinition.class);
  }

  @Override
  @Nullable
  public StratosNameSpaceDefinition getNameSpaceDefinition() {
    return findChildByClass(StratosNameSpaceDefinition.class);
  }

}
