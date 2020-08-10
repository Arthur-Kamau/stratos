package com.araizen.stratos.files

import com.araizen.stratos.files.StratosFileType.Companion.INSTANCE
import com.intellij.openapi.fileTypes.LanguageFileType
import javax.swing.Icon

class StratosFileType private constructor() : LanguageFileType(StratosLanguage.INSTANCE) {
    override fun getName(): String {
        return "Stratos File"
    }

    override fun getDescription(): String {
        return "Stratos language file"
    }

    override fun getDefaultExtension(): String {
        return "st"
    }

    override fun getIcon(): Icon? {
        return StratosFileIcons.FILE
    }

    companion object {
        val INSTANCE = StratosFileType()
    }
}