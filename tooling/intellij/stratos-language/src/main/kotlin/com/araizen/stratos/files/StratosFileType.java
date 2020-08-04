package com.araizen.stratos.files;

import com.intellij.openapi.fileTypes.LanguageFileType;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;

public class StratosFileType extends LanguageFileType {
    public static final StratosFileType INSTANCE = new StratosFileType();

    private StratosFileType() {
        super(StratosLanguage.INSTANCE);
    }

    @NotNull
    @Override
    public String getName() {
        return "Stratos File";
    }

    @NotNull
    @Override
    public String getDescription() {
        return "Stratos language file";
    }

    @NotNull
    @Override
    public String getDefaultExtension() {
        return "st";
    }

    @Nullable
    @Override
    public Icon getIcon() {
        return StratosFileIcons.FILE;
    }
}
