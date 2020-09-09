package lang.stratos.services

import com.intellij.openapi.project.Project
import lang.stratos.bundle.StratosBundle

class StratosProjectService(project: Project) {

    init {
        println(StratosBundle.message("projectService", project.name))
    }
}
