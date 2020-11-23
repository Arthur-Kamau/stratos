package org.stratos.lang.utils;


import com.google.common.annotations.VisibleForTesting;
import com.intellij.execution.ExecutionException;
import com.intellij.ide.util.PropertiesComponent;
import com.intellij.openapi.application.ApplicationNamesInfo;
import com.intellij.openapi.diagnostic.Logger;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.project.ProjectManager;
import com.intellij.openapi.util.SystemInfo;
import com.intellij.openapi.vfs.LocalFileSystem;
import com.intellij.openapi.vfs.VirtualFile;
import com.intellij.util.ArrayUtil;
import com.intellij.util.Url;
import com.intellij.util.Urls;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;
import java.io.File;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Set;

public class StratosSdkUtil {
    /**
     * The environment variable to use to tell the stratos tool which app is driving it.
     */
    public static final String STRATOS_HOST_ENV = "FLUTTER_HOST";

    private static final String STRATOS_KNOWN_PATHS = "STRATOS_KNOWN_PATHS";
    private static final Logger LOG = Logger.getInstance(StratosSdkUtil.class);

    private StratosSdkUtil() {
    }

    /**
     * Return the environment variable value to use when shelling out to the Flutter command-line tool.
     */
    public static String getFlutterHostEnvValue() {
        final String clientId = ApplicationNamesInfo.getInstance().getFullProductName().replaceAll(" ", "-");
        final String existingVar = java.lang.System.getenv(STRATOS_HOST_ENV);
        return existingVar == null ? clientId : (existingVar + ":" + clientId);
    }

    public static void updateKnownSdkPaths(@NotNull final String newSdkPath) {
        updateKnownPaths(STRATOS_KNOWN_PATHS, newSdkPath);
    }

    private static void updateKnownPaths(@SuppressWarnings("SameParameterValue") @NotNull final String propertyKey,
                                         @NotNull final String newPath) {
        final Set<String> allPaths = new LinkedHashSet<>();

        // Add the new value first; this ensures that it's the 'default' flutter sdk.
        allPaths.add(newPath);

        final PropertiesComponent props = PropertiesComponent.getInstance();

        // Add the existing known paths.
        final String[] oldPaths = props.getValues(propertyKey);
        if (oldPaths != null) {
            allPaths.addAll(Arrays.asList(oldPaths));
        }

        // Store the values back.
        if (allPaths.isEmpty()) {
            props.unsetValue(propertyKey);
        } else {
            props.setValues(propertyKey, ArrayUtil.toStringArray(allPaths));
        }
    }

    /**
     * Adds the current path and other known paths to the combo, most recently used first.
     */
    public static void addKnownSDKPathsToCombo(@NotNull JComboBox combo) {
        final Set<String> pathsToShow = new LinkedHashSet<>();

        final String currentPath = combo.getEditor().getItem().toString().trim();
        if (!currentPath.isEmpty()) {
            pathsToShow.add(currentPath);
        }

        final String[] knownPaths = getKnownFlutterSdkPaths();
        for (String path : knownPaths) {
//        if (FlutterSdk.forPath(path) != null) {
//        pathsToShow.add(FileUtil.toSystemDependentName(path));
//        }
        }

        //noinspection unchecked
        combo.setModel(new DefaultComboBoxModel(ArrayUtil.toStringArray(pathsToShow)));

        if (combo.getSelectedIndex() == -1 && combo.getItemCount() > 0) {
            combo.setSelectedIndex(0);
        }
    }

    @NotNull
    public static String[] getKnownFlutterSdkPaths() {
        final Set<String> paths = new HashSet<>();

        System.out.println("==> Sanitize me ........");
//         scan current projects for existing flutter sdk settings
        for (Project project : ProjectManager.getInstance().getOpenProjects()) {
            final StratosSdk flutterSdk = StratosSdk.getStratosSdk(project);
            if (flutterSdk != null) {
                paths.add(flutterSdk.getHomePath());
            }
        }

        // use the list of paths they've entered in the past
        final String[] knownPaths = PropertiesComponent.getInstance().getValues(STRATOS_KNOWN_PATHS);
        if (knownPaths != null) {
            paths.addAll(Arrays.asList(knownPaths));
        }

        // search the user's path
        final String fromUserPath = locateSdkFromPath();
        if (fromUserPath != null) {
            paths.add(fromUserPath);
        }

        return paths.toArray(new String[0]);
    }

    @NotNull
    public static String pathToFlutterTool(@NotNull String sdkPath) throws ExecutionException {
        final String path = findDescendant(sdkPath, "/bin/" + flutterScriptName());
        if (path == null) {
            throw new ExecutionException("Stratos SDK is not configured");
        }
        return path;
    }

    @NotNull
    public static String flutterScriptName() {
        return SystemInfo.isWindows ? "stratos.bat" : "flutter";
    }

    /**
     * Returns the path to the Stratos SDK , or null if it doesn't exist.
     */
    @Nullable
    public static String pathToStratosSdk(@NotNull String flutterSdkPath) {
        return findDescendant(flutterSdkPath, "/bin/stratos");
    }

    @Nullable
    private static String findDescendant(@NotNull String flutterSdkPath, @NotNull String path) {
        final VirtualFile file = LocalFileSystem.getInstance().refreshAndFindFileByPath(flutterSdkPath + path);
        if (file == null || !file.exists()) {
            return null;
        }
        return file.getPath();
    }

    public static boolean isStratosSdkHome(@NotNull final String path) {
        final File stratosConfig = new File(path + "/src/app.config");
        final File stratosBin = new File(path + "/bin/stratos");
        return stratosConfig.isFile() && stratosBin.isFile();
    }


    /**
     * Checks the workspace for any open Flutter projects.
     *
     * @return true if an open Flutter project is found
     */
    public static boolean hasFlutterModules() {
        return Arrays.stream(ProjectManager.getInstance().getOpenProjects()).anyMatch(StratosModuleUtils::hasFlutterModule);
    }

    public static boolean hasFlutterModules(@NotNull Project project) {
        return StratosModuleUtils.hasFlutterModule(project);
    }

    @Nullable
    public static String getErrorMessageIfWrongSdkRootPath(final @NotNull String sdkRootPath) {
        if (sdkRootPath.isEmpty()) {
            return null;
        }

        final File sdkRoot = new File(sdkRootPath);
        if (!sdkRoot.isDirectory())
            return "sdk no exist"; // StratosBundle.message("error.folder.specified.as.sdk.not.exists");

        if (!isStratosSdkHome(sdkRootPath))
            return "sdk not found in location"; //return StratosBundle.message("error.sdk.not.found.in.specified.location");

        return null;
    }

    public static void setFlutterSdkPath(@NotNull final Project project, @NotNull final String flutterSdkPath) {

        // Update the list of known sdk paths.
        StratosSdkUtil.updateKnownSdkPaths(flutterSdkPath);

    }


    @VisibleForTesting
    public static String parseFlutterSdkPath(String packagesFileContent) {
        for (String line : packagesFileContent.split("\n")) {
            // flutter:file:///Users/.../flutter/packages/flutter/lib/
            line = line.trim();

            if (line.isEmpty() || line.startsWith("#")) {
                continue;
            }

            final String flutterPrefix = "flutter:";
            if (line.startsWith(flutterPrefix)) {
                final String urlString = line.substring(flutterPrefix.length());

                if (urlString.startsWith("file:")) {
                    final Url url = Urls.parseEncoded(urlString);
                    if (url == null) {
                        continue;
                    }
                    final String path = url.getPath();
                    // go up three levels
                    final File file = new File(url.getPath());
                    return file.getParentFile().getParentFile().getParentFile().getPath();
                }
            }
        }

        return null;
    }

    /**
     * Locate the Stratos SDK using the user's PATH.
     */
    @Nullable
    public static String locateSdkFromPath() {
        final String stratosBinPath = SystemUtils.which("stratos");
        if (stratosBinPath == null) {
            return null;
        }

        final File flutterBinFile = new File(stratosBinPath);
        return flutterBinFile.getParentFile().getParentFile().getPath();
    }
}
