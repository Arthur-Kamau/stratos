package lang.stratos.util;

import com.intellij.psi.tree.IElementType;
import lang.stratos.language.StratosLanguage;

public class StratosElementType extends IElementType {
    public StratosElementType(String debug_description) {
        super(debug_description, StratosLanguage.INSTANCE);
    }
}
