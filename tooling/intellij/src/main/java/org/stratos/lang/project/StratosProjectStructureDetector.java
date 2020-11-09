package org.stratos.lang.project;

//import com.intellij.ide.util.importProject.ModuleDescriptor;
//import com.intellij.ide.util.importProject.ProjectDescriptor;
//import com.intellij.ide.util.projectWizard.ModuleBuilder;
//import com.intellij.ide.util.projectWizard.ModuleWizardStep;
//import com.intellij.ide.util.projectWizard.ProjectWizardStepFactory;
//import com.intellij.ide.util.projectWizard.importSources.DetectedProjectRoot;
//import com.intellij.ide.util.projectWizard.importSources.ProjectFromSourcesBuilder;
//import com.intellij.ide.util.projectWizard.importSources.ProjectStructureDetector;
//import com.intellij.openapi.module.JavaModuleType;
//import com.intellij.openapi.module.Module;
//import com.intellij.openapi.roots.ModifiableRootModel;
//import com.intellij.openapi.util.io.FileUtilRt;
//import org.jetbrains.annotations.NotNull;

import javax.swing.*;
import java.io.File;
import java.util.*;

/**
 * @author Arthur Kamau
 */
public class StratosProjectStructureDetector {
//        extends ProjectStructureDetector {
//
//  @NotNull
//  @Override
//  public DirectoryProcessingResult detectRoots(@NotNull File dir,
//                                               @NotNull File[] children,
//                                               @NotNull File base,
//                                               @NotNull List<DetectedProjectRoot> result) {
//    for (File child : children) {
//      if (FileUtilRt.extensionEquals(child.getName(), "st")) {
//        File root;
//        for (String pattern : new String[]{"src/main/smalltalk", "src/test/smalltalk", "src"}) {
//          if ((root = findParentLike(pattern, dir, base)) != null) {
//            result.add(new StratosDetectedSourceRoot(root));
//            return DirectoryProcessingResult.SKIP_CHILDREN;
//          }
//        }
//      }
//    }
//    return DirectoryProcessingResult.PROCESS_CHILDREN;
//  }
//
//  private File findParentLike(String pattern, File dir, File limit) {
//    String[] names = pattern.split("/");
//    Collections.reverse(Arrays.asList(names));
//
//    while (dir != null && dir.getPath().startsWith(limit.getPath())) {
//      if (names[0].equals(dir.getName())) {
//        if (checkParents(dir, names)) {
//          return dir;
//        }
//      }
//
//      dir = dir.getParentFile();
//    }
//    return null;
//  }
//
//  private boolean checkParents(File dir, String[] names) {
//    for (String name : names) {
//      if (dir.getName().equals(name)) {
//        dir = dir.getParentFile();
//      } else {
//        return false;
//      }
//    }
//    return true;
//  }
//
//  @Override
//  public void setupProjectStructure(@NotNull Collection<DetectedProjectRoot> roots,
//                                    @NotNull ProjectDescriptor projectDescriptor,
//                                    @NotNull final ProjectFromSourcesBuilder builder) {
//    if (!roots.isEmpty() && !builder.hasRootsFromOtherDetectors(this)) {
//      List<ModuleDescriptor> modules = projectDescriptor.getModules();
//      if (modules.isEmpty()) {
//        modules = new ArrayList<ModuleDescriptor>();
//        for (DetectedProjectRoot root : roots) {
//          modules.add(new ModuleDescriptor(new File(builder.getBaseProjectPath()), JavaModuleType.getModuleType(), root){
//
//            @Override
//            public void updateModuleConfiguration(Module module, ModifiableRootModel rootModel) {
//              super.updateModuleConfiguration(module, rootModel);
//              for (ModuleBuilder moduleBuilder : builder.getContext().getProjectBuilder()) {
//                if (moduleBuilder instanceof StratosModuleBuilder) {
//                  ((StratosModuleBuilder) moduleBuilder).moduleCreated(module);
//                  return;
//                }
//              }
//            }
//          });
//        }
//        projectDescriptor.setModules(modules);
//      }
//    }
//  }
//
//  @Override
//  public List<ModuleWizardStep> createWizardSteps(ProjectFromSourcesBuilder builder, ProjectDescriptor projectDescriptor, Icon stepIcon) {
//    for (ModuleBuilder moduleBuilder : builder.getContext().getAllBuilders()) {
//      if (moduleBuilder instanceof StratosModuleBuilder) {
//        ArrayList<ModuleWizardStep> steps = new ArrayList<ModuleWizardStep>();
//        steps.add(ProjectWizardStepFactory.getInstance().createProjectJdkStep(builder.getContext()));
//        steps.add(new StratosModuleWizardStep((StratosModuleBuilder) moduleBuilder));
//        return steps;
//      }
//    }
//    return Collections.emptyList();
//  }
//
//  @Override
//  public String getDetectorId() {
//    return "Stratos";
//  }
}
