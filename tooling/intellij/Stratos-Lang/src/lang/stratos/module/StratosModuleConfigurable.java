package lang.stratos.module;

import com.intellij.framework.addSupport.FrameworkSupportInModuleConfigurable;
import com.intellij.openapi.module.Module;
import com.intellij.openapi.roots.ModifiableModelsProvider;
import com.intellij.openapi.roots.ModifiableRootModel;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;

/**
 * @author Konstantin Bulenkov
 */
public class StratosModuleConfigurable extends FrameworkSupportInModuleConfigurable {
  public static final String REDLINE_SMALLTALK_SDK_KEY = "REDLINE_SMALLTALK_SDK_NAME";
  private StratosSdkPanel myRsSdkPanel = new StratosSdkPanel();

  @Nullable
  @Override
  public JComponent createComponent() {
    return myRsSdkPanel;
  }

  @Override
  public void addSupport(@NotNull Module module, @NotNull ModifiableRootModel model, @NotNull ModifiableModelsProvider provider) {
    module.setOption(REDLINE_SMALLTALK_SDK_KEY, myRsSdkPanel.getSdkName());
  }
}
