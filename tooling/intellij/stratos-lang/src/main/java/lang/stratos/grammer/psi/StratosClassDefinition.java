// This is a generated file. Not intended for manual editing.
package lang.stratos.grammer.psi;

import java.util.List;
import org.jetbrains.annotations.*;
import com.intellij.psi.PsiElement;

public interface StratosClassDefinition extends PsiElement {

  @Nullable
  StratosFunctionBody getFunctionBody();

  @NotNull
  List<StratosStatement> getStatementList();

  @Nullable
  PsiElement getIdentifier();

}
