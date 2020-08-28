package lang.stratos.configuration;

import com.intellij.execution.ExecutionException;
import com.intellij.execution.Executor;
import com.intellij.execution.configurations.*;
//import com.intellij.execution.configurations.ConfigurationFactory;
import com.intellij.execution.configurations.RunConfigurationBase;
import com.intellij.execution.runners.ExecutionEnvironment;
import com.intellij.openapi.options.SettingsEditor;
import com.intellij.openapi.project.Project;
import org.jetbrains.annotations.NotNull;

import javax.annotation.Nullable;

public class StratosRunConfiguration extends RunConfigurationBase {
    protected StratosRunConfiguration(@NotNull Project project, @org.jetbrains.annotations.Nullable ConfigurationFactory factory, @org.jetbrains.annotations.Nullable String name) {
        super(project, factory, name);
    }
//     protected StratosRunConfiguration(Project project, ConfigurationFactory factory, String name) {
//        super(project, factory, name);
//    }

    @NotNull
    @Override
    public SettingsEditor<? extends RunConfiguration> getConfigurationEditor() {
        return new StratosSettingsEditor();
    }

    @Override
    public void checkConfiguration() throws RuntimeConfigurationException {

    }

    @Nullable
    @Override
    public RunProfileState getState(@NotNull Executor executor, @NotNull ExecutionEnvironment executionEnvironment) throws ExecutionException {
        return null;
    }
}