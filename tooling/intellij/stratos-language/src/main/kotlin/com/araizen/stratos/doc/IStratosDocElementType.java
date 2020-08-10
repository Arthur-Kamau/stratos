package com.araizen.stratos.doc;


import com.araizen.stratos.files.StratosLanguage;
import com.intellij.psi.tree.IElementType;
import org.jetbrains.annotations.NonNls;

public class IStratosDocElementType extends IElementType {
    public IStratosDocElementType(@NonNls final String debugName) {
        super(debugName, StratosLanguage.INSTANCE);
    }
}