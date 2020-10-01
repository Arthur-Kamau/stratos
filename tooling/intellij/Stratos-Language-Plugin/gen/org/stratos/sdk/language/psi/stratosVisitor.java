// This is a generated file. Not intended for manual editing.
package org.stratos.sdk.language.psi;

import org.jetbrains.annotations.*;
import com.intellij.psi.PsiElementVisitor;
import com.intellij.psi.PsiElement;

public class stratosVisitor extends PsiElementVisitor {

  public void visitAssignmentStatement(@NotNull stratosAssignmentStatement o) {
    visitPsiElement(o);
  }

  public void visitDefinition(@NotNull stratosDefinition o) {
    visitPsiElement(o);
  }

  public void visitFunctionBody(@NotNull stratosFunctionBody o) {
    visitPsiElement(o);
  }

  public void visitFunctionDefinition(@NotNull stratosFunctionDefinition o) {
    visitPsiElement(o);
  }

  public void visitFunctionInvocation(@NotNull stratosFunctionInvocation o) {
    visitPsiElement(o);
  }

  public void visitStatement(@NotNull stratosStatement o) {
    visitPsiElement(o);
  }

  public void visitType(@NotNull stratosType o) {
    visitPsiElement(o);
  }

  public void visitVariableDefinition(@NotNull stratosVariableDefinition o) {
    visitPsiElement(o);
  }

  public void visitPsiElement(@NotNull PsiElement o) {
    visitElement(o);
  }

}
