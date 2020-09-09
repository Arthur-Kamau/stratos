package lang.stratos.module;

import com.intellij.ide.util.projectWizard.ModuleWizardStep;
import com.intellij.openapi.options.ConfigurationException;
import com.intellij.openapi.util.text.StringUtil;

import javax.swing.*;

/**
 * @author Konstantin Bulenkov
 */
public class StratosModuleWizardStep extends ModuleWizardStep {
  private final StratosSdkPanel sdkPanel = new StratosSdkPanel();
  private StratosModuleBuilder myBuilder;

  public StratosModuleWizardStep(StratosModuleBuilder builder) {
    myBuilder = builder;
  }

  @Override
  public JComponent getComponent() {
    return sdkPanel;
  }

  @Override
  public void updateDataModel() {
    myBuilder.setSdk(sdkPanel.getSdk());
  }


  @Override
  public boolean validate() throws ConfigurationException {
    if (StringUtil.isEmpty(sdkPanel.getSdkName())) {
      throw new ConfigurationException("Specify Stratos SDK");
    }
    return super.validate();
  }
}
