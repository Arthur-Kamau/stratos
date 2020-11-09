package org.stratos.lang.module;


import com.intellij.ide.util.projectWizard.ModuleBuilder;
import com.intellij.ide.util.projectWizard.ModuleWizardStep;
import com.intellij.ide.util.projectWizard.WizardContext;
import com.intellij.openapi.Disposable;
import com.intellij.openapi.module.ModuleType;
import com.intellij.openapi.roots.ContentEntry;
import com.intellij.openapi.roots.ModifiableRootModel;
import com.intellij.openapi.vfs.VirtualFile;
import org.jetbrains.annotations.Nullable;

import javax.naming.ConfigurationException;

import static com.intellij.openapi.vfs.VfsUtilCore.isEqualOrAncestor;



// module type builder is responsible  for creating  and configuring modules
// it provides instance of module type,  and  IDE does some magic
public class StratosModuleTypeBuilder {
//        extends ModuleBuilder {
//
//
//    /**
//     * configure root morel for the module -  source roots, facets etc.
//     *
//     * @param modifiableRootModel
//     * @throws ConfigurationException
//     */
//    @Override
//    public void setupRootModel(ModifiableRootModel modifiableRootModel) throws ConfigurationException {
//        doAddContentEntry(modifiableRootModel);
//
//        String contentEntryPath = getContentEntryPath();
//        if (contentEntryPath == null) return;
//
//        VirtualFile contentRoot = LocalFileSystem.getInstance().findFileByPath(contentEntryPath);
//        if (contentRoot == null) return;
//
//        final Module module = modifiableRootModel.getModule();

//        //  create directory for the sources if not already there
//        File directory = new File(PathMacroUtil.getModuleDir(module.getModuleFilePath()), "mmtalk");
//        if(!directory.exists()) {
//            directory.mkdirs();
//        }
//
//        // kick  virtual file system in the privates,  so it refreshes
//        // and we can mark directory as sources
//        VirtualFile virtualFile = LocalFileSystem.getInstance().refreshAndFindFileByIoFile(directory);
//
//        ContentEntry e = getContentRootFor(virtualFile, modifiableRootModel);
//        e.addSourceFolder(virtualFile.getUrl(), false);
//
//        // Cave at:  do not commit model,  this will be done by framework
//
//        // time to create a facet -  we will need one for this module anyway
//        FacetType facetType = FacetTypeRegistry.getInstance().findFacetType(PmFacetType.ID);
//        FacetManager.getInstance(module).addFacet(facetType, facetType.getDefaultFacetName(), null);

//    }
//
//    // provide module type we are creating
//    @Override
//    public ModuleType getModuleType() {
//        return StratosModuleType.getInstance();
//    }
//
//    // not sure what to do here,   maybe latere there will be proper GUI for configuration.
//    // for now - just placeholder
//    @Nullable
//    @Override
//    public ModuleWizardStep getCustomOptionsStep(WizardContext context, Disposable parentDisposable) {
//        return new PmModuleWizardStep();
//    }
//
//
//
//    @Nullable
//    private ContentEntry getContentRootFor(VirtualFile url, ModifiableRootModel rootModel) {
//        for (ContentEntry e : rootModel.getContentEntries()) {
//            if (isEqualOrAncestor(e.getUrl(), url.getUrl())) return e;
//        }
//        return null;
//    }

}