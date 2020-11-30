package org.stratos.lang.listener

import com.intellij.openapi.project.Project
import org.stratos.lang.bundle.StratosBundle


class StratosProjectService(project: Project) {

    init {
        println(StratosBundle.message("projectService", project.name))
    }

}
