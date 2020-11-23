package org.stratos.lang.project.runner;

import com.intellij.openapi.fileChooser.FileChooserDescriptorFactory;
import com.intellij.openapi.options.ConfigurationException;
import com.intellij.openapi.options.SettingsEditor;
import com.intellij.openapi.projectRoots.Sdk;
import com.intellij.openapi.ui.LabeledComponent;
import com.intellij.openapi.ui.TextComponentAccessor;
import com.intellij.openapi.ui.TextFieldWithBrowseButton;
import com.intellij.openapi.util.io.FileUtil;
import com.intellij.openapi.util.text.StringUtil;
import org.jetbrains.annotations.NotNull;
import org.stratos.lang.module.StratosSdkComboBox;
import org.stratos.lang.utils.StratosSdk;

import javax.swing.*;

public class StratosSettingsEditor extends SettingsEditor<StratosRunConfiguration> {

    private JPanel myMainPanel;
    private TextFieldWithBrowseButton sdkArgs;

    private StratosSdkComboBox mySdkComboBox;
    private StratosSdkDumpComboBox mySdkDumpComboBox;

    @Override
    protected void resetEditorFrom(@NotNull StratosRunConfiguration s) {
         StratosSdk sdkPath = StratosSdk.getStratosSdk(s.getProject());
        sdkArgs.setText("");
//        sdkArgs.addBrowseFolderListener();
        sdkArgs.addBrowseFolderListener("App Config for Sdk", "app.config file with SDk args", s.getProject(),
                FileChooserDescriptorFactory.createSingleFolderDescriptor(),
                TextComponentAccessor.TEXT_FIELD_SELECTED_TEXT);
//        sdkArgs.setText(FileUtil.toSystemDependentName(StringUtil.notNullize(sdkPath.getDartSdkPath())));

    }

    @Override
    protected void applyEditorTo(@NotNull StratosRunConfiguration s) throws ConfigurationException {

    }

    @NotNull
    @Override
    protected JComponent createEditor() {
        return myMainPanel;
    }

    public String getSdkName() {
        final Sdk selectedSdk = mySdkComboBox.getSelectedSdk();
        return selectedSdk == null ? null : selectedSdk.getName();
      }
    
      public Sdk getSdk() {
        return mySdkComboBox.getSelectedSdk();
      }
    
      public void setSdk(Sdk sdk) {
        mySdkComboBox.getComboBox().setSelectedItem(sdk);
      }
}
