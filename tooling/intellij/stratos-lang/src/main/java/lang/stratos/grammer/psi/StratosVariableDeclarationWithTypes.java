// This is a generated file. Not intended for manual editing.
package lang.stratos.grammer.psi;

import java.util.List;
import org.jetbrains.annotations.*;
import com.intellij.psi.PsiElement;

public interface StratosVariableDeclarationWithTypes extends PsiElement {

  @Nullable
  StratosFunctionInvocation getFunctionInvocation();

  @NotNull
  StratosType getType();

  @NotNull
  StratosVariableDeclaration getVariableDeclaration();

  @NotNull
  PsiElement getIdentifier();

}
