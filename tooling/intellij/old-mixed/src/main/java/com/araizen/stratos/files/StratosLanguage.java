package com.araizen.stratos.files;

import com.intellij.lang.Language;
import org.jetbrains.annotations.NotNull;

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

public class StratosLanguage extends Language {


    public static final StratosLanguage INSTANCE = new StratosLanguage();

//    private StratosLanguage() {
//        super("stratos");
//    }

    private StratosLanguage() {
        super("Stratos", "text/x-stratos-source", "text/stratos", "application/x-stratos", "text/x-stratos");
    }

    /**
     * Returns true if value is a valid identifier (as specified
     * in stratos Language Specification, section 3.8).
     *
     * @param value string to test (can be null or empty)
     * @return true if value is a valid stratos identifier.
     */
    public static boolean isIdentifier(String value) {

        return isIdentifierChars(value)
                && ! isKeyword(value)
                && ! isBooleanLiteral(value)
                && ! isNullLiteral(value);
    }


    /**
     * Returns true if value is a valid class name (fully qualified or not).
     *
     * @param value string to test (can be null or empty)
     * @return  true if value is a valid class name
     */
    public static boolean isClassName(String value) {
        boolean result;

        if (value == null) {
            result = false;
        } else {
            result = true;
            for (String item : value.split("\\.")) { //NOI18N
                if (isIdentifier(item) == false) {
                    result = false;
                    break;
                }
            }
        }

        return result;
    }

    /*
     * Private
     */

    private static boolean isIdentifierChars(String value) {
        if (value == null || value.isEmpty()
                || !Character.isJavaIdentifierStart(value.codePointAt(0))) {
            return false;
        }
        for (int i = 0; i < value.length();) {
            int codePoint = value.codePointAt(i);
            if (!Character.isJavaIdentifierPart(codePoint)) {
                return false;
            }
            i += Character.charCount(codePoint);
        }
        return true;
    }

    private static Set<String> keywords;
    private static synchronized boolean isKeyword(String value) {
        if (keywords == null) {
            keywords = new HashSet<>();
            Collections.addAll(
                    keywords,
                    "abstract", "continue", "for", "new", "switch", //NOI18N
                    "assert", "default", "if", "package", "synchronized", //NOI18N
                    "boolean", "do", "goto", "private", "this", //NOI18N
                    "break", "double", "implements", "protected", "throw", //NOI18N
                    "byte", "else", "import", "public", "throws", //NOI18N
                    "case", "enum", "instanceof", "return", "transient", //NOI18N
                    "catch", "extends", "int", "short", "try", //NOI18N
                    "char", "final", "interface", "static", "void", //NOI18N
                    "class", "finally", "long", "strictfp", "volatile",  //NOI18N
                    "const", "float", "native", "super", "while"); //NOI18N
        }
        return keywords.contains(value);
    }


    @NotNull
    @Override
    public String getDisplayName() {
        return "Stratos";
    }

    @Override
    public boolean isCaseSensitive() {
        return true;
    }

    private static boolean isBooleanLiteral(String value) {
        return value.equals("true") || value.equals("false"); //NOI18N
    }

    private static boolean isNullLiteral(String value) {
        return value.equals("null"); //NOI18N
    }
}
