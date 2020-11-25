package org.stratos.lang;

import com.intellij.icons.AllIcons;
import com.intellij.openapi.util.IconLoader;

import javax.swing.*;

public class StratosIcons {
	private static Icon load(String path) {
		return IconLoader.getIcon(path, StratosIcons.class);
	}
	public static final Icon SAMPLE_ICON = AllIcons.FileTypes.Text;
	public static final Icon FUNC_ICON = IconLoader.getIcon("/org/stratos/lang/f.png");

	public static final Icon AttachDebugger = load("/icons/attachDebugger.png");

	public static final Icon HotReload = load("/icons/hot-reload.png");
	public static final Icon HotRestart = load("/icons/hot-restart.png");

	public static final Icon HotReloadRun = load("/icons/reload_run.png");
	public static final Icon HotReloadDebug = load("/icons/reload_debug.png");

	public static final Icon DebugBanner = load("/icons/debugBanner.png");
}
