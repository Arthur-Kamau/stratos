package com.araizen.stratos
import com.intellij.openapi.actionSystem.AnAction
import com.intellij.openapi.actionSystem.AnActionEvent


class RunAction : AnAction(){

    override fun actionPerformed (e : AnActionEvent){
        print("run action ")
    }
}