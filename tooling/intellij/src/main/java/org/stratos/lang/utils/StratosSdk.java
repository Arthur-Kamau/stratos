package org.stratos.lang.utils;

import com.intellij.openapi.diagnostic.Logger;
import com.intellij.openapi.fileEditor.FileDocumentManager;
import com.intellij.openapi.module.Module;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.roots.OrderRootType;
import com.intellij.openapi.roots.libraries.Library;
import com.intellij.openapi.roots.libraries.LibraryTable;
import com.intellij.openapi.roots.libraries.LibraryTablesRegistrar;
import com.intellij.openapi.util.Key;
import com.intellij.openapi.util.io.FileUtil;
import com.intellij.openapi.vfs.LocalFileSystem;
import com.intellij.openapi.vfs.VfsUtil;
import com.intellij.openapi.vfs.VirtualFile;
import com.intellij.openapi.vfs.VirtualFileManager;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
//import org.stratos.lang.module.sdk.StratosCreateAdditionalSettings;

import java.util.*;

import static java.util.Arrays.asList;

public class StratosSdk {
    public static final String FLUTTER_SDK_GLOBAL_LIB_NAME = "Stratos SDK";

    public static final String DART_SDK_SUFFIX = "/bin/cache/dart-sdk";
    public static final String LINUX_DART_SUFFIX = "/google-dartlang";
    public static final String MAC_DART_SUFFIX = "/dart_lang/macos_sdk";

    private static final String DART_CORE_SUFFIX = DART_SDK_SUFFIX + "/lib/core";

    private static final Logger LOG = Logger.getInstance(StratosSdk.class);

    private static final Map<String, StratosSdk> projectSdkCache = new HashMap<>();

    private final @NotNull VirtualFile myHome;
    private final @NotNull StratosSdkVersion myVersion;
    private final Map<String, String> cachedConfigValues = new HashMap<>();

    private StratosSdk(@NotNull final VirtualFile home, @NotNull final StratosSdkVersion version) {
        myHome = home;
        myVersion = version;
    }


    /**
     * Return the StratosSdk for the given project.
     * <p>
     * Returns null if the Dart SDK is not set or does not exist.
     */
    @Nullable
    public static StratosSdk getFlutterSdk(@NotNull final Project project) {
        if (project.isDisposed()) {
            return null;
        }

//        final DartSdk dartSdk = DartPlugin.getDartSdk(project);
//        if (dartSdk == null) {
//            return null;
//        }

//        final String dartPath = dartSdk.getHomePath();
//        if (!dartPath.endsWith(DART_SDK_SUFFIX)) {
//            return null;
//        }
//
//        final String sdkPath = dartPath.substring(0, dartPath.length() - DART_SDK_SUFFIX.length());
//
//        // Cache based on the project and path ('e41cfa3d:/Users/devoncarew/projects/flutter/flutter').
//        final String cacheKey = project.getLocationHash() + ":" + sdkPath;
//
//        synchronized (projectSdkCache) {
//            if (!projectSdkCache.containsKey(cacheKey)) {
//                projectSdkCache.put(cacheKey, StratosSdk.forPath(sdkPath));
//            }
//
//            return projectSdkCache.get(cacheKey);
//        }


        return null ;
    }

    /**
     * Returns the Flutter SDK for a project that has a possibly broken "Dart SDK" project library.
     * <p>
     * (This can happen for a newly-cloned Flutter SDK where the Dart SDK is not cached yet.)
     */
    @Nullable
    public static StratosSdk getIncomplete(@NotNull final Project project) {
        if (project.isDisposed()) {
            return null;
        }
        final Library lib = getDartSdkLibrary(project);
        if (lib == null) {
            return null;
        }
        return getFlutterFromDartSdkLibrary(lib);
    }

    @Nullable
    public static StratosSdk forPath(@NotNull final String path) {
        final VirtualFile home = LocalFileSystem.getInstance().findFileByPath(path);
        if (home == null || !StratosSdkUtil.isFlutterSdkHome(path)) {
            return null;
        }
        else {
            return new StratosSdk(home, StratosSdkVersion.readFromSdk(home));
        }
    }

    @Nullable
    private static Library getDartSdkLibrary(@NotNull Project project) {
        final LibraryTable libraryTable = LibraryTablesRegistrar.getInstance().getLibraryTable(project);
        for (Library lib : libraryTable.getLibraries()) {
            if ("Dart SDK".equals(lib.getName())) {
                return lib;
            }
        }
        return null;
    }

    @Nullable
    private static StratosSdk getFlutterFromDartSdkLibrary(Library lib) {
        final String[] urls = lib.getUrls(OrderRootType.CLASSES);
        for (String url : urls) {
            if (url.endsWith(DART_CORE_SUFFIX)) {
                final String flutterUrl = url.substring(0, url.length() - DART_CORE_SUFFIX.length());
                final VirtualFile home = VirtualFileManager.getInstance().findFileByUrl(flutterUrl);
                return home == null ? null : new StratosSdk(home, StratosSdkVersion.readFromSdk(home));
            }
        }
        return null;
    }



    @NotNull
    public VirtualFile getHome() {
        return myHome;
    }

    @NotNull
    public String getHomePath() {
        return myHome.getPath();
    }

    /**
     * Returns the Flutter Version as captured in the 'version' file. This version is very coarse grained and not meant for presentation and
     * rather only for sanity-checking the presence of baseline features (e.g, hot-reload).
     */
    @NotNull
    public StratosSdkVersion getVersion() {
        return myVersion;
    }

    /**
     * Returns the path to the Dart SDK cached within the Flutter SDK, or null if it doesn't exist.
     */
    @Nullable
    public String getDartSdkPath() {
        return StratosSdkUtil.pathToDartSdk(getHomePath());
    }


}

