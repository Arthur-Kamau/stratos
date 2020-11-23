package org.stratos.lang.project.runner;


import com.intellij.execution.ExecutionException;
import com.intellij.execution.Executor;
import com.intellij.execution.configurations.*;
import com.intellij.execution.executors.DefaultRunExecutor;
import com.intellij.execution.runners.ExecutionEnvironment;
import com.intellij.execution.runners.ProgramRunner;
import com.intellij.openapi.options.SettingsEditor;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class StratosRunner implements ProgramRunner {
    public static final String ID = "StratosRunnerRunner";


    @Override
    @NotNull
    public String getRunnerId() {
        return ID;
    }

    @Override
    public boolean canRun(@NotNull String executorId, @NotNull RunProfile profile) {
        return DefaultRunExecutor.EXECUTOR_ID.equals(executorId) && profile instanceof StratosRunConfiguration;
    }

    @Nullable
    @Override
    public RunnerSettings createConfigurationData(@NotNull ConfigurationInfoProvider settingsProvider) {
        return null;
    }

    @Override
    public void checkConfiguration(RunnerSettings settings, @Nullable ConfigurationPerRunnerSettings configurationPerRunnerSettings) throws RuntimeConfigurationException {

    }

    @Nullable
    @Override
    public SettingsEditor getSettingsEditor(Executor executor, RunConfiguration configuration) {
        return null;
    }

    @Override
    public void execute(@NotNull ExecutionEnvironment executionEnvironment) throws ExecutionException {
        System.out.println("===========================================");
        System.out.println("===========================================");
        System.out.println("  -------------------- Run Organisation   -----------------");
        System.out.println("===========================================");
        System.out.println("===========================================");
    }


//    @Override
//    protected RunContentDescriptor doExecute(Project project,
//                                             Executor executor,
//                                             RunProfileState state,
//                                             RunContentDescriptor contentToReuse,
//                                             ExecutionEnvironment env) throws ExecutionException {
//
//        StratosRunConfiguration asd = StratosRunConfiguration.newInstance("tmp", project);
//        return super.doExecute(project, executor, asd.getState(executor, env), contentToReuse, env);
//    }

}
