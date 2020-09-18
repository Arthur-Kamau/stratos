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

public class StratosStatementImpl extends ASTWrapperPsiElement implements StratosStatement {

  public StratosStatementImpl(@NotNull ASTNode node) {
    super(node);
  }

  public void accept(@NotNull StratosVisitor visitor) {
    visitor.visitStatement(this);
  }

  public void accept(@NotNull PsiElementVisitor visitor) {
    if (visitor instanceof StratosVisitor) accept((StratosVisitor)visitor);
    else super.accept(visitor);
  }

  @Override
  @Nullable
  public StratosAssignmentStatement getAssignmentStatement() {
    return findChildByClass(StratosAssignmentStatement.class);
  }

  @Override
  @Nullable
  public StratosEnumType getEnumType() {
    return findChildByClass(StratosEnumType.class);
  }

  @Override
  @Nullable
  public StratosFunctionInvocation getFunctionInvocation() {
    return findChildByClass(StratosFunctionInvocation.class);
  }

  @Override
  @Nullable
  public StratosStructType getStructType() {
    return findChildByClass(StratosStructType.class);
  }

  @Override
  @Nullable
  public StratosVariableDefinition getVariableDefinition() {
    return findChildByClass(StratosVariableDefinition.class);
  }

}
