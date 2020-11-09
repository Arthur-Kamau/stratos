package org.stratos.lang;

import com.intellij.lang.Language;

public class StratosLanguage extends Language {
    public static final StratosLanguage INSTANCE = new StratosLanguage();

    private StratosLanguage() {
        super("Stratos");
    }
}
