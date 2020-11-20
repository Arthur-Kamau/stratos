package org.stratos.lang.module;



import com.intellij.execution.OutputListener;
import com.intellij.execution.process.ProcessListener;
import com.intellij.ide.util.projectWizard.ModuleBuilder;
import com.intellij.ide.util.projectWizard.ModuleWizardStep;
import com.intellij.ide.util.projectWizard.SettingsStep;
import com.intellij.ide.util.projectWizard.WizardContext;
import com.intellij.openapi.Disposable;
import com.intellij.openapi.application.WriteAction;
import com.intellij.openapi.diagnostic.Logger;
import com.intellij.openapi.module.Module;
import com.intellij.openapi.module.*;
import com.intellij.openapi.options.ConfigurationException;
import com.intellij.openapi.progress.ProgressManager;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.roots.ModifiableRootModel;
import com.intellij.openapi.ui.Messages;
import com.intellij.openapi.util.Disposer;
import com.intellij.openapi.util.text.StringUtil;
import com.intellij.openapi.vfs.LocalFileSystem;
import com.intellij.openapi.vfs.VirtualFile;
//import icons.FlutterIcons;
//import io.flutter.StratosBundle;
//import io.flutter.FlutterConstants;
//import io.flutter.FlutterMessages;
//import io.flutter.StratosUtils;
//import io.flutter.actions.FlutterDoctorAction;
//import org.stratos.lang.module.settings.FlutterCreateAdditionalSettingsFields;
//import io.flutter.pub.PubRoot;
//import io.flutter.sdk.FlutterCreateAdditionalSettings;
//import io.flutter.sdk.FlutterSdk;
//import io.flutter.sdk.FlutterSdkUtil;
//import io.flutter.utils.AndroidUtils;
//import io.flutter.utils.FlutterModuleUtils;
import org.apache.commons.lang.StringUtils;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.stratos.lang.StratosIcons;
//import org.stratos.lang.bundle.StratosBundle;
import org.stratos.lang.module.sdk.StratosCreateAdditionalSettings;
import org.stratos.lang.module.sdk.StratosGeneratorPeer;
import org.stratos.lang.module.settings.FlutterCreateAdditionalSettingsFields;
import org.stratos.lang.utils.*;

import javax.swing.*;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicReference;

import static java.util.Arrays.asList;

public class StratosModuleBuilder extends ModuleBuilder {
    private static final Logger LOG = Logger.getInstance(StratosModuleBuilder.class);

    private StratosModuleWizardStep myStep;
    private FlutterCreateAdditionalSettingsFields mySettingsFields;
    private Project myProject;

    @Override
    public String getName() {
        return getPresentableName();
    }

    @Nullable
    public Project getProject() {
        return myProject; // Non-null when creating a module.
    }

    @Override
    public String getPresentableName() {
        return "Stratos";
    }

    @Override
    public String getDescription() {
        return "description .............  ";
//        return StratosBundle.message("flutter.project.description");
    }

    @Override
    public Icon getNodeIcon() {
        return StratosIcons.SAMPLE_ICON;
    //FlutterIcons.Flutter;
    }

    @Override
    public void setupRootModel(@NotNull ModifiableRootModel model) {
        doAddContentEntry(model);
        // Add a reference to Dart SDK project library, without committing.
        model.addInvalidLibrary("Dart SDK", "project");
    }

    protected StratosSdk getFlutterSdk() {
        return myStep.getFlutterSdk();
    }

    @Nullable
    @Override
    public Module commitModule(@NotNull Project project, @Nullable ModifiableModuleModel model) {
        final String basePath = getModuleFileDirectory();
        if (basePath == null) {
            Messages.showErrorDialog("Module path not set", "Internal Error");
            return null;
        }
        final VirtualFile baseDir = LocalFileSystem.getInstance().refreshAndFindFileByPath(basePath);
        if (baseDir == null) {
            Messages.showErrorDialog("Unable to determine Flutter project directory", "Internal Error");
            return null;
        }

        final StratosSdk sdk = getFlutterSdk();
        if (sdk == null) {
            Messages.showErrorDialog("Flutter SDK not found", "Error");
            return null;
        }

        final OutputListener listener = new OutputListener();
        final PubRoot root = runFlutterCreateWithProgress(baseDir, sdk, project, listener, getAdditionalSettings());
        if (root == null) {
            final String stderr = listener.getOutput().getStderr();
            final String msg = stderr.isEmpty() ? "Flutter create command was unsuccessful" : stderr;
            System.out.println("Error "+msg);
//            final int code = FlutterMessages.showDialog(project, msg, "Project Creation Error", new String[]{"Run Flutter Doctor", "Cancel"}, 0);
//            if (code == 0) {
//                new FlutterDoctorAction().startCommand(project, sdk, null);
//            }
            return null;
        }
        StratosSdkUtil.updateKnownSdkPaths(sdk.getHomePath());

        // Create the Flutter module. This indirectly calls setupRootModule, etc.
        final Module flutter = super.commitModule(project, model);
        if (flutter == null) {
            return null;
        }

//        StratosModuleUtils.autoShowMain(project, root);
//
//        if (!(AndroidUtils.isAndroidProject(getProject()) &&
//                (getAdditionalSettings().getType() == FlutterProjectType.MODULE))) {
//            addAndroidModule(project, model, basePath, flutter.getName());
//        }

        return flutter;
    }

    private static String validateSettings(StratosCreateAdditionalSettings settings) {
        final String description = settings.getDescription();
        if (description != null && description.contains(": ")) {
            return "Invalid package description: '" + description + "' - cannot contain the sequence ': '.";
        }
        final String org = settings.getOrg();
        if (org == null) {
            return null;
        }
        if (StringUtils.endsWith(org, ".")) {
            return "Invalid organization name: '" + org + "' - cannot end in '.'.";
        }
        // Invalid package names will cause issues down the line.
        return  org ; // AndroidUtils.validateAndroidPackageName(org);
    }

    private static void addAndroidModule(@NotNull Project project,
                                         @Nullable ModifiableModuleModel model,
                                         @NotNull String baseDirPath,
                                         @NotNull String flutterModuleName) {
        final VirtualFile baseDir = LocalFileSystem.getInstance().refreshAndFindFileByPath(baseDirPath);
        if (baseDir == null) {
            return;
        }

        final VirtualFile androidFile = findAndroidModuleFile(baseDir, flutterModuleName);
        if (androidFile == null) return;

        try {
            final ModifiableModuleModel toCommit;
            if (model == null) {
                toCommit = ModuleManager.getInstance(project).getModifiableModel();
                //noinspection AssignmentToMethodParameter
                model = toCommit;
            }
            else {
                toCommit = null;
            }

            model.loadModule(androidFile.getPath());

            if (toCommit != null) {
                WriteAction.run(toCommit::commit);
            }
        }
        catch (ModuleWithNameAlreadyExists | IOException e) {
            StratosUtils.warn(LOG, e);
        }
    }

    @Nullable
    private static VirtualFile findAndroidModuleFile(@NotNull VirtualFile baseDir, String flutterModuleName) {
        baseDir.refresh(false, false);
        for (String name : asList(flutterModuleName + "_android.iml", "android.iml")) {
            final VirtualFile candidate = baseDir.findChild(name);
            if (candidate != null && candidate.exists()) {
                return candidate;
            }
        }
        return null;
    }

    @Override
    public boolean validate(@Nullable Project current, @NotNull Project dest) {
        final String settingsValidation = validateSettings(getAdditionalSettings());
        if (settingsValidation != null) {
            Messages.showErrorDialog(settingsValidation, "Error");
            return false;
        }

        return myStep.getFlutterSdk() != null;
    }

    /**
     * @see <a href="dart.dev/tools/pub/pubspec#name">https://dart.dev/tools/pub/pubspec#name</a>
     */
    @Override
    public boolean validateModuleName(@NotNull String moduleName) throws ConfigurationException {
        if (!StratosUtils.isValidPackageName(moduleName)) {
            throw new ConfigurationException(
                    "Invalid module name: '" + moduleName + "' - must be a valid Dart package name (lower_case_with_underscores).");
        }

        if (StratosUtils.isDartKeyword(moduleName)) {
            throw new ConfigurationException("Invalid module name: '" + moduleName + "' - must not be a Dart keyword.");
        }

        if (!StratosUtils.isValidDartIdentifier(moduleName)) {
            throw new ConfigurationException("Invalid module name: '" + moduleName + "' - must be a valid Dart identifier.");
        }

        if (StratosConstants.FLUTTER_PACKAGE_DEPENDENCIES.contains(moduleName)) {
            throw new ConfigurationException("Invalid module name: '" + moduleName + "' - this will conflict with Flutter package dependencies.");
        }

        if (moduleName.length() > StratosConstants.MAX_MODULE_NAME_LENGTH) {
            throw new ConfigurationException("Invalid module name - must be less than " +
                    StratosConstants.MAX_MODULE_NAME_LENGTH +
                    " characters.");
        }

        return super.validateModuleName(moduleName);
    }

    @NotNull
    public StratosCreateAdditionalSettings getAdditionalSettings() {
        return mySettingsFields.getAdditionalSettings();
    }

    @Nullable
    @Override
    public ModuleWizardStep modifySettingsStep(@NotNull SettingsStep settingsStep) {
        final ModuleWizardStep wizard = super.modifySettingsStep(settingsStep);
        mySettingsFields.addSettingsFields(settingsStep);
        return wizard;
    }

    @Override
    public ModuleWizardStep modifyProjectTypeStep(@NotNull SettingsStep settingsStep) {
        // Don't allow super to add an SDK selection field (#2052).
        return null;
    }

    @Nullable
    @Override
    public ModuleWizardStep getCustomOptionsStep(final WizardContext context, final Disposable parentDisposable) {
        if (!context.isCreatingNewProject()) {
            myProject = context.getProject();
        }
        myStep = new StratosModuleWizardStep(context);
        mySettingsFields = new FlutterCreateAdditionalSettingsFields(new StratosCreateAdditionalSettings(), this::getFlutterSdk);
        Disposer.register(parentDisposable, myStep);
        return myStep;
    }

    @Override
    @NotNull
    public String getBuilderId() {
        // The builder id is used to distinguish between different builders with the same module type, see
        // com.intellij.ide.projectWizard.ProjectTypeStep for an example.
        return StringUtil.notNullize(super.getBuilderId()) + "_" + StratosModuleBuilder.class.getCanonicalName();
    }

    @Override
    @NotNull
    public ModuleType<?> getModuleType() {
        return StratosModuleUtils.getFlutterModuleType();
    }

    /**
     * Runs flutter create without showing a console, but with an indeterminate progress dialog.
     * <p>
     * Returns the PubRoot if successful.
     */
    @Nullable
    public static PubRoot runFlutterCreateWithProgress(@NotNull VirtualFile baseDir,
                                                       @NotNull StratosSdk sdk,
                                                       @NotNull Project project,
                                                       @Nullable ProcessListener processListener,
                                                       @Nullable StratosCreateAdditionalSettings additionalSettings) {
        final ProgressManager progress = ProgressManager.getInstance();
        final AtomicReference<PubRoot> result = new AtomicReference<>(null);

        StratosUtils.disableGradleProjectMigrationNotification(project);

        progress.runProcessWithProgressSynchronously(() -> {
            progress.getProgressIndicator().setIndeterminate(true);
            result.set(sdk.createFiles(baseDir, null, processListener, additionalSettings));
        }, "Creating Flutter project", false, project);

        return result.get();
    }

    public void setFlutterSdkPath(String s) {
        final ComboBoxEditor combo = myStep.myPeer.getSdkEditor();
        combo.setItem(s);
    }

    public class StratosModuleWizardStep extends ModuleWizardStep implements Disposable {
        private final StratosGeneratorPeer myPeer;

        public StratosModuleWizardStep(@NotNull WizardContext context) {
            //TODO(pq): find a way to listen to wizard cancelation and propagate to peer.
            myPeer = new StratosGeneratorPeer(context);
        }

        @Override
        public JComponent getComponent() {
            return myPeer.getComponent();
        }

        @Override
        public void updateDataModel() {
        }

        @Override
        public boolean validate() {
            final boolean valid = myPeer.validate();
            if (valid) {
                myPeer.apply();
            }
            return valid;
        }

        @Override
        public void dispose() {
        }

        @Nullable
        StratosSdk getFlutterSdk() {
            final String sdkPath = myPeer.getSdkComboPath();

            // Ensure the local filesystem has caught up to external processes (e.g., git clone).
            if (!sdkPath.isEmpty()) {
                try {
                    LocalFileSystem
                            .getInstance().refreshAndFindFileByPath(sdkPath);
                }
                catch (Throwable e) {
                    // It's possible that the refresh will fail in which case we just want to trap and ignore.
                }
            }
            return StratosSdk.forPath(sdkPath);
        }
    }
}

//import com.intellij.ide.util.projectWizard.*;
//import com.intellij.openapi.Disposable;
//import com.intellij.openapi.module.Module;
//import com.intellij.openapi.projectRoots.ProjectJdkTable;
//import com.intellij.openapi.roots.ModifiableRootModel;
//import com.intellij.openapi.roots.ui.configuration.ModulesProvider;
//import com.intellij.openapi.vfs.LocalFileSystem;
//import com.intellij.openapi.vfs.VirtualFile;
//import org.jetbrains.annotations.NotNull;
//import org.jetbrains.annotations.Nullable;
//import org.jetbrains.jps.model.serialization.PathMacroUtil;
//import org.stratos.lang.module.sdk.StratosSdkType;
//import org.stratos.lang.module.wizard.StratosProjectTypeWizardStep;
//import org.stratos.lang.module.wizard.StratosSdkWizardStep;
//
//import java.io.File;
//import java.io.IOException;
//
//
///**
// * @author Arthur Kamau
// */
//public  class StratosModuleBuilder  extends ModuleBuilder {
//
//
//    @Override
//    public void setupRootModel(@NotNull ModifiableRootModel model) {
//
//        String contentEntryPath = getContentEntryPath();
//        if (contentEntryPath == null) return;
//
//        VirtualFile contentRoot = LocalFileSystem.getInstance().findFileByPath(contentEntryPath);
//        if (contentRoot == null) return;
//
//        final com.intellij.openapi.module.Module module = model.getModule();
//
//        //todo
//        // check the project type selected in previous dialog.
//        // if script/library/project
//
//        //  create directory for the sources if not already there
//        File directory = new File(PathMacroUtil.getModuleDir(module.getModuleFilePath()), "src");
//        File configFile = new File(PathMacroUtil.getModuleDir(module.getModuleFilePath()), "app.config");
//        File mainFile = new File(PathMacroUtil.getModuleDir(module.getModuleFilePath()), "src/main.st");
//        if(!directory.exists()) {
//            directory.mkdirs();
//            try {
//                configFile.createNewFile();
//                mainFile.createNewFile();
//            } catch (IOException e) {
//                System.out.println("===========================================");
//                System.out.println("Error cause "+e.getCause() + " message " + e.getMessage());
//                e.printStackTrace();
//
//            }
//        }
//
//        // kick  virtual file system in the privates,  so it refreshes
//        // and we can mark directory as sources
//        VirtualFile virtualFile = LocalFileSystem.getInstance().refreshAndFindFileByIoFile(directory);
//
//
//
//    }
//
//
//
//
//    @Override
//    public StratosModuleType getModuleType() {
//        return StratosModuleType.getInstance();
//    }
//
//    @Override
//    public String getDescription() {
//        if (ProjectJdkTable.getInstance().getSdksOfType(StratosSdkType.getInstance()).isEmpty()) {
//            return "<html><body>Before you start make sure you have Stratos compiler installed." +
//                    "<br/>Download <a href='https://github.com/Arthur-Kamau/stratos'>the latest version</a>" +
//                    "<br/>Unpack the zip file to any folder and run the installer " +
//                    "<br/>Make sure the installer is in system path " +
//                    "</body></html>";
//        } else {
//            return "Stratos Project";
//        }
//    }
//
//    @Override
//    public ModuleWizardStep[] createWizardSteps(@NotNull WizardContext wizardContext, @NotNull ModulesProvider modulesProvider) {
//
//        return new ModuleWizardStep[]{new StratosProjectTypeWizardStep() };
//
//    }
//
//
//    @Nullable
//    @Override
//    public ModuleWizardStep getCustomOptionsStep(WizardContext context, Disposable parentDisposable) {
////        return new StratosSdkWizardStep();
//    }
//
//
//
//}
