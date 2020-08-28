package lang.stratos.grammer.types;

import com.intellij.psi.impl.source.tree.LazyParseablePsiElement;
        import com.intellij.psi.tree.IElementType;

        import org.jetbrains.annotations.NotNull;
        import org.jetbrains.annotations.Nullable;

public class StratosLazyParseableBlockImpl extends LazyParseablePsiElement implements StratosLazyParseableBlock {
    public StratosLazyParseableBlockImpl(@NotNull final IElementType type, @Nullable final CharSequence buffer) {
        super(type, buffer);
    }

//    @Override
//    public IElementType getTokenType() {
//        return getElementType();
//    }

//    @Override
//    @Nullable
//    public DartStatements getStatements() {
//        return findChildByClass(DartStatements.class);
//    }

//    @Override
//    public String toString() {
//        return getTokenType().toString();
//    }
}
