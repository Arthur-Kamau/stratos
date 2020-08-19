package lang.stratos.language;

import com.intellij.lang.Language;
import com.intellij.openapi.fileTypes.LanguageFileType;
import org.jetbrains.annotations.Nullable;

public class StratosLanguage  extends Language {
    public static final Language INSTANCE = new StratosLanguage();

    public static final String DART_MIME_TYPE = "application/stratos";

    private StratosLanguage() {
        super("Stratos", DART_MIME_TYPE);
    }

    @Nullable
    @Override
    public LanguageFileType getAssociatedFileType() {
        return StratosFileType.INSTANCE;
    }
}
