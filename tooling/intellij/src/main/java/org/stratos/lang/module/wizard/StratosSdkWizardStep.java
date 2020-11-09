package org.stratos.lang.module.wizard;

import com.intellij.ide.util.projectWizard.ModuleWizardStep;
import com.intellij.openapi.options.ConfigurationException;
import com.intellij.openapi.util.text.StringUtil;

import javax.swing.*;

/**
 * @author Arthur Kamau
 */

import com.intellij.ide.util.projectWizard.ModuleWizardStep;
import org.stratos.lang.module.sdk.StratosSdkPanel;

import javax.swing.*;

public class StratosSdkWizardStep extends ModuleWizardStep {

  private final StratosSdkPanel sdkPanel = new StratosSdkPanel();
  @Override
  public JComponent getComponent() {

    return sdkPanel;
//    return new JLabel("Stratos Project Types ( project / script / library) ");
  }

  @Override
  public void updateDataModel() {
    //todo update model according to UI
  }

}

//public class StratosModuleWizardStep extends ModuleWizardStep {
//  private final StratosSdkPanel sdkPanel = new StratosSdkPanel();
//  private StratosModuleBuilder myBuilder;
//
//  public StratosModuleWizardStep(StratosModuleBuilder builder) {
//    myBuilder = builder;
//  }
//
//  @Override
//  public JComponent getComponent() {
//    return sdkPanel;
//  }
//
//  @Override
//  public void updateDataModel() {
//    myBuilder.setSdk(sdkPanel.getSdk());
//  }
//
//
//  @Override
//  public boolean validate() throws ConfigurationException {
//    if (StringUtil.isEmpty(sdkPanel.getSdkName())) {
//      throw new ConfigurationException("Specify Stratos SDK");
//    }
//    return super.validate();
//  }
//}
