package lang.stratos.language;
import com.intellij.psi.tree.IElementType;
import org.jetbrains.annotations.NonNls;
import org.jetbrains.annotations.NotNull;
public class StratosElementType extends IElementType {

    public StratosElementType(@NotNull @NonNls String debugName) {
        super(debugName, StratosLanguage.INSTANCE);
    }

}
