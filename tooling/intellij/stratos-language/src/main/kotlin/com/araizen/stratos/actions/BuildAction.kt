package com.araizen.stratos.actions

import com.intellij.openapi.actionSystem.AnAction
import com.intellij.openapi.actionSystem.AnActionEvent


class BuildAction : AnAction(){

    override fun actionPerformed (e : AnActionEvent){
print("build action ")
    }
}