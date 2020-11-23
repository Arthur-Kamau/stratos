package org.stratos.lang.project.runner;
import com.intellij.execution.ExecutionException;
import com.intellij.execution.Executor;
import com.intellij.execution.configurations.*;
import com.intellij.execution.runners.ExecutionEnvironment;
import com.intellij.openapi.options.SettingsEditor;
import com.intellij.openapi.project.Project;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

public class StratosRunConfiguration extends RunConfigurationBase {
    public StratosRunConfiguration(Project project, ConfigurationFactory factory, String name) {
        super(project, factory, name);
    }

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
//
//
////import com.intellij.compiler.make.MakeUtil;
//import com.intellij.diagnostic.logging.LogConfigurationPanel;
//import com.intellij.execution.*;
//import com.intellij.execution.application.ApplicationConfiguration;
//import com.intellij.execution.configurations.*;
//import com.intellij.execution.filters.TextConsoleBuilderFactory;
//import com.intellij.execution.runners.ExecutionEnvironment;
//import com.intellij.execution.util.JavaParametersUtil;
//import com.intellij.execution.util.ProgramParametersUtil;
//import com.intellij.openapi.extensions.Extensions;
//import com.intellij.openapi.module.Module;
//import com.intellij.openapi.module.ModuleManager;
//import com.intellij.openapi.options.SettingsEditor;
//import com.intellij.openapi.options.SettingsEditorGroup;
//import com.intellij.openapi.project.Project;
//import com.intellij.openapi.projectRoots.ProjectJdkTable;
//import com.intellij.openapi.projectRoots.Sdk;
//import com.intellij.openapi.util.text.StringUtil;
//import com.intellij.openapi.vfs.VirtualFile;
//import org.jetbrains.annotations.NotNull;
////import st.redline.smalltalk.module.facet.RsFacet;
//
//import java.util.Arrays;
//import java.util.Collection;
//import java.util.HashMap;
//
//
//public class StratosRunConfiguration extends ApplicationConfiguration {
//    protected StratosRunConfiguration(String name, Project project, ConfigurationFactory factory) {
//        super(name, project, factory);
//        Module[] modules = ModuleManager.getInstance(project).getModules();
//        if (modules.length == 1) {
//            setModule(modules[0]);
//        }
//    }
//
//    public void setMainClass(String mainClass) {
//        MAIN_CLASS_NAME = mainClass;
//    }
//
//
//    @Override
//    public String suggestedName() {
//        if (StringUtil.isEmpty(MAIN_CLASS_NAME)) {
//            return getName();
//        }
//        if (MAIN_CLASS_NAME.contains(".")) {
//            return MAIN_CLASS_NAME.substring(MAIN_CLASS_NAME.lastIndexOf('.') + 1);
//        }
//        return MAIN_CLASS_NAME;
//    }
//
//    @Override
//    protected StratosRunConfiguration createInstance() {
//        return new StratosRunConfiguration(getName(), getProject(), StratosRunConfigurationType.getInstance().getConfigurationFactories()[0]);
//    }
//
//    public static StratosRunConfiguration newInstance(String name, Project project) {
//        return new StratosRunConfiguration(name, project, StratosRunConfigurationType.getInstance().getConfigurationFactories()[0]);
//    }
//
//    @Override
//    public SettingsEditor<? extends RunConfiguration> getConfigurationEditor() {
//        SettingsEditorGroup<ApplicationConfiguration> group = new SettingsEditorGroup<ApplicationConfiguration>();
////        group.addEditor(ExecutionBundle.message("run.configuration.configuration.tab.title"), new StratosRunConfigurationSettingsEditor(getProject()));
//        group.addEditor("Run Configuration", new StratosRunConfigurationSettingsEditor(getProject()));
//        JavaRunConfigurationExtensionManager.getInstance().appendEditors(this, group);
//        group.addEditor(ExecutionBundle.message("logs.tab.title"), new LogConfigurationPanel<ApplicationConfiguration>());
//        return group;
//    }
//
//    @Override
//    public Collection<Module> getValidModules() {
//        return Arrays.asList(ModuleManager.getInstance(getProject()).getModules());
//    }
//
//    @Override
//    public boolean canRunOn(@NotNull ExecutionTarget target) {
//        return super.canRunOn(target);
//    }
//
//    @Override
//    public RunProfileState getState(@NotNull Executor executor, @NotNull ExecutionEnvironment env) throws ExecutionException {
//        final JavaCommandLineState state = new JavaApplicationCommandLineState(this, env) {
//            @Override
//            protected JavaParameters createJavaParameters() throws ExecutionException {
//                final JavaParameters params = new JavaParameters();
//                params.getProgramParametersList().add(MAIN_CLASS_NAME);
//                final JavaRunConfigurationModule runConfigurationModule = getConfigurationModule();
//
//                final String jreHome = ALTERNATIVE_JRE_PATH_ENABLED ? ALTERNATIVE_JRE_PATH : null;
//                JavaParametersUtil.configureModule(runConfigurationModule, params, JavaParameters.JDK_AND_CLASSES, jreHome);
//                JavaParametersUtil.configureConfiguration(params, StratosRunConfiguration.this);
//
//                params.setMainClass("st.redline.core.Stic");
//
//                final Module module = runConfigurationModule.getModule();
//                if (module == null) {
//                    throw new ExecutionException("Module isn't set");
//                }
//                final StratosFacet facet = StratosFacet.getInstance(module);
//                if (facet == null) {
//                    throw new ExecutionException("No Redline Smalltalk Facet is installed");
//                }
//                final Sdk sdk = ProjectJdkTable.getInstance().findJdk(facet.getConfiguration().getState().redlineSmalltalkSdkName);
//                if (sdk == null) {
//                    throw new ExecutionException("No Redline Smalltalk SDK is set");
//                }
//                final VirtualFile directory = sdk.getHomeDirectory();
//
//                if (directory == null) {
//                    throw new ExecutionException("Redline SDK home is not set");
//                }
//                final VirtualFile lib = directory.findChild("lib");
//
//                if (lib == null) {
//                    throw new ExecutionException("Can't find 'lib' folder inside Redline SDK");
//                }
//
//                for (VirtualFile file : lib.getChildren()) {
//                    if ("jar".equalsIgnoreCase(file.getExtension())) {
//                        params.getClassPath().add(file);
//                    }
//                }
//                for(RunConfigurationExtension ext: Extensions.getExtensions(RunConfigurationExtension.EP_NAME)) {
//                    ext.updateJavaParameters(StratosRunConfiguration.this, params, getRunnerSettings());
//                }
//                final HashMap<String, String> map = new HashMap<String, String>();
//                map.put("STRATOS_HOME", sdk.getHomePath());
//                params.setEnv(map);
////                params.setWorkingDirectory(MakeUtil.getModuleOutputDirPath(module));
//                params.setWorkingDirectory("~~~~~~");
//                return params;
//            }
//        };
//        JavaRunConfigurationModule module = getConfigurationModule();
//        state.setConsoleBuilder(TextConsoleBuilderFactory.getInstance().createBuilder(getProject(), module.getSearchScope()));
//        return state;
//    }
//
//    @Override
//    public void checkConfiguration() throws RuntimeConfigurationException {
//        JavaParametersUtil.checkAlternativeJRE(this);
//        final JavaRunConfigurationModule configurationModule = getConfigurationModule();
//        //todo check smalltalk main class
//        ProgramParametersUtil.checkWorkingDirectoryExist(this, getProject(), configurationModule.getModule());
//        JavaRunConfigurationExtensionManager.checkConfigurationIsValid(this);
//    }
//}
