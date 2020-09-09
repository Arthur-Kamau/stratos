package lang.stratos.module;

import com.intellij.ide.util.projectWizard.importSources.DetectedSourceRoot;
import org.jetbrains.annotations.NotNull;

import java.io.File;

/**
* @author Konstantin Bulenkov
*/
public class StratosDetectedSourceRoot extends DetectedSourceRoot {
  public StratosDetectedSourceRoot(File root) {
    super(root, "");
  }

  @NotNull
  @Override
  public String getRootTypeName() {
    return "Stratos";
  }
}
