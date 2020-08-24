package lang.stratos.language;


import com.intellij.openapi.editor.colors.TextAttributesKey;
import com.intellij.openapi.fileTypes.SyntaxHighlighter;
import com.intellij.openapi.options.colors.AttributesDescriptor;
import com.intellij.openapi.options.colors.ColorDescriptor;
import com.intellij.openapi.options.colors.ColorSettingsPage;
import lang.stratos.icons.StratosIcons;
import org.jetbrains.annotations.NotNull;

import javax.annotation.Nullable;
import javax.swing.*;
import java.util.Map;

public class StratosColorSettingsPage implements ColorSettingsPage {

    private static final AttributesDescriptor[] DESCRIPTORS = new AttributesDescriptor[]{
            new AttributesDescriptor("Key", StratosSyntaxHighlighter.KEY),
            new AttributesDescriptor("Separator", StratosSyntaxHighlighter.SEPARATOR),
            new AttributesDescriptor("Value", StratosSyntaxHighlighter.VALUE),
            new AttributesDescriptor("Bad Value", StratosSyntaxHighlighter.BAD_CHARACTER)
    };

    @Nullable
    @Override
    public Icon getIcon() {
        return StratosIcons.StratosFileIcon;
    }

    @NotNull
    @Override
    public SyntaxHighlighter getHighlighter() {
        return new StratosSyntaxHighlighter();
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