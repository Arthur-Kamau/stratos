package org.stratos.lang.action;

import com.intellij.openapi.actionSystem.*;
import com.intellij.openapi.project.Project;
import org.jetbrains.annotations.NotNull;

import com.intellij.openapi.module.Module;
import com.intellij.openapi.module.ModuleUtilCore;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.roots.ModuleRootManager;
import com.intellij.openapi.vfs.VirtualFile;
import org.stratos.lang.utils.StratosModuleUtils;

public class BuildActionGroup extends DefaultActionGroup {

    public enum BuildType {
        AAR("aar"),
        APK("apk"),
        APP_BUNDLE("appbundle"),
        IOS("ios");

        final public String type;

        BuildType(String type) {
            this.type = type;
        }
    }

    @Override
    public void update(AnActionEvent event) {
        final Presentation presentation = event.getPresentation();
        final boolean enabled =  true;//isInFlutterModule(event);
        presentation.setEnabled(enabled);
        presentation.setVisible(enabled);
    }

    private static boolean isInFlutterModule(@NotNull AnActionEvent event) {
        return findFlutterModule(event) != null;
    }

    private static Module findFlutterModule(@NotNull AnActionEvent event) {
        final Project project = event.getProject();
        if (project == null) {
            return null;
        }
        final VirtualFile file = event.getData(CommonDataKeys.VIRTUAL_FILE);
        if (file == null) {
            return null;
        }
        return findFlutterModule(project, file);
    }

    public static Module findFlutterModule(@NotNull Project project, @NotNull VirtualFile file) {
        Module module = ModuleUtilCore.findModuleForFile(file, project);
        if (module == null) {
            return null;
        }
        if (StratosModuleUtils.declaresFlutter(module)) {
            return module;
        }
        // We may get here if the file is in the Android module of a Flutter module project.
        final VirtualFile parent = ModuleRootManager.getInstance(module).getContentRoots()[0].getParent();
        module = ModuleUtilCore.findModuleForFile(parent, project);
        if (module == null) {
            return null;
        }
        return StratosModuleUtils.declaresFlutter(module) ? module : null;
    }


    abstract public static class FlutterBuildAction extends AnAction {

        abstract protected BuildType buildType();

        @Override
        public void actionPerformed(@NotNull AnActionEvent event) {
            final Presentation presentation = event.getPresentation();
            final Project project = event.getProject();
            if (project == null) {
                return;
            }
            System.out.println("TODO HERE");
//            final FlutterSdk sdk = FlutterSdk.getFlutterSdk(project);
//            if (sdk == null) {
//                return;
//            }
//            final PubRoot pubRoot = PubRoot.forEventWithRefresh(event);
//            if (pubRoot == null) {
//                return;
//            }
//            final BuildType buildType = buildType();
//            build(project, pubRoot, sdk, buildType, presentation.getDescription());
        }
    }

    public static class AAR extends FlutterBuildAction {
        @Override
        protected BuildType buildType() {
            return BuildType.AAR;
        }
    }

    public static class APK extends FlutterBuildAction {
        @Override
        protected BuildType buildType() {
            return BuildType.APK;
        }
    }

    public static class AppBundle extends FlutterBuildAction {
        @Override
        protected BuildType buildType() {
            return BuildType.APP_BUNDLE;
        }
    }

    public static class Ios extends FlutterBuildAction {
        @Override
        protected BuildType buildType() {
            return BuildType.IOS;
        }
    }
}
