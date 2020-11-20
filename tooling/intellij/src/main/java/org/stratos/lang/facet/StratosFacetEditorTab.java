package org.stratos.lang.facet;

import com.intellij.facet.ui.FacetEditorTab;
import com.intellij.openapi.options.ConfigurationException;
import com.intellij.openapi.projectRoots.ProjectJdkTable;
import com.intellij.openapi.projectRoots.Sdk;
import com.intellij.openapi.util.text.StringUtil;
import org.jetbrains.annotations.Nls;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;


public class StratosFacetEditorTab extends FacetEditorTab {

//  private final StratosSdkPanel mySdkPanel;
  private final StratosModuleSettings mySettings;

  public StratosFacetEditorTab(StratosModuleSettings settings) {
    mySettings = settings;
//    mySdkPanel = new StratosSdkPanel();
  }

  @Nls
  @Override
  public String getDisplayName() {
    return "Stratos Project";
  }

  @Nullable
  @Override
  public JComponent createComponent() {
    return new  JLabel("fuck off");//mySdkPanel;
  }

  @Override
  public boolean isModified() {
    return !StringUtil.equals(mySettings.redlineSmalltalkSdkName, "yooo");//mySdkPanel.getSdkName());
  }

  @Override
  public void apply() throws ConfigurationException {
    mySettings.redlineSmalltalkSdkName = "kooo"; //mySdkPanel.getSdkName();
  }

  @Override
  public void reset() {
    final Sdk sdk = ProjectJdkTable.getInstance().findJdk(mySettings.redlineSmalltalkSdkName);
    if (sdk != null) {
      System.out.println(".........look");
      //mySdkPanel.setSdk(sdk);
    }
  }

  @Override
  public void disposeUIResources() {
  }
}