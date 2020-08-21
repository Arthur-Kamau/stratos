package lang.stratos.grammer;

import com.intellij.psi.tree.IElementType;
import lang.stratos.language.StratosLanguage;

public class StratosTokenType  extends IElementType {

    public StratosTokenType(String debugName) {
        super(debugName, StratosLanguage.INSTANCE);
    }

    @Override
    public String toString() {
        return "StratosType." + super.toString();
    }
}