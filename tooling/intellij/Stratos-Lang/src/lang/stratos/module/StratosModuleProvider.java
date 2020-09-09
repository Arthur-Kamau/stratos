package lang.stratos.module;

import com.intellij.framework.FrameworkTypeEx;
import com.intellij.framework.addSupport.FrameworkSupportInModuleConfigurable;
import com.intellij.framework.addSupport.FrameworkSupportInModuleProvider;
import com.intellij.ide.util.frameworkSupport.FrameworkSupportModel;
import com.intellij.openapi.module.JavaModuleType;
import com.intellij.openapi.module.ModuleType;
import org.jetbrains.annotations.NotNull;

/**
 * @author Konstantin Bulenkov
 */
public class StratosModuleProvider extends FrameworkSupportInModuleProvider {
  public StratosModuleProvider() {

  }

  @NotNull
  @Override
  public FrameworkTypeEx getFrameworkType() {
    return FrameworkTypeEx.EP_NAME.findExtension(StratosFrameworkType.class);
  }

  @NotNull
  @Override
  public FrameworkSupportInModuleConfigurable createConfigurable(@NotNull FrameworkSupportModel model) {
    return new StratosModuleConfigurable();
  }

  @Override
  public boolean isEnabledForModuleType(@NotNull ModuleType moduleType) {
    return moduleType instanceof JavaModuleType;
  }
}
