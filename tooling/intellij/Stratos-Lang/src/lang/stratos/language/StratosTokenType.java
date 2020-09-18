package lang.stratos.language;

import com.intellij.psi.tree.IElementType;
import org.jetbrains.annotations.NonNls;
import org.jetbrains.annotations.NotNull;
public class StratosTokenType extends IElementType {

    public StratosTokenType(@NotNull @NonNls String debugName) {
        super(debugName, StratosLanguage.INSTANCE);
    }

    @Override
    public String toString() {
        return "StratosTokenType." + super.toString();
    }

}