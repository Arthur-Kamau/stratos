package com.araizen.stratos

import com.intellij.openapi.actionSystem.AnAction
import com.intellij.openapi.actionSystem.AnActionEvent


class AnalyseAction : AnAction() {

    override fun actionPerformed(e: AnActionEvent) {
        print("Aalyse Action")
    }
}