package lang.stratos.grammer;

import com.intellij.extapi.psi.PsiFileBase;
import com.intellij.openapi.fileTypes.FileType;
import com.intellij.psi.FileViewProvider;
import lang.stratos.language.StratosFileType;
import lang.stratos.language.StratosLanguage;
import org.jetbrains.annotations.NotNull;

public class StratosFile  extends PsiFileBase {

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
        return "Simple File";
    }

}

