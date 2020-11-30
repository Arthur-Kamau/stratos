package org.stratos.lang.toolWindow;


import com.esotericsoftware.minlog.Log;
import com.intellij.openapi.project.DumbAware;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.wm.ToolWindow;
import com.intellij.openapi.wm.ToolWindowFactory;
import com.intellij.ui.content.Content;
import com.intellij.ui.content.ContentFactory;
import org.jetbrains.annotations.NotNull;

public class RunToolWindowWindowFactory implements ToolWindowFactory, DumbAware {

    @Override
    public void init(ToolWindow window) {
        window.setAvailable(true, null);
    }

    @Override
    public boolean isApplicable(@NotNull Project project) {

        return true;
    }

    /**
     * Create the tool window content.
     *
     * @param project    current project
     * @param toolWindow current tool window
     */
    @Override
    public void createToolWindowContent(@NotNull Project project, @NotNull ToolWindow toolWindow) {
        Log.error("HEy .........     ");

        StratosToolWindow myToolWindow = new StratosToolWindow(toolWindow);
        ContentFactory contentFactory = ContentFactory.SERVICE.getInstance();
        Content content = contentFactory.createContent(myToolWindow.getContent(), "Run", false);
        toolWindow.getContentManager().addContent(content);
    }

}
