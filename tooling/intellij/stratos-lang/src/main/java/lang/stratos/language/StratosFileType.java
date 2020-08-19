package lang.stratos.language;

import com.intellij.openapi.fileTypes.LanguageFileType;
import lang.stratos.icons.StratosIcons;
import lang.stratos.util.StratosBundle;
import org.jetbrains.annotations.NotNull;

import javax.swing.*;
public class StratosFileType extends LanguageFileType {
    public static final LanguageFileType INSTANCE = new StratosFileType();
    public static final String DEFAULT_EXTENSION = "st";

    private StratosFileType() {
        super(StratosLanguage.INSTANCE);
    }

    @NotNull
    @Override
    public String getName() {
        return "Dart";
    }

    @NotNull
    @Override
    public String getDescription() {
        return StratosBundle.message("file.type.description.stratos");
    }

    @NotNull
    @Override
    public String getDefaultExtension() {
        return DEFAULT_EXTENSION;
    }

    @Override
    public Icon getIcon() {
        return StratosIcons.StratosIcon;
    }
}
