// This is a generated file. Not intended for manual editing.
package lang.stratos.psi;

import java.util.List;
import org.jetbrains.annotations.*;
import com.intellij.psi.PsiElement;

public interface StratosFunctionDefinition extends PsiElement {

  @NotNull
  StratosFunctionBody getFunctionBody();

  @Nullable
  StratosType getType();

  @NotNull
  PsiElement getIdentifier();

}
