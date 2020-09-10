package lang.stratos.module;


import com.intellij.facet.FacetManager;
import com.intellij.facet.FacetType;
import com.intellij.facet.FacetTypeRegistry;
import com.intellij.ide.util.projectWizard.*;
import com.intellij.openapi.Disposable;
import com.intellij.openapi.roots.ContentEntry;
import com.intellij.openapi.roots.ModifiableRootModel;
import com.intellij.openapi.vfs.LocalFileSystem;
import com.intellij.openapi.vfs.VirtualFile;
import lang.stratos.module.facet.StratosFacetType;
import lang.stratos.module.wizard.StratosModuleWizardStep;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.jetbrains.jps.model.serialization.PathMacroUtil;

import java.io.File;
import java.io.IOException;

import static com.intellij.openapi.vfs.VfsUtilCore.isEqualOrAncestor;


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

    @Nullable
    @Override
    public ModuleWizardStep getCustomOptionsStep(WizardContext context, Disposable parentDisposable) {
        return new StratosModuleWizardStep();
    }


}

//public class StratosModuleBuilder  JavaModuleBuilder implements ModuleBuilderListener {
//    private Sdk mySdk;
//
//    public StratosModuleBuilder() {
//        addListener(this);
//    }
//
//    @Override
//    public String getBuilderId() {
//        return "stratos";
//    }
//
////        @Override
////        public Icon getBigIcon() {
////        return null;
////        }
//
//    @Override
//    public Icon getNodeIcon() {
//        return StratosIcons.StratosFileIcon; //SmalltalkIcons.Redline;
//    }
//
//    @Override
//    public String getDescription() {
//        if (ProjectJdkTable.getInstance().getSdksOfType(StratosSdkType.getInstance()).isEmpty()) {
//            return "<html><body>Before you start make sure you have Redline Smalltalk installed." +
//                    "<br/>Download <a href='https://github.com/redline-smalltalk/redline-smalltalk.github.com/raw/master/assets/redline-deploy.zip'>the latest version</a>" +
//                    "<br/>Unpack the zip file to any folder and select it as Redline SDK</body></html>";
//        } else {
//            return "Redline Smalltalk module";
//        }
//    }
//
//    @Override
//    public String getPresentableName() {
//        return "Stratos Module";
//    }
//
//    @Override
//    public String getGroupName() {
//        return "Stratos";
//    }
//
//    @Override
//    public ModuleWizardStep[] createWizardSteps(WizardContext wizardContext, ModulesProvider modulesProvider) {
//        return new ModuleWizardStep[]{new StratosModuleWizardStep(this)};
//    }
//
//    public void moduleCreated(@NotNull final Module module) {
//        if (mySdk != null && mySdk.getSdkType() instanceof StratosSdkType) {
//            setupFacet(module, mySdk);
//            VirtualFile[] roots = ModuleRootManager.getInstance(module).getSourceRoots();
//            if (roots.length == 1) {
//                VirtualFile srcRoot = roots[0];
//                if (srcRoot.getName().equals("stratos")) {
//                    VirtualFile main = srcRoot.getParent();
//                    if (main != null && "main".equals(main.getName())) {
//                        final VirtualFile src = main.getParent();
//                        if (src != null) {
//                            ApplicationManager.getApplication().runWriteAction(new Runnable() {
//                                @Override
//                                public void run() {
//                                    try {
//                                        VirtualFile test = src.createChildDirectory(this, "test");
//                                        if (test != null) {
//                                            VirtualFile testSrc = test.createChildDirectory(this, "smalltalk");
//                                            ModifiableRootModel model = ModuleRootManager.getInstance(module).getModifiableModel();
//                                            ContentEntry entry = findContentEntry(model, testSrc);
//                                            if (entry != null) {
//                                                entry.addSourceFolder(testSrc, true);
//                                                model.commit();
//                                            }
//                                        }
//                                    } catch (IOException e) {//
//                                    }
//                                }
//                            });
//                        }
//                    }
//                }
//            }
//
//        }
//    }
//
//    @Nullable
//    public static ContentEntry findContentEntry(@NotNull ModuleRootModel model, @NotNull VirtualFile vFile) {
//        final ContentEntry[] contentEntries = model.getContentEntries();
//        for (ContentEntry contentEntry : contentEntries) {
//            final VirtualFile contentEntryFile = contentEntry.getFile();
//            if (contentEntryFile != null && VfsUtilCore.isAncestor(contentEntryFile, vFile, false)) {
//                return contentEntry;
//            }
//        }
//        return null;
//    }
//
//    public static void setupFacet(Module module, Sdk sdk) {
//        final String facetId = StratosFacetType.getInstance().getStringId();
//        if (!StringUtil.isEmptyOrSpaces(facetId)) {
//
//            final FacetManager facetManager = FacetManager.getInstance(module);
//            final FacetType<?, ?> type = FacetTypeRegistry.getInstance().findFacetType(facetId);
//
//            if (type != null) {
//
//                if (facetManager.getFacetByType(type.getId()) == null) {
//                    final ModifiableFacetModel model = facetManager.createModifiableModel();
//
//                    final StratosFacet facet = (StratosFacet) facetManager.addFacet(type, type.getDefaultFacetName(), null);
//                    facet.getConfiguration().setSdk(sdk);
//                    model.addFacet(facet);
//                    model.commit();
//                }
//            }
//        }
//    }
//
//
//    public void setSdk(Sdk sdk) {
//        mySdk = sdk;
//    }
//
//    @Nullable
//    @Override
//    public ModuleWizardStep modifySettingsStep(SettingsStep settingsStep) {
//        JavaSettingsStep step = (JavaSettingsStep) super.modifySettingsStep(settingsStep);
//        if (step != null) {
//            step.setSourcePath("src" + File.separator + "main" + File.separator + "smalltalk");
//        }
//        return step;
//    }

//
//        @NotNull
//        @Override
//        protected List<WizardInputField<?>> getAdditionalFields() {
////                return super.getAdditionalFields();
//                return Arrays.<WizardInputField>asList(new StratosWizardInputField());
//        }

    //        @Override
//        protected List<WizardInputField> getAdditionalFields() {
////        return Arrays.<WizardInputField>asList(new StratosWizardInputField());
//                return Arrays.<WizardInputField>asList(new StratosWizardInputField());
//        }
//}
