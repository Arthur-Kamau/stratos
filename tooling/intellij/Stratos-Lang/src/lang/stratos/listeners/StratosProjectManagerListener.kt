package lang.stratos.listeners

import com.intellij.openapi.project.Project
import com.intellij.openapi.project.ProjectManagerListener
import lang.stratos.services.StratosProjectService

internal class StratosProjectManagerListener : ProjectManagerListener {

    override fun projectOpened(project: Project) {
        project.getService(StratosProjectService::class.java)
    }
}
