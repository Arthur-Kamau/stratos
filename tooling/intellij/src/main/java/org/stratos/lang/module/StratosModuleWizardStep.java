package org.stratos.lang.module;


import com.intellij.ide.util.projectWizard.ModuleWizardStep;

import javax.swing.*;

public class StratosModuleWizardStep extends ModuleWizardStep {

    private final StratosSdkPanel sdkPanel = new StratosSdkPanel();

    @Override
    public JComponent getComponent() {
        return sdkPanel;
//        return new JLabel("Provide some setting here");
    }

    @Override
    public void updateDataModel() {
        //todo update model according to UI
    }

}