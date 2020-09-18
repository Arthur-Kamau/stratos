package lang.stratos.language;


import com.intellij.extapi.psi.PsiFileBase;
import com.intellij.openapi.fileTypes.FileType;
import com.intellij.psi.FileViewProvider;
import com.intellij.psi.PsiElement;
import com.intellij.psi.ResolveState;
import com.intellij.psi.scope.PsiScopeProcessor;
import com.intellij.psi.search.GlobalSearchScope;
import com.intellij.psi.search.SearchScope;
import com.intellij.psi.tree.IElementType;
//import com.jetbrains.lang.dart.DartFileType;
//import com.jetbrains.lang.dart.DartLanguage;
//import com.jetbrains.lang.dart.psi.impl.DartPsiCompositeElementImpl;
import org.jetbrains.annotations.NotNull;



public class StratosFile extends PsiFileBase implements StratosExecutionScope {
    public StratosFile(@NotNull FileViewProvider viewProvider) {
        super(viewProvider, StratosLanguage.INSTANCE);
    }

    @NotNull
    @Override
    public FileType getFileType() {
        return StratosFileType.INSTANCE;
    }

    @Override
    public String toString() {
        return "Stratos File";
    }

    @NotNull
    @Override
    public SearchScope getUseScope() {
        // There are corner cases when file from a project may be used in a library, or from a different module without any dependency, etc.
        return GlobalSearchScope.allScope(getProject());
    }

    @Override
    public boolean processDeclarations(@NotNull PsiScopeProcessor processor,
                                       @NotNull ResolveState state,
                                       PsiElement lastParent,
                                       @NotNull PsiElement place) {
        return StratosPsiCompositeElementImpl.processDeclarationsImpl(this, processor, state, lastParent)
                && super.processDeclarations(processor, state, lastParent, place);
    }

    @Override
    public IElementType getTokenType() {
        return getNode().getElementType();
    }
}
