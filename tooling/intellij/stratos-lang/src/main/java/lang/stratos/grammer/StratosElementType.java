package lang.stratos.grammer;

import com.intellij.psi.tree.IElementType;
import lang.stratos.language.StratosLanguage;

public class StratosElementType extends IElementType {

    public StratosElementType(String debugName) {
        super(debugName, StratosLanguage.INSTANCE);
    }
}
