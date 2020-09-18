// This is a generated file. Not intended for manual editing.
package lang.stratos.psi;

import java.util.List;
import org.jetbrains.annotations.*;
import com.intellij.psi.PsiElement;

public interface StratosStatement extends PsiElement {

  @Nullable
  StratosAssignmentStatement getAssignmentStatement();

  @Nullable
  StratosEnumType getEnumType();

  @Nullable
  StratosFunctionInvocation getFunctionInvocation();

  @Nullable
  StratosStructType getStructType();

  @Nullable
  StratosVariableDefinition getVariableDefinition();

}
