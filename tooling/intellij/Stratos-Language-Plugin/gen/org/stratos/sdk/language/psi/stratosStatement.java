// This is a generated file. Not intended for manual editing.
package org.stratos.sdk.language.psi;

import java.util.List;
import org.jetbrains.annotations.*;
import com.intellij.psi.PsiElement;

public interface stratosStatement extends PsiElement {

  @Nullable
  stratosAssignmentStatement getAssignmentStatement();

  @Nullable
  stratosFunctionInvocation getFunctionInvocation();

  @Nullable
  stratosVariableDefinition getVariableDefinition();

}