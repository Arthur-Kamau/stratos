package org.stratos.lang.listener

import com.intellij.openapi.project.Project
import com.intellij.openapi.project.ProjectManagerListener

internal class StratosProjectManagerListener : ProjectManagerListener {

    override fun projectOpened(project: Project) {
        project.getService(StratosProjectService::class.java)
    }
}
