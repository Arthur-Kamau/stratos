package org.stratos.lang.module;

import com.intellij.ide.util.projectWizard.ModuleWizardStep;
import com.intellij.ide.util.projectWizard.WizardContext;
import com.intellij.openapi.module.ModuleType;
import com.intellij.openapi.module.ModuleTypeManager;
import com.intellij.openapi.roots.ui.configuration.ModulesProvider;
import org.jetbrains.annotations.NotNull;
import org.stratos.lang.StratosIcons;

import javax.swing.*;

public class StratosModuleType extends ModuleType<StratosModuleBuilder> {

    private static final StratosModuleType INSTANCE = new StratosModuleType();

    private static final String ID = "DEMO_MODULE_TYPE";

    public StratosModuleType() {
        super(ID);
    }

    public static StratosModuleType getInstance() {
        return INSTANCE;
    }

    @NotNull
    @Override
    public StratosModuleBuilder createModuleBuilder() {
        return new StratosModuleBuilder();
    }

    @NotNull
    @Override
    public String getName() {
        return "SDK Module Type";
    }

    @NotNull
    @Override
    public String getDescription() {
        return "Stratos custom module type";
    }

    @NotNull
    @Override
    public Icon getNodeIcon(@Deprecated boolean b) {
        return StratosIcons.SAMPLE_ICON;
    }

    @NotNull
    @Override
    public ModuleWizardStep[] createWizardSteps(@NotNull WizardContext wizardContext,
                                                @NotNull StratosModuleBuilder moduleBuilder,
                                                @NotNull ModulesProvider modulesProvider) {
        return super.createWizardSteps(wizardContext, moduleBuilder, modulesProvider);
    }

}
