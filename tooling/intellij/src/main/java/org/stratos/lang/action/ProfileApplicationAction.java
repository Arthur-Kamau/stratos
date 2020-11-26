package org.stratos.lang.action;

import com.intellij.openapi.actionSystem.AnActionEvent;
import com.intellij.openapi.project.DumbAwareAction;
import org.jetbrains.annotations.NotNull;

public class ProfileApplicationAction extends DumbAwareAction {
    @Override
    public void actionPerformed(@NotNull final AnActionEvent e) {

        System.out.println("=========>> Mhenga check this");
    }
}

