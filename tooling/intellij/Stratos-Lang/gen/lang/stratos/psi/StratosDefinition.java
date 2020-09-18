// This is a generated file. Not intended for manual editing.
package lang.stratos.psi;

import java.util.List;
import org.jetbrains.annotations.*;
import com.intellij.psi.PsiElement;

public interface StratosDefinition extends PsiElement {

  @NotNull
  List<StratosEnumType> getEnumTypeList();

  @NotNull
  List<StratosFunctionDefinition> getFunctionDefinitionList();

  @Nullable
  StratosPackageDefinition getPackageDefinition();

  @NotNull
  List<StratosStructType> getStructTypeList();

}
