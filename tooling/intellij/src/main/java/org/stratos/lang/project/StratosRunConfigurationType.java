package org.stratos.lang.project;

import com.intellij.execution.configurations.ConfigurationFactory;
import com.intellij.execution.configurations.ConfigurationType;
import com.intellij.execution.configurations.ConfigurationTypeUtil;

import org.jetbrains.annotations.NotNull;
import org.stratos.lang.Icons;

import javax.swing.*;

public class StratosRunConfigurationType implements ConfigurationType {

        public static StratosRunConfigurationType getInstance() {
                return ConfigurationTypeUtil.findConfigurationType(StratosRunConfigurationType.class);
        }

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
                return Icons.SAMPLE_ICON;
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

