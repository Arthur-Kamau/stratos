package lang.stratos.language;

import com.intellij.openapi.editor.colors.TextAttributesKey;

import com.intellij.openapi.editor.DefaultLanguageHighlighterColors;
import com.intellij.openapi.editor.HighlighterColors;
import com.intellij.openapi.editor.colors.CodeInsightColors;

import static com.intellij.openapi.editor.colors.TextAttributesKey.createTextAttributesKey;

public class StratosSyntaxHighlighterColors {
    public static final String STRATOS_ERROR = "STRATOS_ERROR";
    public static final String STRATOS_WARNING = "STRATOS_WARNING";
    public static final String STRATOS_HINT = "STRATOS_HINT";

    public static final String STRATOS_ANNOTATION = "STRATOS_ANNOTATION";
    public static final String STRATOS_CLASS = "STRATOS_CLASS";
    public static final String STRATOS_CONSTRUCTOR = "STRATOS_CONSTRUCTOR";

    public static final String STRATOS_DYNAMIC_LOCAL_VARIABLE_DECLARATION = "STRATOS_DYNAMIC_LOCAL_VARIABLE_DECLARATION";
    public static final String STRATOS_DYNAMIC_LOCAL_VARIABLE_REFERENCE = "STRATOS_DYNAMIC_LOCAL_VARIABLE_REFERENCE";
    public static final String STRATOS_DYNAMIC_PARAMETER_DECLARATION = "STRATOS_DYNAMIC_PARAMETER_DECLARATION";
    public static final String STRATOS_DYNAMIC_PARAMETER_REFERENCE = "STRATOS_DYNAMIC_PARAMETER_REFERENCE";

    public static final String STRATOS_ENUM = "STRATOS_ENUM";
    public static final String STRATOS_ENUM_CONSTANT = "STRATOS_ENUM_CONSTANT";
    public static final String STRATOS_FUNCTION_TYPE_ALIAS = "STRATOS_FUNCTION_TYPE_ALIAS";

    public static final String STRATOS_IDENTIFIER = "STRATOS_IDENTIFIER";
    public static final String STRATOS_INSTANCE_FIELD_DECLARATION = "STRATOS_INSTANCE_FIELD_DECLARATION";
    public static final String STRATOS_INSTANCE_FIELD_REFERENCE = "STRATOS_INSTANCE_FIELD_REFERENCE";
    public static final String STRATOS_INSTANCE_GETTER_DECLARATION = "STRATOS_INSTANCE_GETTER_DECLARATION";
    public static final String STRATOS_INSTANCE_GETTER_REFERENCE = "STRATOS_INSTANCE_GETTER_REFERENCE";
    public static final String STRATOS_INSTANCE_METHOD_DECLARATION = "STRATOS_INSTANCE_METHOD_DECLARATION";
    public static final String STRATOS_INSTANCE_METHOD_REFERENCE = "STRATOS_INSTANCE_METHOD_REFERENCE";
    public static final String STRATOS_INSTANCE_SETTER_DECLARATION = "STRATOS_INSTANCE_SETTER_DECLARATION";
    public static final String STRATOS_INSTANCE_SETTER_REFERENCE = "STRATOS_INSTANCE_SETTER_REFERENCE";

    public static final String STRATOS_IMPORT_PREFIX = "STRATOS_IMPORT_PREFIX";
    public static final String STRATOS_KEYWORD = "STRATOS_KEYWORD";
    public static final String STRATOS_LABEL = "STRATOS_LABEL";
    public static final String STRATOS_LIBRARY_NAME = "STRATOS_LIBRARY_NAME";

    public static final String STRATOS_LOCAL_FUNCTION_DECLARATION = "STRATOS_LOCAL_FUNCTION_DECLARATION";
    public static final String STRATOS_LOCAL_FUNCTION_REFERENCE = "STRATOS_LOCAL_FUNCTION_REFERENCE";
    public static final String STRATOS_LOCAL_VARIABLE_DECLARATION = "STRATOS_LOCAL_VARIABLE_DECLARATION";
    public static final String STRATOS_LOCAL_VARIABLE_REFERENCE = "STRATOS_LOCAL_VARIABLE_REFERENCE";

    public static final String STRATOS_PARAMETER_DECLARATION = "STRATOS_PARAMETER_DECLARATION";
    public static final String STRATOS_PARAMETER_REFERENCE = "STRATOS_PARAMETER_REFERENCE";

    public static final String STRATOS_STATIC_FIELD_DECLARATION = "STRATOS_STATIC_FIELD_DECLARATION";
    public static final String STRATOS_STATIC_GETTER_DECLARATION = "STRATOS_STATIC_GETTER_DECLARATION";
    public static final String STRATOS_STATIC_GETTER_REFERENCE = "STRATOS_STATIC_GETTER_REFERENCE";
    public static final String STRATOS_STATIC_METHOD_DECLARATION = "STRATOS_STATIC_METHOD_DECLARATION";
    public static final String STRATOS_STATIC_METHOD_REFERENCE = "STRATOS_STATIC_METHOD_REFERENCE";
    public static final String STRATOS_STATIC_SETTER_DECLARATION = "STRATOS_STATIC_SETTER_DECLARATION";
    public static final String STRATOS_STATIC_SETTER_REFERENCE = "STRATOS_STATIC_SETTER_REFERENCE";

    public static final String STRATOS_TOP_LEVEL_FUNCTION_DECLARATION = "STRATOS_TOP_LEVEL_FUNCTION_DECLARATION";
    public static final String STRATOS_TOP_LEVEL_FUNCTION_REFERENCE = "STRATOS_TOP_LEVEL_FUNCTION_REFERENCE";
    public static final String STRATOS_TOP_LEVEL_GETTER_DECLARATION = "STRATOS_TOP_LEVEL_GETTER_DECLARATION";
    public static final String STRATOS_TOP_LEVEL_GETTER_REFERENCE = "STRATOS_TOP_LEVEL_GETTER_REFERENCE";
    public static final String STRATOS_TOP_LEVEL_SETTER_DECLARATION = "STRATOS_TOP_LEVEL_SETTER_DECLARATION";
    public static final String STRATOS_TOP_LEVEL_SETTER_REFERENCE = "STRATOS_TOP_LEVEL_SETTER_REFERENCE";
    public static final String STRATOS_TOP_LEVEL_VARIABLE_DECLARATION = "STRATOS_TOP_LEVEL_VARIABLE_DECLARATION";

    public static final String STRATOS_TYPE_NAME_DYNAMIC = "STRATOS_TYPE_NAME_DYNAMIC";
    public static final String STRATOS_TYPE_PARAMETER = "STRATOS_TYPE_PARAMETER";
    public static final String STRATOS_UNRESOLVED_INSTANCE_MEMBER_REFERENCE = "STRATOS_UNRESOLVED_INSTANCE_MEMBER_REFERENCE";

    private static final String STRATOS_BLOCK_COMMENT = "STRATOS_BLOCK_COMMENT";
    private static final String STRATOS_DOC_COMMENT = "STRATOS_DOC_COMMENT";
    private static final String STRATOS_LINE_COMMENT = "STRATOS_LINE_COMMENT";

    private static final String STRATOS_NUMBER = "STRATOS_NUMBER";
    private static final String STRATOS_STRING = "STRATOS_STRING";
    private static final String STRATOS_VALID_STRING_ESCAPE = "STRATOS_VALID_STRING_ESCAPE";
    private static final String STRATOS_INVALID_STRING_ESCAPE = "STRATOS_INVALID_STRING_ESCAPE";
    private static final String STRATOS_OPERATION_SIGN = "STRATOS_OPERATION_SIGN";
    private static final String STRATOS_PARENTH = "STRATOS_PARENTH";
    private static final String STRATOS_BRACKETS = "STRATOS_BRACKETS";
    private static final String STRATOS_BRACES = "STRATOS_BRACES";
    private static final String STRATOS_COMMA = "STRATOS_COMMA";
    private static final String STRATOS_DOT = "STRATOS_DOT";
    private static final String STRATOS_SEMICOLON = "STRATOS_SEMICOLON";
    private static final String STRATOS_COLON = "STRATOS_COLON";
    private static final String STRATOS_FAT_ARROW = "STRATOS_FAT_ARROW";
    private static final String STRATOS_BAD_CHARACTER = "STRATOS_BAD_CHARACTER";
    private static final String STRATOS_SYMBOL_LITERAL = "STRATOS_SYMBOL_LITERAL";

    public static final TextAttributesKey ERROR =
            createTextAttributesKey(STRATOS_ERROR, CodeInsightColors.ERRORS_ATTRIBUTES);
    public static final TextAttributesKey WARNING =
            createTextAttributesKey(STRATOS_WARNING, CodeInsightColors.WARNINGS_ATTRIBUTES);
    public static final TextAttributesKey HINT =
            createTextAttributesKey(STRATOS_HINT, CodeInsightColors.WEAK_WARNING_ATTRIBUTES);

    public static final TextAttributesKey BLOCK_COMMENT =
            createTextAttributesKey(STRATOS_BLOCK_COMMENT, DefaultLanguageHighlighterColors.BLOCK_COMMENT);
    public static final TextAttributesKey DOC_COMMENT =
            createTextAttributesKey(STRATOS_DOC_COMMENT, DefaultLanguageHighlighterColors.DOC_COMMENT);
    public static final TextAttributesKey LINE_COMMENT =
            createTextAttributesKey(STRATOS_LINE_COMMENT, DefaultLanguageHighlighterColors.LINE_COMMENT);

    public static final TextAttributesKey NUMBER = createTextAttributesKey(STRATOS_NUMBER, DefaultLanguageHighlighterColors.NUMBER);
    public static final TextAttributesKey STRING = createTextAttributesKey(STRATOS_STRING, DefaultLanguageHighlighterColors.STRING);
    public static final TextAttributesKey VALID_STRING_ESCAPE =
            createTextAttributesKey(STRATOS_VALID_STRING_ESCAPE, DefaultLanguageHighlighterColors.VALID_STRING_ESCAPE);
    public static final TextAttributesKey INVALID_STRING_ESCAPE =
            createTextAttributesKey(STRATOS_INVALID_STRING_ESCAPE, DefaultLanguageHighlighterColors.INVALID_STRING_ESCAPE);
    public static final TextAttributesKey OPERATION_SIGN =
            createTextAttributesKey(STRATOS_OPERATION_SIGN, DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey PARENTHS = createTextAttributesKey(STRATOS_PARENTH, DefaultLanguageHighlighterColors.PARENTHESES);
    public static final TextAttributesKey BRACKETS = createTextAttributesKey(STRATOS_BRACKETS, DefaultLanguageHighlighterColors.BRACKETS);
    public static final TextAttributesKey BRACES = createTextAttributesKey(STRATOS_BRACES, DefaultLanguageHighlighterColors.BRACES);
    public static final TextAttributesKey COMMA = createTextAttributesKey(STRATOS_COMMA, DefaultLanguageHighlighterColors.COMMA);
    public static final TextAttributesKey DOT = createTextAttributesKey(STRATOS_DOT, DefaultLanguageHighlighterColors.DOT);
    public static final TextAttributesKey SEMICOLON = createTextAttributesKey(STRATOS_SEMICOLON, DefaultLanguageHighlighterColors.SEMICOLON);
    public static final TextAttributesKey COLON = createTextAttributesKey(STRATOS_COLON, DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey FAT_ARROW =
            createTextAttributesKey(STRATOS_FAT_ARROW, DefaultLanguageHighlighterColors.OPERATION_SIGN);
    public static final TextAttributesKey BAD_CHARACTER = createTextAttributesKey(STRATOS_BAD_CHARACTER, HighlighterColors.BAD_CHARACTER);
    public static final TextAttributesKey SYMBOL_LITERAL =
            createTextAttributesKey(STRATOS_SYMBOL_LITERAL, DefaultLanguageHighlighterColors.KEYWORD);

    public static final TextAttributesKey ANNOTATION = createTextAttributesKey(STRATOS_ANNOTATION, DefaultLanguageHighlighterColors.METADATA);
    public static final TextAttributesKey CLASS = createTextAttributesKey(STRATOS_CLASS, DefaultLanguageHighlighterColors.CLASS_NAME);
    public static final TextAttributesKey ENUM = createTextAttributesKey(STRATOS_ENUM, DefaultLanguageHighlighterColors.CLASS_NAME);
    public static final TextAttributesKey ENUM_CONSTANT =
            createTextAttributesKey(STRATOS_ENUM_CONSTANT, DefaultLanguageHighlighterColors.INSTANCE_FIELD);
    public static final TextAttributesKey FUNCTION_TYPE_ALIAS =
            createTextAttributesKey(STRATOS_FUNCTION_TYPE_ALIAS, DefaultLanguageHighlighterColors.CLASS_NAME);
    public static final TextAttributesKey IDENTIFIER =
            createTextAttributesKey(STRATOS_IDENTIFIER, DefaultLanguageHighlighterColors.IDENTIFIER);

    public static final TextAttributesKey INSTANCE_FIELD_DECLARATION =
            createTextAttributesKey(STRATOS_INSTANCE_FIELD_DECLARATION, DefaultLanguageHighlighterColors.INSTANCE_FIELD);
    public static final TextAttributesKey INSTANCE_FIELD_REFERENCE =
            createTextAttributesKey(STRATOS_INSTANCE_FIELD_REFERENCE, DefaultLanguageHighlighterColors.INSTANCE_FIELD);
    public static final TextAttributesKey INSTANCE_GETTER_DECLARATION =
            createTextAttributesKey(STRATOS_INSTANCE_GETTER_DECLARATION, INSTANCE_FIELD_DECLARATION);
    public static final TextAttributesKey INSTANCE_GETTER_REFERENCE =
            createTextAttributesKey(STRATOS_INSTANCE_GETTER_REFERENCE, INSTANCE_FIELD_DECLARATION);
    public static final TextAttributesKey INSTANCE_METHOD_DECLARATION =
            createTextAttributesKey(STRATOS_INSTANCE_METHOD_DECLARATION, DefaultLanguageHighlighterColors.INSTANCE_METHOD);
    public static final TextAttributesKey INSTANCE_METHOD_REFERENCE =
            createTextAttributesKey(STRATOS_INSTANCE_METHOD_REFERENCE, DefaultLanguageHighlighterColors.FUNCTION_CALL);
    public static final TextAttributesKey INSTANCE_SETTER_DECLARATION =
            createTextAttributesKey(STRATOS_INSTANCE_SETTER_DECLARATION, INSTANCE_FIELD_DECLARATION);
    public static final TextAttributesKey INSTANCE_SETTER_REFERENCE =
            createTextAttributesKey(STRATOS_INSTANCE_SETTER_REFERENCE, INSTANCE_FIELD_DECLARATION);

    public static final TextAttributesKey IMPORT_PREFIX =
            createTextAttributesKey(STRATOS_IMPORT_PREFIX, DefaultLanguageHighlighterColors.IDENTIFIER);
    public static final TextAttributesKey KEYWORD = createTextAttributesKey(STRATOS_KEYWORD, DefaultLanguageHighlighterColors.KEYWORD);
    public static final TextAttributesKey LABEL = createTextAttributesKey(STRATOS_LABEL, DefaultLanguageHighlighterColors.LABEL);
    public static final TextAttributesKey LIBRARY_NAME =
            createTextAttributesKey(STRATOS_LIBRARY_NAME, DefaultLanguageHighlighterColors.IDENTIFIER);
    public static final TextAttributesKey LOCAL_VARIABLE_DECLARATION =
            createTextAttributesKey(STRATOS_LOCAL_VARIABLE_DECLARATION, DefaultLanguageHighlighterColors.LOCAL_VARIABLE);
    public static final TextAttributesKey LOCAL_VARIABLE_REFERENCE =
            createTextAttributesKey(STRATOS_LOCAL_VARIABLE_REFERENCE, DefaultLanguageHighlighterColors.LOCAL_VARIABLE);
    public static final TextAttributesKey PARAMETER_DECLARATION =
            createTextAttributesKey(STRATOS_PARAMETER_DECLARATION, DefaultLanguageHighlighterColors.PARAMETER);
    public static final TextAttributesKey PARAMETER_REFERENCE =
            createTextAttributesKey(STRATOS_PARAMETER_REFERENCE, DefaultLanguageHighlighterColors.PARAMETER);

    public static final TextAttributesKey STATIC_FIELD_DECLARATION =
            createTextAttributesKey(STRATOS_STATIC_FIELD_DECLARATION, DefaultLanguageHighlighterColors.STATIC_FIELD);
    public static final TextAttributesKey STATIC_GETTER_DECLARATION =
            createTextAttributesKey(STRATOS_STATIC_GETTER_DECLARATION, STATIC_FIELD_DECLARATION);
    public static final TextAttributesKey STATIC_GETTER_REFERENCE =
            createTextAttributesKey(STRATOS_STATIC_GETTER_REFERENCE, STATIC_FIELD_DECLARATION);
    public static final TextAttributesKey STATIC_METHOD_DECLARATION =
            createTextAttributesKey(STRATOS_STATIC_METHOD_DECLARATION, DefaultLanguageHighlighterColors.STATIC_METHOD);
    public static final TextAttributesKey STATIC_METHOD_REFERENCE =
            createTextAttributesKey(STRATOS_STATIC_METHOD_REFERENCE, DefaultLanguageHighlighterColors.STATIC_METHOD);
    public static final TextAttributesKey STATIC_SETTER_DECLARATION =
            createTextAttributesKey(STRATOS_STATIC_SETTER_DECLARATION, STATIC_FIELD_DECLARATION);
    public static final TextAttributesKey STATIC_SETTER_REFERENCE =
            createTextAttributesKey(STRATOS_STATIC_SETTER_REFERENCE, STATIC_FIELD_DECLARATION);

    public static final TextAttributesKey TOP_LEVEL_VARIABLE_DECLARATION =
            createTextAttributesKey(STRATOS_TOP_LEVEL_VARIABLE_DECLARATION, DefaultLanguageHighlighterColors.GLOBAL_VARIABLE);
    public static final TextAttributesKey TOP_LEVEL_FUNCTION_DECLARATION =
            createTextAttributesKey(STRATOS_TOP_LEVEL_FUNCTION_DECLARATION, DefaultLanguageHighlighterColors.FUNCTION_DECLARATION);
    public static final TextAttributesKey TOP_LEVEL_FUNCTION_REFERENCE =
            createTextAttributesKey(STRATOS_TOP_LEVEL_FUNCTION_REFERENCE, DefaultLanguageHighlighterColors.FUNCTION_CALL);
    public static final TextAttributesKey TOP_LEVEL_GETTER_DECLARATION =
            createTextAttributesKey(STRATOS_TOP_LEVEL_GETTER_DECLARATION, TOP_LEVEL_VARIABLE_DECLARATION);
    public static final TextAttributesKey TOP_LEVEL_GETTER_REFERENCE =
            createTextAttributesKey(STRATOS_TOP_LEVEL_GETTER_REFERENCE, TOP_LEVEL_VARIABLE_DECLARATION);
    public static final TextAttributesKey TOP_LEVEL_SETTER_DECLARATION =
            createTextAttributesKey(STRATOS_TOP_LEVEL_SETTER_DECLARATION, TOP_LEVEL_VARIABLE_DECLARATION);
    public static final TextAttributesKey TOP_LEVEL_SETTER_REFERENCE =
            createTextAttributesKey(STRATOS_TOP_LEVEL_SETTER_REFERENCE, TOP_LEVEL_VARIABLE_DECLARATION);

    public static final TextAttributesKey TYPE_NAME_DYNAMIC =
            createTextAttributesKey(STRATOS_TYPE_NAME_DYNAMIC, DefaultLanguageHighlighterColors.KEYWORD);
    public static final TextAttributesKey TYPE_PARAMETER =
            createTextAttributesKey(STRATOS_TYPE_PARAMETER, DefaultLanguageHighlighterColors.CLASS_NAME);
    public static final TextAttributesKey UNRESOLVED_INSTANCE_MEMBER_REFERENCE =
            createTextAttributesKey(STRATOS_UNRESOLVED_INSTANCE_MEMBER_REFERENCE, DefaultLanguageHighlighterColors.IDENTIFIER);

    public static final TextAttributesKey CONSTRUCTOR =
            createTextAttributesKey(STRATOS_CONSTRUCTOR, INSTANCE_METHOD_DECLARATION);

    public static final TextAttributesKey DYNAMIC_LOCAL_VARIABLE_DECLARATION =
            createTextAttributesKey(STRATOS_DYNAMIC_LOCAL_VARIABLE_DECLARATION, LOCAL_VARIABLE_DECLARATION);
    public static final TextAttributesKey DYNAMIC_LOCAL_VARIABLE_REFERENCE =
            createTextAttributesKey(STRATOS_DYNAMIC_LOCAL_VARIABLE_REFERENCE, LOCAL_VARIABLE_REFERENCE);
    public static final TextAttributesKey DYNAMIC_PARAMETER_DECLARATION =
            createTextAttributesKey(STRATOS_DYNAMIC_PARAMETER_DECLARATION, PARAMETER_DECLARATION);
    public static final TextAttributesKey DYNAMIC_PARAMETER_REFERENCE =
            createTextAttributesKey(STRATOS_DYNAMIC_PARAMETER_REFERENCE, PARAMETER_REFERENCE);

    public static final TextAttributesKey LOCAL_FUNCTION_DECLARATION =
            createTextAttributesKey(STRATOS_LOCAL_FUNCTION_DECLARATION, LOCAL_VARIABLE_DECLARATION);
    public static final TextAttributesKey LOCAL_FUNCTION_REFERENCE =
            createTextAttributesKey(STRATOS_LOCAL_FUNCTION_REFERENCE, LOCAL_VARIABLE_REFERENCE);

}
