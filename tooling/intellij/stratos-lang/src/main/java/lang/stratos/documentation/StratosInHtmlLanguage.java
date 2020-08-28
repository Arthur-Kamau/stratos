package lang.stratos.documentation;


import com.intellij.lang.DependentLanguage;
import com.intellij.lang.Language;
import lang.stratos.language.StratosLanguage;

public class StratosInHtmlLanguage extends Language implements DependentLanguage {
    public static StratosInHtmlLanguage INSTANCE = new StratosInHtmlLanguage();

    protected StratosInHtmlLanguage() {
        super(StratosLanguage.INSTANCE, "Stratos in Html");
    }
}
