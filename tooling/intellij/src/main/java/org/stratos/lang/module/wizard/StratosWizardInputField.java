package org.stratos.lang.module.wizard;

import com.intellij.ide.util.projectWizard.WizardInputField;
import com.intellij.openapi.projectRoots.ProjectJdkTable;
import com.intellij.openapi.projectRoots.Sdk;
import com.intellij.openapi.util.Condition;
import org.stratos.lang.module.sdk.StratosSdkComboBox;

import javax.swing.*;

/**
 * @author Arthur Kamau
 */
public class StratosWizardInputField {
//        extends WizardInputField {
//  private final StratosSdkComboBox myCombo;
//
//  protected StratosWizardInputField() {
//    super("STRATOS_SDK", findMostRecentSdkPath());
//    myCombo = new StratosSdkComboBox();
//  }
//
//  private static String findMostRecentSdkPath() {
//    Sdk sdk = ProjectJdkTable.getInstance().findMostRecentSdk(new Condition<Sdk>() {
//      @Override
//      public boolean value(Sdk sdk) {
//        return sdk.getSdkType() instanceof StratosSdkType;
//      }
//    });
//    return sdk != null ? sdk.getName() : null;
//  }
//
//  @Override
//  public String getLabel() {
//    return "Stratos SDK";
//  }
//
//  @Override
//  public JComponent getComponent() {
//    return myCombo;
//  }
//
//  @Override
//  public String getValue() {
//    Sdk sdk = myCombo.getSelectedSdk();
//    return sdk == null ? "" : sdk.getHomePath();
//  }
}
