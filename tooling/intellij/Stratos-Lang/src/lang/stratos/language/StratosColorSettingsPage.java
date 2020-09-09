package lang.stratos.language;


import com.intellij.openapi.editor.colors.TextAttributesKey;
import com.intellij.openapi.fileTypes.SyntaxHighlighter;
import com.intellij.openapi.options.colors.AttributesDescriptor;
import com.intellij.openapi.options.colors.ColorDescriptor;
import com.intellij.openapi.options.colors.ColorSettingsPage;
import lang.stratos.bundle.StratosBundle;
import lang.stratos.icons.StratosIcons;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;
import java.util.Map;

import static lang.stratos.language.StratosSyntaxHighlighterColors.*;

public class StratosColorSettingsPage implements ColorSettingsPage {

    private static final AttributesDescriptor[] DESCRIPTORS = new AttributesDescriptor[]{
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.error"), ERROR),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.warning"), WARNING),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.hint"), HINT),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.block.comment"), BLOCK_COMMENT),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.doc.comment"), DOC_COMMENT),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.line.comment"), LINE_COMMENT),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.number"), NUMBER),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.string"), STRING),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.valid.string.escape"), VALID_STRING_ESCAPE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.invalid.string.escape"), INVALID_STRING_ESCAPE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.operator"), OPERATION_SIGN),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.parenths"), PARENTHS),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.brackets"), BRACKETS),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.braces"), BRACES),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.comma"), COMMA),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.dot"), DOT),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.semicolon"), SEMICOLON),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.colon"), COLON),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.fat.arrow"), FAT_ARROW),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.bad.character"), BAD_CHARACTER),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.symbol.literal"), SYMBOL_LITERAL),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.annotation"), ANNOTATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.class"), CLASS),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.constructor"), CONSTRUCTOR),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.dynamic.local.variable.declaration"),
                    DYNAMIC_LOCAL_VARIABLE_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.dynamic.local.variable.reference"),
                    DYNAMIC_LOCAL_VARIABLE_REFERENCE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.dynamic.parameter.declaration"),
                    DYNAMIC_PARAMETER_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.dynamic.parameter.reference"),
                    DYNAMIC_PARAMETER_REFERENCE),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.enum"), ENUM),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.enum.constant"), ENUM_CONSTANT),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.function.type.alias"), FUNCTION_TYPE_ALIAS),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.identifier"), IDENTIFIER),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.instance.field.declaration"),
                    INSTANCE_FIELD_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.instance.field.reference"), INSTANCE_FIELD_REFERENCE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.instance.getter.declaration"),
                    INSTANCE_GETTER_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.instance.getter.reference"), INSTANCE_GETTER_REFERENCE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.instance.method.declaration"),
                    INSTANCE_METHOD_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.instance.method.reference"), INSTANCE_METHOD_REFERENCE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.instance.setter.declaration"),
                    INSTANCE_SETTER_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.instance.setter.reference"), INSTANCE_SETTER_REFERENCE),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.import.prefix"), IMPORT_PREFIX),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.keyword"), KEYWORD),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.label"), LABEL),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.library.name"), LIBRARY_NAME),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.local.function.declaration"),
                    LOCAL_FUNCTION_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.local.function.reference"), LOCAL_FUNCTION_REFERENCE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.local.variable.declaration"),
                    LOCAL_VARIABLE_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.local.variable.reference"), LOCAL_VARIABLE_REFERENCE),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.parameter.declaration"), PARAMETER_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.parameter.reference"), PARAMETER_REFERENCE),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.static.field.declaration"), STATIC_FIELD_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.static.getter.declaration"), STATIC_GETTER_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.static.getter.reference"), STATIC_GETTER_REFERENCE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.static.method.declaration"), STATIC_METHOD_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.static.method.reference"), STATIC_METHOD_REFERENCE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.static.setter.declaration"), STATIC_SETTER_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.static.setter.reference"), STATIC_SETTER_REFERENCE),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.top.level.function.declaration"),
                    TOP_LEVEL_FUNCTION_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.top.level.function.reference"),
                    TOP_LEVEL_FUNCTION_REFERENCE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.top.level.getter.declaration"),
                    TOP_LEVEL_GETTER_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.top.level.getter.reference"),
                    TOP_LEVEL_GETTER_REFERENCE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.top.level.setter.declaration"),
                    TOP_LEVEL_SETTER_DECLARATION),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.top.level.setter.reference"),
                    TOP_LEVEL_SETTER_REFERENCE),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.top.level.variable.declaration"),
                    TOP_LEVEL_VARIABLE_DECLARATION),

            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.type.name.dynamic"), TYPE_NAME_DYNAMIC),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.type.parameter"), TYPE_PARAMETER),
            new AttributesDescriptor(StratosBundle.message("stratos.color.settings.description.unresolved.instance.member.reference"),
                    UNRESOLVED_INSTANCE_MEMBER_REFERENCE),

    };

    @Nullable
    @Override
    public Icon getIcon() {
        return StratosIcons.StratosFileIcon;
    }

    @NotNull
    @Override
    public SyntaxHighlighter getHighlighter() {
        return null;
//        return new StratosSyntaxHighlighter();
    }

    @NotNull
    @Override
    public String getDemoText() {
        return "fn main (args :vargs){\n" +
                "        // print \n" +
                "       println('Hello there')\n" +
                "\n" +
                "       // create enum \n" +
        "enum Animal {"+
       "     Pig , Cow , Chicken, Lion "+
       " }"+
                "\n // immutable , non nullable  variable assignment  " +
                "       val  animal  = Animal.Cow\n" +
                "\n // switch/ else if condition " +
                "       when (race){\n" +
                "          Chicken -> {\n" +
                " print('its a chicken');"+
                "          }\n" +
                "          else ->{\n" +
                " print('animal is unknown');"+
                "          }\n" +
                "       }\n" +
                "\n" +
                "\n" +
                "}";
    }

    @Nullable
    @Override
    public Map<String, TextAttributesKey> getAdditionalHighlightingTagToDescriptorMap() {
        return null;
    }

    @NotNull
    @Override
    public AttributesDescriptor[] getAttributeDescriptors() {
        return DESCRIPTORS;
    }

    @NotNull
    @Override
    public ColorDescriptor[] getColorDescriptors() {
        return ColorDescriptor.EMPTY_ARRAY;
    }

    @NotNull
    @Override
    public String getDisplayName() {
        return "Stratos";
    }

}