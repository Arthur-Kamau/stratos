package lang.stratos.language;

import com.intellij.psi.NavigatablePsiElement;
        import com.intellij.psi.tree.IElementType;

public interface StratosPsiCompositeElement extends NavigatablePsiElement {
    IElementType getTokenType();
}
