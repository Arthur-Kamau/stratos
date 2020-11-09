package org.stratos.lang.module.wizard;

import com.intellij.ide.util.projectWizard.ModuleWizardStep;
import org.stratos.lang.module.projectType.StratosProjectTypePanel;
import org.stratos.lang.module.sdk.StratosSdkPanel;

import javax.swing.*;

public class StratosProjectTypeWizardStep extends ModuleWizardStep {

    private final StratosProjectTypePanel sdkPanel = new StratosProjectTypePanel();
    @Override
    public JComponent getComponent() {

        return   sdkPanel;

    }

    @Override
    public void updateDataModel() {
        //todo update model according to UI
    }

}
