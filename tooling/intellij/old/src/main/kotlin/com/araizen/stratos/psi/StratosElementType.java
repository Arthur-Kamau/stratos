package com.araizen.stratos.psi;

import com.araizen.stratos.files.StratosLanguage;
import com.intellij.psi.tree.IElementType;
import org.jetbrains.annotations.NonNls;
import org.jetbrains.annotations.NotNull;

public class StratosElementType extends IElementType {
    public StratosElementType(@NotNull @NonNls String debugName) {
        super(debugName, StratosLanguage.INSTANCE);
    }
}