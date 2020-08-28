package lang.stratos.configuration;


import com.intellij.execution.configurations.ConfigurationFactory;
import com.intellij.execution.configurations.ConfigurationType;
import lang.stratos.icons.StratosIcons;
import org.jetbrains.annotations.NotNull;

import javax.swing.*;

public class StratosRunConfigurationType implements ConfigurationType {
    @Override
    public String getDisplayName() {
        return "Stratos";
    }

    @Override
    public String getConfigurationTypeDescription() {
        return "Stratos Run Configuration Type";
    }

    @Override
    public Icon getIcon() {
        return StratosIcons.StratosInformation;  //AllIcons.General.Information;
    }

    @NotNull
    @Override
    public String getId() {
        return "STRATOS_RUN_CONFIGURATION";
    }

    @Override
    public ConfigurationFactory[] getConfigurationFactories() {
        return new ConfigurationFactory[]{new StratosConfigurationFactory(this)};
    }
}