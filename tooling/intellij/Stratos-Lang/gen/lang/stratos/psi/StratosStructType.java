// This is a generated file. Not intended for manual editing.
package lang.stratos.psi;

import java.util.List;
import org.jetbrains.annotations.*;
import com.intellij.psi.PsiElement;

public interface StratosStructType extends PsiElement {

  @NotNull
  List<StratosBasicVariableDefinition> getBasicVariableDefinitionList();

  @NotNull
  StratosTypeWrapper getTypeWrapper();

  @NotNull
  PsiElement getIdentifier();

}
