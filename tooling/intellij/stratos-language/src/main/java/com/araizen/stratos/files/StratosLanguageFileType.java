package com.araizen.stratos.files;

import com.intellij.openapi.fileTypes.LanguageFileType;
import org.jetbrains.annotations.NotNull;

import javax.swing.*;

/**
 * @author Konstantin Bulenkov
 */
public class StratosLanguageFileType extends LanguageFileType {

    public static final LanguageFileType INSTANCE = new StratosLanguageFileType();

    public StratosLanguageFileType() {
        super(StratosLanguage.INSTANCE);
    }

    @NotNull
    @Override
    public String getName() {
        return "Stratos";
    }

    @NotNull
    @Override
    public String getDescription() {
        return "Stratos";
    }

    @NotNull
    @Override
    public String getDefaultExtension() {
        return "st";
    }

    @Override
    public Icon getIcon() {
        return StratosFileIcons.STRATOSICON;
    }
}
