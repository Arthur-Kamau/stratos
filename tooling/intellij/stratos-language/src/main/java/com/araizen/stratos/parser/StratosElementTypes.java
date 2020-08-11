package com.araizen.stratos.parser;

import com.intellij.psi.tree.IElementType;

public interface StratosElementTypes {

    final IElementType COMMENT = new StratosElementType("COMMENT");
    final IElementType STRING = new StratosElementType("STRING");
    final IElementType DIGITS = new StratosElementType("DIGITS");
    final IElementType IDENTIFIER = new StratosElementType("IDENTIFIER");
    final IElementType KEYWORD = new StratosElementType("KEYWORD");
    final IElementType WHITE_SPACE = new StratosElementType("WHITE_SPACE");
}
