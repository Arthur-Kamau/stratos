package lang.stratos.commentor;



import com.intellij.lang.CodeDocumentationAwareCommenter;
        import com.intellij.psi.PsiComment;
        import com.intellij.psi.tree.IElementType;
//        import com.jetbrains.lang.dart.StratosTokenSets;
import lang.stratos.util.StratosTokenSets;
import org.jetbrains.annotations.Nullable;

public class StratosCommenter implements CodeDocumentationAwareCommenter {
    @Override
    public String getLineCommentPrefix() {
        return "//";
    }

    @Override
    public String getBlockCommentPrefix() {
        return "/**";
    }

    @Override
    public String getBlockCommentSuffix() {
        return "*/";
    }

    @Override
    public String getCommentedBlockCommentPrefix() {
        return "*";
    }

    @Override
    public String getCommentedBlockCommentSuffix() {
        return null;
    }

    @Override
    @Nullable
    public IElementType getLineCommentTokenType() {
        return StratosTokenSets.SINGLE_LINE_COMMENT;
    }

    @Override
    @Nullable
    public IElementType getBlockCommentTokenType() {
        return StratosTokenSets.MULTI_LINE_COMMENT;
    }

    @Override
    public String getDocumentationCommentPrefix() {
        return "/**";
    }

    @Override
    public String getDocumentationCommentLinePrefix() {
        return "*";
    }

    @Override
    public String getDocumentationCommentSuffix() {
        return "*/";
    }

    @Override
    public boolean isDocumentationComment(final PsiComment element) {
//        return element.getTokenType() == StratosTokenSets.SINGLE_LINE_DOC_COMMENT ||
//                element.getTokenType() == StratosTokenSets.MULTI_LINE_DOC_COMMENT;
        return true;
    }

    @Override
    @Nullable
//    public IElementType getDocumentationCommentTokenType() {
//        return StratosTokenSets.SINGLE_LINE_DOC_COMMENT;
//    }
    public IElementType getDocumentationCommentTokenType() {
        return null;
    }
}
