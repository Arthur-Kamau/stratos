package com.araizen.stratos.psi;
 
//import com.intellij.lang.java.JavaLanguage;
import com.araizen.stratos.files.StratosLanguage;
import com.intellij.psi.tree.IElementType;
import org.jetbrains.annotations.NonNls;
import org.jetbrains.annotations.NotNull;

public class StratosElementType extends IElementType {
    private final boolean myLeftBound;

//    public StratosElementType(@NotNull @NonNls String debugName) {
//        super(debugName, StratosLanguage.INSTANCE);}

    public StratosElementType(@NonNls final String debugName) {
        this(debugName, false);
    }

    public StratosElementType(@NonNls final String debugName, final boolean leftBound) {
        super(debugName, StratosLanguage.INSTANCE);
        myLeftBound = leftBound;
    }

    @Override
    public boolean isLeftBound() {
        return myLeftBound;
    }
}