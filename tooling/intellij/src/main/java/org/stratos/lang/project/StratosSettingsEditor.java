package org.stratos.lang.project;

import com.intellij.openapi.options.ConfigurationException;
import com.intellij.openapi.options.SettingsEditor;
import com.intellij.openapi.ui.ComponentWithBrowseButton;
import com.intellij.openapi.ui.LabeledComponent;
import com.intellij.openapi.ui.TextFieldWithBrowseButton;
import org.jetbrains.annotations.NotNull;

import javax.swing.*;

public class StratosSettingsEditor extends SettingsEditor<StratosRunConfiguration> {
    private JPanel myPanel;
    private LabeledComponent<ComponentWithBrowseButton> myMainClass;

    @Override
    protected void resetEditorFrom(StratosRunConfiguration demoRunConfiguration) {

    }

    @Override
    protected void applyEditorTo(StratosRunConfiguration demoRunConfiguration) throws ConfigurationException {

    }

    @NotNull
    @Override
    protected JComponent createEditor() {
        return myPanel;
    }

    private void createUIComponents() {
        myMainClass = new LabeledComponent<ComponentWithBrowseButton>();
        myMainClass.setComponent(new TextFieldWithBrowseButton());
    }
}