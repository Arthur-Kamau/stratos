package org.stratos.lang.action;

import com.intellij.notification.NotificationDisplayType;
import com.intellij.notification.NotificationGroup;
import com.intellij.openapi.actionSystem.*;
import com.intellij.openapi.project.Project;
import org.jetbrains.annotations.NotNull;

import com.intellij.openapi.module.Module;
import com.intellij.openapi.module.ModuleUtilCore;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.roots.ModuleRootManager;
import com.intellij.openapi.vfs.VirtualFile;
import org.stratos.lang.notification.NotificationConfigBuilder;
import org.stratos.lang.utils.StratosModuleUtils;
import com.intellij.notification.NotificationType;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.startup.StartupActivity;
import org.jetbrains.annotations.NotNull;

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
            // notification group for non-sticky balloon notifications

            NotificationConfigBuilder builder = NotificationConfigBuilder.create(project);
            builder.setNotificationType(NotificationType.INFORMATION);
            builder.setTitle("Configurable Notification");
            builder.setSubtitle("user-defined properties");
            builder.setContent("<html>Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt " +
                    "ut labore et dolore magna aliqua. Ut enim ad minim veniam, " +
                    "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. " +
                    "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur." +
                    "<br><strong style=\"font-size:larger;\">Open article on <a href=\"https://www.plugin-dev.com/\">plugin-dev.com</a>.</strong></html>");
            builder.addDefaultActions();
            builder.build().notify(project);


            System.out.println("TODO HERE");
//            final FlutterSdk sdk = FlutterSdk.getFlutterSdk(project);
//            if (sdk == null) {
//                return;
//            }
//            final PubRoot pubRoot = PubRoot.forEventWithRefresh(event);
//            if (pubRoot == null) {
//                return;
//            }

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
