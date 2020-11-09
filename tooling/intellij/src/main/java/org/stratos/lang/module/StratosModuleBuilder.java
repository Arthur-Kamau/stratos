package org.stratos.lang.module;


import com.intellij.ide.util.projectWizard.*;
import com.intellij.openapi.Disposable;
import com.intellij.openapi.module.Module;
import com.intellij.openapi.projectRoots.ProjectJdkTable;
import com.intellij.openapi.roots.ModifiableRootModel;
import com.intellij.openapi.roots.ui.configuration.ModulesProvider;
import com.intellij.openapi.vfs.LocalFileSystem;
import com.intellij.openapi.vfs.VirtualFile;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.jetbrains.jps.model.serialization.PathMacroUtil;
import org.stratos.lang.module.sdk.StratosSdkType;
import org.stratos.lang.module.wizard.StratosProjectTypeWizardStep;
import org.stratos.lang.module.wizard.StratosSdkWizardStep;

import java.io.File;
import java.io.IOException;


/**
 * @author Arthur Kamau
 */
public  class StratosModuleBuilder  extends ModuleBuilder {


    @Override
    public void setupRootModel(@NotNull ModifiableRootModel model) {

        String contentEntryPath = getContentEntryPath();
        if (contentEntryPath == null) return;

        VirtualFile contentRoot = LocalFileSystem.getInstance().findFileByPath(contentEntryPath);
        if (contentRoot == null) return;

        final com.intellij.openapi.module.Module module = model.getModule();

        //todo
        // check the project type selected in previous dialog.
        // if script/library/project

        //  create directory for the sources if not already there
        File directory = new File(PathMacroUtil.getModuleDir(module.getModuleFilePath()), "src");
        File configFile = new File(PathMacroUtil.getModuleDir(module.getModuleFilePath()), "app.config");
        File mainFile = new File(PathMacroUtil.getModuleDir(module.getModuleFilePath()), "src/main.st");
        if(!directory.exists()) {
            directory.mkdirs();
            try {
                configFile.createNewFile();
                mainFile.createNewFile();
            } catch (IOException e) {
                System.out.println("===========================================");
                System.out.println("Error cause "+e.getCause() + " message " + e.getMessage());
                e.printStackTrace();

            }
        }

        // kick  virtual file system in the privates,  so it refreshes
        // and we can mark directory as sources
        VirtualFile virtualFile = LocalFileSystem.getInstance().refreshAndFindFileByIoFile(directory);



    }




    @Override
    public StratosModuleType getModuleType() {
        return StratosModuleType.getInstance();
    }

    @Override
    public String getDescription() {
        if (ProjectJdkTable.getInstance().getSdksOfType(StratosSdkType.getInstance()).isEmpty()) {
            return "<html><body>Before you start make sure you have Stratos compiler installed." +
                    "<br/>Download <a href='https://github.com/Arthur-Kamau/stratos'>the latest version</a>" +
                    "<br/>Unpack the zip file to any folder and run the installer " +
                    "<br/>Make sure the installer is in system path " +
                    "</body></html>";
        } else {
            return "Stratos Project";
        }
    }

    @Override
    public ModuleWizardStep[] createWizardSteps(@NotNull WizardContext wizardContext, @NotNull ModulesProvider modulesProvider) {

        return new ModuleWizardStep[]{new StratosProjectTypeWizardStep() };

    }


    @Nullable
    @Override
    public ModuleWizardStep getCustomOptionsStep(WizardContext context, Disposable parentDisposable) {
        return new StratosSdkWizardStep();
    }



}
