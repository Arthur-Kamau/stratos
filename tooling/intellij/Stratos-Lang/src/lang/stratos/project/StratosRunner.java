package lang.stratos.project;


import com.intellij.execution.ExecutionException;
        import com.intellij.execution.ExecutionResult;
        import com.intellij.execution.Executor;
        import com.intellij.execution.configurations.RunProfile;
        import com.intellij.execution.configurations.RunProfileState;
        import com.intellij.execution.executors.DefaultRunExecutor;
        import com.intellij.execution.runners.DefaultProgramRunner;
        import com.intellij.execution.runners.ExecutionEnvironment;
        import com.intellij.execution.ui.RunContentDescriptor;
        import com.intellij.openapi.actionSystem.AnAction;
        import com.intellij.openapi.project.Project;
        import org.jetbrains.annotations.NotNull;

public class StratosRunner extends DefaultProgramRunner {
    public static final String ID = "StratosRunnerRunner";

    @Override
    public AnAction[] createActions(ExecutionResult executionResult) {
        return super.createActions(executionResult);
    }

    @Override
    @NotNull
    public String getRunnerId() {
        return ID;
    }

    @Override
    public boolean canRun(@NotNull String executorId, @NotNull RunProfile profile) {
        return DefaultRunExecutor.EXECUTOR_ID.equals(executorId) && profile instanceof StratosRunConfiguration;
    }


    @Override
    protected RunContentDescriptor doExecute(Project project,
                                             Executor executor,
                                             RunProfileState state,
                                             RunContentDescriptor contentToReuse,
                                             ExecutionEnvironment env) throws ExecutionException {

        RsRunConfiguration asd = RsRunConfiguration.newInstance("tmp", project);
        return super.doExecute(project, executor, asd.getState(executor, env), contentToReuse, env);
    }

}
